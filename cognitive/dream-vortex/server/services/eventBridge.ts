/**
 * Event Bridge Service - Bidirectional Narrative-Business Event Propagation
 * 
 * This service creates the feedback loop between DreamCog narrative events
 * and Vorticog business events, enabling emergent storytelling where
 * gameplay generates narrative and narrative affects gameplay.
 */

import { eq, and, lte } from "drizzle-orm";
import { drizzle } from "drizzle-orm/mysql2";
import {
  worldEvents,
  WorldEvent,
  InsertWorldEvent,
  scheduledWorldEvents,
  ScheduledWorldEvent,
  InsertScheduledWorldEvent,
  eventPropagation,
  InsertEventPropagation,
  companies,
  agents,
  agentMemories,
  notifications,
  marketListings,
  gameState,
} from "../../drizzle/schema";

// ============================================================================
// Types
// ============================================================================

export interface BusinessEvent {
  type: "bankruptcy" | "merger" | "market_crash" | "expansion" | "layoff" | "innovation" | "scandal" | "success";
  companyId: number;
  magnitude: number; // 1-100
  description: string;
  affectedResources?: number[];
  affectedCities?: number[];
}

export interface NarrativeEvent {
  type: "conflict" | "alliance" | "betrayal" | "discovery" | "crisis" | "celebration" | "tragedy";
  worldId: number;
  importance: number; // 1-100
  description: string;
  affectedAgentIds?: number[];
}

export interface PropagationResult {
  sourceType: "business" | "narrative";
  targetEvents: Array<{
    type: string;
    description: string;
    effects: Record<string, unknown>;
  }>;
}

// ============================================================================
// Database Helper
// ============================================================================

let _db: ReturnType<typeof drizzle> | null = null;

function getDb() {
  if (!_db && process.env.DATABASE_URL) {
    _db = drizzle(process.env.DATABASE_URL);
  }
  return _db;
}

// ============================================================================
// Event Bridge Service
// ============================================================================

export class EventBridgeService {
  /**
   * Process a business event and propagate to narrative system
   */
  async propagateBusinessEvent(event: BusinessEvent): Promise<PropagationResult> {
    const db = getDb();
    if (!db) throw new Error("Database not available");
    
    const targetEvents: PropagationResult["targetEvents"] = [];
    
    // Get current game state for turn number
    const [state] = await db.select().from(gameState).limit(1);
    const currentTurn = state?.currentTurn || 1;
    
    // Create world event based on business event type
    const worldEventData = this.businessToNarrativeMapping(event, currentTurn);
    
    if (worldEventData && worldEventData.eventType && worldEventData.title) {
      // Get or create default world
      let worldId = 1;
      const [existingEvent] = await db.select().from(worldEvents).limit(1);
      if (existingEvent) worldId = existingEvent.worldId;
      
      await db.insert(worldEvents).values({
        worldId,
        eventType: worldEventData.eventType,
        title: worldEventData.title,
        description: worldEventData.description || "",
        importance: worldEventData.importance || 50,
        eventDate: new Date().toISOString(),
        consequences: worldEventData.consequences,
      });
      
      targetEvents.push({
        type: worldEventData.eventType || "economic",
        description: worldEventData.description || "",
        effects: {},
      });
    }
    
    // Affect agents emotionally based on business event
    await this.affectAgentsFromBusinessEvent(event);
    
    // Log the propagation
    await db.insert(eventPropagation).values({
      sourceType: "business",
      sourceEventId: event.companyId,
      sourceEventType: event.type,
      targetType: "narrative",
      targetEventType: worldEventData?.eventType || "unknown",
      propagationData: { event, targetEvents },
    });
    
    return { sourceType: "business", targetEvents };
  }
  
  /**
   * Process a narrative event and propagate to business system
   */
  async propagateNarrativeEvent(event: NarrativeEvent): Promise<PropagationResult> {
    const db = getDb();
    if (!db) throw new Error("Database not available");
    
    const targetEvents: PropagationResult["targetEvents"] = [];
    
    // Map narrative event to business effects
    const businessEffects = this.narrativeToBusinessMapping(event);
    
    // Apply market effects
    if (businessEffects.marketPriceModifier) {
      for (const [resourceId, modifier] of Object.entries(businessEffects.marketPriceModifier)) {
        // Update market listings prices
        const listings = await db.select().from(marketListings).where(eq(marketListings.resourceTypeId, parseInt(resourceId)));
        for (const listing of listings) {
          const newPrice = parseFloat(listing.pricePerUnit) * (1 + modifier);
          await db.update(marketListings)
            .set({ pricePerUnit: newPrice.toFixed(2) })
            .where(eq(marketListings.id, listing.id));
        }
        
        targetEvents.push({
          type: "market_adjustment",
          description: `Market prices adjusted by ${(modifier * 100).toFixed(1)}% due to ${event.type}`,
          effects: { resourceId, modifier },
        });
      }
    }
    
    // Affect company reputations
    if (businessEffects.reputationModifier && event.affectedAgentIds) {
      for (const agentId of event.affectedAgentIds) {
        const [agent] = await db.select().from(agents).where(eq(agents.id, agentId)).limit(1);
        if (agent?.companyId) {
          const [company] = await db.select().from(companies).where(eq(companies.id, agent.companyId)).limit(1);
          if (company) {
            const newReputation = Math.max(0, Math.min(100, company.reputation + businessEffects.reputationModifier));
            await db.update(companies)
              .set({ reputation: newReputation })
              .where(eq(companies.id, company.id));
            
            targetEvents.push({
              type: "reputation_change",
              description: `Company reputation changed by ${businessEffects.reputationModifier} due to ${event.type}`,
              effects: { companyId: company.id, change: businessEffects.reputationModifier },
            });
          }
        }
      }
    }
    
    // Create notifications for affected players
    if (event.affectedAgentIds) {
      for (const agentId of event.affectedAgentIds) {
        const [agent] = await db.select().from(agents).where(eq(agents.id, agentId)).limit(1);
        if (agent?.companyId) {
          const [company] = await db.select().from(companies).where(eq(companies.id, agent.companyId)).limit(1);
          if (company) {
            await db.insert(notifications).values({
              userId: company.userId,
              type: event.type === "crisis" || event.type === "tragedy" ? "warning" : "info",
              title: `World Event: ${event.type.charAt(0).toUpperCase() + event.type.slice(1)}`,
              message: event.description,
            });
          }
        }
      }
    }
    
    // Log the propagation
    await db.insert(eventPropagation).values({
      sourceType: "narrative",
      sourceEventId: event.worldId,
      sourceEventType: event.type,
      targetType: "business",
      targetEventType: "market_effects",
      propagationData: { event, targetEvents, businessEffects },
    });
    
    return { sourceType: "narrative", targetEvents };
  }
  
  /**
   * Process scheduled events that are due
   */
  async processScheduledEvents(): Promise<void> {
    const db = getDb();
    if (!db) return;
    
    const now = new Date();
    
    // Get all pending scheduled events that are due
    const dueEvents = await db.select()
      .from(scheduledWorldEvents)
      .where(and(
        eq(scheduledWorldEvents.status, "pending"),
        lte(scheduledWorldEvents.scheduledFor, now)
      ));
    
    for (const event of dueEvents) {
      try {
        // Create the world event
        await db.insert(worldEvents).values({
          worldId: event.worldId,
          eventType: "other",
          title: event.eventName,
          description: event.description || "",
          importance: event.priority || 5,
          eventDate: now.toISOString(),
        });
        
        // Mark as completed
        await db.update(scheduledWorldEvents)
          .set({ status: "completed" })
          .where(eq(scheduledWorldEvents.id, event.id));
        
        // If recurring, create next occurrence
        if (event.isRecurring) {
          const nextDate = new Date(event.scheduledFor);
          nextDate.setDate(nextDate.getDate() + 7); // Default to weekly
          
          await db.insert(scheduledWorldEvents).values({
            worldId: event.worldId,
            eventName: event.eventName,
            description: event.description,
            scheduledFor: nextDate,
            eventTrigger: event.eventTrigger,
            targetAgentIds: event.targetAgentIds,
            targetLocationId: event.targetLocationId,
            priority: event.priority,
            isRecurring: true,
          });
        }
      } catch (error) {
        console.error(`Failed to process scheduled event ${event.id}:`, error);
      }
    }
  }
  
  /**
   * Schedule a future event
   */
  async scheduleEvent(data: InsertScheduledWorldEvent): Promise<ScheduledWorldEvent> {
    const db = getDb();
    if (!db) throw new Error("Database not available");
    
    const [result] = await db.insert(scheduledWorldEvents).values(data).$returningId();
    const [created] = await db.select().from(scheduledWorldEvents).where(eq(scheduledWorldEvents.id, result.id)).limit(1);
    return created;
  }
  
  // ============================================================================
  // Private Helper Methods
  // ============================================================================
  
  private businessToNarrativeMapping(event: BusinessEvent, currentTurn: number): Partial<InsertWorldEvent> | null {
    const mappings: Record<BusinessEvent["type"], Partial<InsertWorldEvent>> = {
      bankruptcy: {
        eventType: "economic",
        title: "Corporate Collapse",
        description: `A major company has declared bankruptcy, sending shockwaves through the market. ${event.description}`,
        importance: Math.min(10, Math.ceil(event.magnitude / 10)),
        consequences: "Economic uncertainty spreads as suppliers and employees face uncertain futures.",
      },
      merger: {
        eventType: "political",
        title: "Corporate Alliance",
        description: `Two powerful entities have merged, reshaping the business landscape. ${event.description}`,
        importance: Math.min(10, Math.ceil(event.magnitude / 12)),
        consequences: "The balance of power shifts as the new entity gains influence.",
      },
      market_crash: {
        eventType: "economic",
        title: "Market Turmoil",
        description: `Markets plunge as panic spreads among traders and investors. ${event.description}`,
        importance: Math.min(10, Math.ceil(event.magnitude / 8)),
        consequences: "Widespread financial hardship affects businesses and individuals alike.",
      },
      expansion: {
        eventType: "economic",
        title: "Corporate Expansion",
        description: `A company expands its reach, bringing new opportunities. ${event.description}`,
        importance: Math.min(10, Math.ceil(event.magnitude / 15)),
        consequences: "New jobs and economic activity boost the local economy.",
      },
      layoff: {
        eventType: "social",
        title: "Mass Layoffs",
        description: `Workers face uncertain futures as layoffs sweep through the industry. ${event.description}`,
        importance: Math.min(10, Math.ceil(event.magnitude / 12)),
        consequences: "Families struggle as unemployment rises in the region.",
      },
      innovation: {
        eventType: "discovery",
        title: "Breakthrough Innovation",
        description: `A revolutionary discovery changes the rules of the game. ${event.description}`,
        importance: Math.min(10, Math.ceil(event.magnitude / 10)),
        consequences: "Industries adapt as new technologies reshape the market.",
      },
      scandal: {
        eventType: "political",
        title: "Corporate Scandal",
        description: `Revelations of misconduct shake public trust. ${event.description}`,
        importance: Math.min(10, Math.ceil(event.magnitude / 10)),
        consequences: "Investigations begin as stakeholders demand accountability.",
      },
      success: {
        eventType: "economic",
        title: "Business Triumph",
        description: `A company achieves remarkable success, inspiring others. ${event.description}`,
        importance: Math.min(10, Math.ceil(event.magnitude / 15)),
        consequences: "Confidence grows as success breeds more investment.",
      },
    };
    
    return mappings[event.type] || null;
  }
  
  private narrativeToBusinessMapping(event: NarrativeEvent): {
    marketPriceModifier?: Record<string, number>;
    reputationModifier?: number;
  } {
    const mappings: Record<NarrativeEvent["type"], { marketPriceModifier?: Record<string, number>; reputationModifier?: number }> = {
      conflict: {
        marketPriceModifier: { "1": 0.15, "2": 0.10 }, // Increase prices during conflict
        reputationModifier: -10,
      },
      alliance: {
        reputationModifier: 15,
      },
      betrayal: {
        reputationModifier: -25,
      },
      discovery: {
        marketPriceModifier: { "1": -0.10 }, // New discoveries can lower prices
        reputationModifier: 10,
      },
      crisis: {
        marketPriceModifier: { "1": 0.25, "2": 0.20, "3": 0.15 },
        reputationModifier: -15,
      },
      celebration: {
        reputationModifier: 5,
      },
      tragedy: {
        marketPriceModifier: { "1": 0.10 },
        reputationModifier: -5,
      },
    };
    
    const baseEffects = mappings[event.type] || {};
    
    // Scale effects by importance
    const scale = event.importance / 50;
    if (baseEffects.marketPriceModifier) {
      for (const key of Object.keys(baseEffects.marketPriceModifier)) {
        baseEffects.marketPriceModifier[key] *= scale;
      }
    }
    if (baseEffects.reputationModifier) {
      baseEffects.reputationModifier = Math.round(baseEffects.reputationModifier * scale);
    }
    
    return baseEffects;
  }
  
  private async affectAgentsFromBusinessEvent(event: BusinessEvent): Promise<void> {
    const db = getDb();
    if (!db) return;
    
    // Get agents associated with the company
    const companyAgents = await db.select().from(agents).where(eq(agents.companyId, event.companyId));
    
    for (const agent of companyAgents) {
      let happinessChange = 0;
      let stressChange = 0;
      let loyaltyChange = 0;
      
      switch (event.type) {
        case "bankruptcy":
          happinessChange = -30;
          stressChange = 40;
          loyaltyChange = -20;
          break;
        case "layoff":
          happinessChange = -20;
          stressChange = 30;
          loyaltyChange = -15;
          break;
        case "success":
          happinessChange = 20;
          stressChange = -10;
          loyaltyChange = 10;
          break;
        case "expansion":
          happinessChange = 10;
          stressChange = 5;
          loyaltyChange = 5;
          break;
        case "scandal":
          happinessChange = -15;
          stressChange = 20;
          loyaltyChange = -25;
          break;
        case "innovation":
          happinessChange = 15;
          stressChange = -5;
          loyaltyChange = 10;
          break;
      }
      
      // Apply changes with bounds
      const newHappiness = Math.max(0, Math.min(100, agent.happiness + happinessChange));
      const newStress = Math.max(0, Math.min(100, agent.stress + stressChange));
      const newLoyalty = Math.max(0, Math.min(100, agent.loyalty + loyaltyChange));
      
      await db.update(agents)
        .set({ happiness: newHappiness, stress: newStress, loyalty: newLoyalty })
        .where(eq(agents.id, agent.id));
      
      // Create memory of the event
      await db.insert(agentMemories).values({
        agentId: agent.id,
        memoryType: event.type === "success" || event.type === "innovation" ? "achievement" : 
                   event.type === "bankruptcy" || event.type === "layoff" ? "trauma" : "event",
        content: `Experienced ${event.type}: ${event.description}`,
        emotionalImpact: happinessChange,
        importance: Math.ceil(event.magnitude / 10),
        memoryDate: new Date(),
      });
    }
  }
}

export const eventBridge = new EventBridgeService();
