/**
 * World-Economy Synchronization Service
 * 
 * This service connects world-building elements to market dynamics,
 * ensuring that narrative world state affects the business simulation
 * and vice versa.
 */

import { eq, and, desc } from "drizzle-orm";
import { drizzle } from "drizzle-orm/mysql2";
import {
  worlds,
  World,
  InsertWorld,
  worldEvents,
  WorldEvent,
  cities,
  resourceTypes,
  marketListings,
  gameState,
  loreEntries,
  InsertLoreEntry,
  companies,
  notifications,
} from "../../drizzle/schema";

// ============================================================================
// Types
// ============================================================================

export interface WorldState {
  world: World;
  activeEvents: WorldEvent[];
  marketModifiers: MarketModifiers;
  economicIndicators: EconomicIndicators;
}

export interface MarketModifiers {
  globalPriceMultiplier: number;
  globalDemandMultiplier: number;
  globalSupplyMultiplier: number;
  resourceModifiers: Record<number, { price: number; demand: number; supply: number }>;
  cityModifiers: Record<number, { taxRate: number; dangerLevel: number; shippingCost: number }>;
}

export interface EconomicIndicators {
  marketHealth: number; // 0-100
  inflation: number; // percentage
  unemployment: number; // percentage
  consumerConfidence: number; // 0-100
  tradeVolume: number;
}

export interface LocationEffect {
  cityId: number;
  dangerLevel: number; // 0-100
  shippingCostMultiplier: number;
  productionEfficiencyModifier: number;
  description: string;
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
// World Economy Service
// ============================================================================

export class WorldEconomyService {
  /**
   * Get the current world state including all active modifiers
   */
  async getWorldState(worldId: number): Promise<WorldState | null> {
    const db = getDb();
    if (!db) return null;
    
    const [world] = await db.select().from(worlds).where(eq(worlds.id, worldId)).limit(1);
    if (!world) return null;
    
    // Get recent world events (use importance as activity indicator)
    const activeEvents = await db.select()
      .from(worldEvents)
      .where(eq(worldEvents.worldId, worldId))
      .orderBy(desc(worldEvents.importance))
      .limit(10);
    
    // Calculate market modifiers from active events
    const marketModifiers = this.calculateMarketModifiers(activeEvents);
    
    // Calculate economic indicators
    const economicIndicators = await this.calculateEconomicIndicators(worldId);
    
    return {
      world,
      activeEvents,
      marketModifiers,
      economicIndicators,
    };
  }
  
  /**
   * Create or update a world
   */
  async createWorld(data: InsertWorld): Promise<World> {
    const db = getDb();
    if (!db) throw new Error("Database not available");
    
    const [result] = await db.insert(worlds).values(data).$returningId();
    const [created] = await db.select().from(worlds).where(eq(worlds.id, result.id)).limit(1);
    
    // Create initial lore entries
    await this.generateInitialLore(result.id, data);
    
    return created;
  }
  
  /**
   * Apply world event effects to the economy
   */
  async applyWorldEventEffects(eventId: number): Promise<void> {
    const db = getDb();
    if (!db) return;
    
    const [event] = await db.select().from(worldEvents).where(eq(worldEvents.id, eventId)).limit(1);
    if (!event) return;
    
    // Parse consequences as effects if available
    const consequenceText = event.consequences || "";
    const effects = this.parseConsequencesToEffects(consequenceText, event.eventType);
    
    // Apply price modifiers to market listings based on event type
    if (effects.marketPriceModifier) {
      for (const [resourceKey, modifier] of Object.entries(effects.marketPriceModifier)) {
        if (resourceKey === "all") {
          // Apply to all listings
          const listings = await db.select().from(marketListings).where(eq(marketListings.isActive, true));
          for (const listing of listings) {
            const newPrice = parseFloat(listing.pricePerUnit) * (1 + modifier);
            await db.update(marketListings)
              .set({ pricePerUnit: Math.max(0.01, newPrice).toFixed(2) })
              .where(eq(marketListings.id, listing.id));
          }
        } else {
          const listings = await db.select()
            .from(marketListings)
            .where(and(
              eq(marketListings.resourceTypeId, parseInt(resourceKey)),
              eq(marketListings.isActive, true)
            ));
          for (const listing of listings) {
            const newPrice = parseFloat(listing.pricePerUnit) * (1 + modifier);
            await db.update(marketListings)
              .set({ pricePerUnit: Math.max(0.01, newPrice).toFixed(2) })
              .where(eq(marketListings.id, listing.id));
          }
        }
      }
    }
    
    // Notify affected companies
    await this.notifyAffectedCompanies(event);
  }
  
  /**
   * Get location effects for a city based on world state
   */
  async getLocationEffects(cityId: number, worldId: number): Promise<LocationEffect> {
    const db = getDb();
    if (!db) {
      return {
        cityId,
        dangerLevel: 0,
        shippingCostMultiplier: 1,
        productionEfficiencyModifier: 1,
        description: "Normal conditions",
      };
    }
    
    // Get recent events for this world
    const recentEvents = await db.select()
      .from(worldEvents)
      .where(eq(worldEvents.worldId, worldId))
      .orderBy(desc(worldEvents.createdAt))
      .limit(20);
    
    let dangerLevel = 0;
    let shippingCostMultiplier = 1;
    let productionEfficiencyModifier = 1;
    const descriptions: string[] = [];
    
    for (const event of recentEvents) {
      // Check if event affects this location
      if (event.locationId && event.locationId !== cityId) continue;
      
      // Apply effects based on event type
      switch (event.eventType) {
        case "battle":
          dangerLevel += 30;
          shippingCostMultiplier *= 1.5;
          productionEfficiencyModifier *= 0.7;
          descriptions.push(`Conflict: ${event.title}`);
          break;
        case "natural":
          dangerLevel += 20;
          shippingCostMultiplier *= 1.3;
          productionEfficiencyModifier *= 0.8;
          descriptions.push(`Natural disaster: ${event.title}`);
          break;
        case "political":
          shippingCostMultiplier *= 1.2;
          descriptions.push(`Political instability: ${event.title}`);
          break;
        case "economic":
          productionEfficiencyModifier *= 0.9;
          descriptions.push(`Economic event: ${event.title}`);
          break;
        case "magical":
          productionEfficiencyModifier *= 1.1;
          descriptions.push(`Magical event: ${event.title}`);
          break;
        case "social":
          // Minor effects
          descriptions.push(`Social event: ${event.title}`);
          break;
        case "discovery":
          productionEfficiencyModifier *= 1.15;
          descriptions.push(`Discovery: ${event.title}`);
          break;
      }
    }
    
    return {
      cityId,
      dangerLevel: Math.min(100, dangerLevel),
      shippingCostMultiplier,
      productionEfficiencyModifier,
      description: descriptions.length > 0 ? descriptions.join("; ") : "Normal conditions",
    };
  }
  
  // ============================================================================
  // Private Helper Methods
  // ============================================================================
  
  private calculateMarketModifiers(events: WorldEvent[]): MarketModifiers {
    const modifiers: MarketModifiers = {
      globalPriceMultiplier: 1,
      globalDemandMultiplier: 1,
      globalSupplyMultiplier: 1,
      resourceModifiers: {},
      cityModifiers: {},
    };
    
    for (const event of events) {
      const impactScale = event.importance / 10; // 0.1 to 1.0
      
      switch (event.eventType) {
        case "battle":
          modifiers.globalPriceMultiplier *= 1 + (0.1 * impactScale);
          modifiers.globalSupplyMultiplier *= 1 - (0.15 * impactScale);
          break;
        case "economic":
          modifiers.globalDemandMultiplier *= 1 - (0.1 * impactScale);
          break;
        case "discovery":
          modifiers.globalSupplyMultiplier *= 1 + (0.1 * impactScale);
          break;
        case "natural":
          modifiers.globalSupplyMultiplier *= 1 - (0.2 * impactScale);
          modifiers.globalPriceMultiplier *= 1 + (0.15 * impactScale);
          break;
        case "political":
          modifiers.globalDemandMultiplier *= 1 - (0.05 * impactScale);
          break;
      }
    }
    
    return modifiers;
  }
  
  private async calculateEconomicIndicators(worldId: number): Promise<EconomicIndicators> {
    const db = getDb();
    if (!db) {
      return {
        marketHealth: 50,
        inflation: 2,
        unemployment: 5,
        consumerConfidence: 50,
        tradeVolume: 0,
      };
    }
    
    // Get market data
    const listings = await db.select().from(marketListings).where(eq(marketListings.isActive, true));
    
    // Calculate indicators
    const avgPrice = listings.length > 0 
      ? listings.reduce((sum, l) => sum + parseFloat(l.pricePerUnit), 0) / listings.length 
      : 10;
    
    const marketHealth = Math.min(100, Math.max(0, 50 + (listings.length * 2) - (avgPrice > 20 ? 10 : 0)));
    const inflation = Math.max(0, (avgPrice - 10) / 10 * 5); // Base price assumed 10
    const tradeVolume = listings.reduce((sum, l) => sum + parseInt(l.quantity), 0);
    
    return {
      marketHealth,
      inflation,
      unemployment: Math.max(0, 10 - marketHealth / 10),
      consumerConfidence: marketHealth,
      tradeVolume,
    };
  }
  
  private async generateInitialLore(worldId: number, worldData: InsertWorld): Promise<void> {
    const db = getDb();
    if (!db) return;
    
    const loreEntries_data: InsertLoreEntry[] = [
      {
        worldId,
        category: "history",
        title: `The Founding of ${worldData.name}`,
        content: `${worldData.name} was established as a realm of ${worldData.genre || "fantasy"} wonder. ${worldData.description || ""}`,
        isPublic: true,
        isSecret: false,
      },
      {
        worldId,
        category: "culture",
        title: `Customs and Traditions`,
        content: `The inhabitants of ${worldData.name} follow ancient traditions passed down through generations.`,
        isPublic: true,
        isSecret: false,
      },
    ];
    
    for (const entry of loreEntries_data) {
      await db.insert(loreEntries).values(entry);
    }
  }
  
  private parseConsequencesToEffects(consequences: string, eventType: WorldEvent["eventType"]): {
    marketPriceModifier?: Record<string, number>;
    taxRateModifier?: number;
  } {
    const effects: { marketPriceModifier?: Record<string, number>; taxRateModifier?: number } = {};
    
    // Generate effects based on event type
    switch (eventType) {
      case "battle":
        effects.marketPriceModifier = { "all": 0.15 };
        break;
      case "economic":
        effects.marketPriceModifier = { "all": -0.10 };
        break;
      case "natural":
        effects.marketPriceModifier = { "all": 0.20 };
        break;
      case "political":
        effects.taxRateModifier = 0.02;
        break;
      case "discovery":
        effects.marketPriceModifier = { "all": -0.05 };
        break;
    }
    
    return effects;
  }
  
  private async notifyAffectedCompanies(event: WorldEvent): Promise<void> {
    const db = getDb();
    if (!db) return;
    
    // Get all companies and notify them of significant events
    if (event.importance >= 7) {
      const allCompanies = await db.select().from(companies).limit(100);
      
      for (const company of allCompanies) {
        await db.insert(notifications).values({
          userId: company.userId,
          type: event.eventType === "battle" || event.eventType === "natural" ? "warning" : "info",
          title: `World Event: ${event.title}`,
          message: event.description || `A significant ${event.eventType} event has occurred.`,
        });
      }
    }
  }
}

export const worldEconomy = new WorldEconomyService();
