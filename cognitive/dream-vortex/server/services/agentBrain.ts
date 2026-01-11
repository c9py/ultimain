/**
 * Agent Brain Service - Personality-Weighted Decision Making
 * Uses Big Five Personality model from agentBigFivePersonality table
 */

import { eq, and, desc } from "drizzle-orm";
import { drizzle } from "drizzle-orm/mysql2";
import {
  agents,
  Agent,
  agentBigFivePersonality,
  AgentBigFivePersonality,
  agentMotivations,
  InsertAgentMotivation,
  agentMemories,
  AgentMemory,
  relationships,
  Relationship,
  agentHistories,
  InsertAgentHistory,
} from "../../drizzle/schema";

// Types
export interface DecisionContext {
  type: "trade" | "negotiation" | "investment" | "hiring" | "partnership" | "conflict" | "cooperation";
  situation: string;
  options: DecisionOption[];
  relatedAgentId?: number;
  relatedCompanyId?: number;
  financialStakes?: number;
  riskLevel?: number;
}

export interface DecisionOption {
  id: string;
  description: string;
  expectedOutcome: string;
  riskLevel: number;
  potentialReward: number;
  requiresCooperation: boolean;
  requiresConflict: boolean;
}

export interface DecisionResult {
  chosenOption: DecisionOption;
  reasoning: string;
  confidence: number;
  personalityInfluence: PersonalityInfluence;
  emotionalInfluence: EmotionalInfluence;
}

export interface PersonalityInfluence {
  openness: number;
  conscientiousness: number;
  extraversion: number;
  agreeableness: number;
  neuroticism: number;
}

export interface EmotionalInfluence {
  mood: number;
  stress: number;
  trust: number;
}

interface ScoredOption {
  option: DecisionOption;
  score: {
    total: number;
    personalityInfluence: PersonalityInfluence;
    emotionalInfluence: EmotionalInfluence;
  };
}

let _db: ReturnType<typeof drizzle> | null = null;

function getDb() {
  if (!_db && process.env.DATABASE_URL) {
    _db = drizzle(process.env.DATABASE_URL);
  }
  return _db;
}

export class AgentBrainService {
  async makeDecision(agentId: number, context: DecisionContext): Promise<DecisionResult> {
    const db = getDb();
    if (!db) throw new Error("Database not available");
    
    const [agent] = await db.select().from(agents).where(eq(agents.id, agentId)).limit(1);
    if (!agent) throw new Error(`Agent ${agentId} not found`);
    
    // Get Big Five personality from separate table
    const [personality] = await db.select().from(agentBigFivePersonality).where(eq(agentBigFivePersonality.agentId, agentId)).limit(1);
    const memories = await db.select().from(agentMemories).where(eq(agentMemories.agentId, agentId)).orderBy(desc(agentMemories.importance)).limit(10);
    
    let relationship: Relationship | undefined;
    if (context.relatedAgentId) {
      const [rel] = await db.select().from(relationships).where(
        and(
          eq(relationships.agent1Id, Math.min(agentId, context.relatedAgentId)),
          eq(relationships.agent2Id, Math.max(agentId, context.relatedAgentId))
        )
      ).limit(1);
      relationship = rel;
    }
    
    const scoredOptions: ScoredOption[] = context.options.map(option => ({
      option,
      score: this.calculateOptionScore(agent, personality, memories, relationship, option),
    }));
    
    scoredOptions.sort((a, b) => b.score.total - a.score.total);
    const chosen = scoredOptions[0];
    const reasoning = this.generateReasoning(personality, chosen);
    const confidence = this.calculateConfidence(scoredOptions);
    
    // Record decision in agent history
    await db.insert(agentHistories).values({
      agentId,
      happiness: agent.happiness,
      satisfaction: agent.satisfaction,
      stress: agent.stress,
      loyalty: agent.loyalty,
      trust: agent.trust,
      notes: `Decision: ${context.type} - ${reasoning}`,
    });
    
    return { chosenOption: chosen.option, reasoning, confidence, personalityInfluence: chosen.score.personalityInfluence, emotionalInfluence: chosen.score.emotionalInfluence };
  }
  
  private calculateOptionScore(
    agent: Agent, 
    personality: AgentBigFivePersonality | undefined, 
    memories: AgentMemory[], 
    relationship: Relationship | undefined, 
    option: DecisionOption
  ) {
    let score = 50;
    const personalityInfluence: PersonalityInfluence = { openness: 0, conscientiousness: 0, extraversion: 0, agreeableness: 0, neuroticism: 0 };
    
    // Use personality from Big Five table if available, otherwise use defaults
    const openness = personality?.openness ?? 50;
    const conscientiousness = personality?.conscientiousness ?? 50;
    const extraversion = personality?.extraversion ?? 50;
    const agreeableness = personality?.agreeableness ?? 50;
    const neuroticism = personality?.neuroticism ?? 50;
    const impulsiveness = personality?.impulsiveness ?? 50;
    const riskTaking = personality?.riskTaking ?? 50;
    
    const opennessEffect = (openness - 50) / 50;
    if (option.riskLevel > 50) { personalityInfluence.openness = opennessEffect * 15; score += personalityInfluence.openness; }
    
    const conscientiousnessEffect = (conscientiousness - 50) / 50;
    personalityInfluence.conscientiousness = conscientiousnessEffect * (100 - option.riskLevel) / 5;
    score += personalityInfluence.conscientiousness;
    
    const extraversionEffect = (extraversion - 50) / 50;
    if (option.requiresCooperation) { personalityInfluence.extraversion = extraversionEffect * 10; score += personalityInfluence.extraversion; }
    
    const agreeablenessEffect = (agreeableness - 50) / 50;
    if (option.requiresConflict) { personalityInfluence.agreeableness = -agreeablenessEffect * 15; }
    else if (option.requiresCooperation) { personalityInfluence.agreeableness = agreeablenessEffect * 10; }
    score += personalityInfluence.agreeableness;
    
    const neuroticismEffect = (neuroticism - 50) / 50;
    personalityInfluence.neuroticism = -neuroticismEffect * option.riskLevel / 5;
    score += personalityInfluence.neuroticism;
    
    const emotionalInfluence: EmotionalInfluence = { mood: 0, stress: 0, trust: 0 };
    
    // Use agent's emotional state directly
    const moodEffect = (agent.happiness - 50) / 50;
    emotionalInfluence.mood = moodEffect * option.potentialReward / 10;
    score += emotionalInfluence.mood;
    
    const stressEffect = agent.stress / 100;
    emotionalInfluence.stress = -stressEffect * option.riskLevel / 5;
    score += emotionalInfluence.stress;
    
    if (relationship) {
      const trustEffect = (relationship.positivity - 50) / 50;
      emotionalInfluence.trust = trustEffect * 10;
      score += emotionalInfluence.trust;
    }
    
    for (const memory of memories) {
      if (memory.memoryType === "trauma" && option.riskLevel > 70) score -= 10;
      if (memory.memoryType === "achievement" && option.potentialReward > 70) score += 5;
    }
    
    const riskToleranceEffect = (riskTaking - 50) / 50;
    score += riskToleranceEffect * option.riskLevel / 5;
    
    const impulsivenessEffect = (impulsiveness - 50) / 50;
    score += impulsivenessEffect * option.potentialReward / 10;
    
    return { total: Math.max(0, Math.min(100, score)), personalityInfluence, emotionalInfluence };
  }
  
  private generateReasoning(personality: AgentBigFivePersonality | undefined, chosen: ScoredOption): string {
    const reasons: string[] = [];
    const conscientiousness = personality?.conscientiousness ?? 50;
    const openness = personality?.openness ?? 50;
    const agreeableness = personality?.agreeableness ?? 50;
    const neuroticism = personality?.neuroticism ?? 50;
    
    if (conscientiousness > 60 && chosen.option.riskLevel < 50) reasons.push("As a careful planner, I prefer the more measured approach.");
    if (openness > 60 && chosen.option.riskLevel > 50) reasons.push("I'm drawn to the innovative potential of this option.");
    if (agreeableness > 60 && chosen.option.requiresCooperation) reasons.push("Working together seems like the best path forward.");
    if (agreeableness < 40 && chosen.option.requiresConflict) reasons.push("Sometimes you have to be willing to fight for what you want.");
    if (neuroticism > 60 && chosen.option.riskLevel < 40) reasons.push("I'd rather play it safe than risk everything.");
    if (chosen.score.emotionalInfluence.stress < -5) reasons.push("Given the current pressure, I need to be conservative.");
    if (chosen.score.emotionalInfluence.trust > 5) reasons.push("I trust this partner based on our history together.");
    if (chosen.score.emotionalInfluence.mood > 5) reasons.push("I'm feeling optimistic about the potential here.");
    if (reasons.length === 0) reasons.push(`After weighing all factors, "${chosen.option.description}" offers the best balance of risk and reward.`);
    return reasons.join(" ");
  }
  
  private calculateConfidence(scoredOptions: ScoredOption[]): number {
    if (scoredOptions.length < 2) return 100;
    const topScore = scoredOptions[0].score.total;
    const secondScore = scoredOptions[1].score.total;
    return Math.min(100, 50 + (topScore - secondScore) * 2);
  }
  
  async processDecisionOutcome(agentId: number, outcome: "success" | "failure" | "neutral", decisionType: string, reasoning: string): Promise<void> {
    const db = getDb();
    if (!db) return;
    
    const [agent] = await db.select().from(agents).where(eq(agents.id, agentId)).limit(1);
    if (!agent) return;
    
    // Update agent emotional state based on outcome
    if (outcome === "success") {
      await db.update(agents).set({ 
        happiness: Math.min(100, agent.happiness + 10),
        satisfaction: Math.min(100, agent.satisfaction + 15),
        stress: Math.max(0, agent.stress - 10)
      }).where(eq(agents.id, agentId));
    } else if (outcome === "failure") {
      await db.update(agents).set({ 
        happiness: Math.max(0, agent.happiness - 15),
        satisfaction: Math.max(0, agent.satisfaction - 20),
        stress: Math.min(100, agent.stress + 20)
      }).where(eq(agents.id, agentId));
    }
    
    // Create memory of the decision
    await db.insert(agentMemories).values({
      agentId,
      memoryType: outcome === "success" ? "achievement" : outcome === "failure" ? "trauma" : "event",
      content: `Made a ${decisionType} decision: ${reasoning}. Outcome: ${outcome}`,
      emotionalImpact: outcome === "success" ? 30 : outcome === "failure" ? -30 : 0,
      importance: outcome === "failure" ? 70 : 50,
      memoryDate: new Date(),
    });
  }
  
  async createAgentWithPersonality(data: { 
    name: string; 
    type: Agent["type"]; 
    personaId: number;
    cityId: number;
    companyId?: number;
    businessUnitId?: number;
  }): Promise<Agent> {
    const db = getDb();
    if (!db) throw new Error("Database not available");
    
    // Create the agent
    const [result] = await db.insert(agents).values({
      name: data.name,
      type: data.type,
      personaId: data.personaId,
      cityId: data.cityId,
      companyId: data.companyId,
      businessUnitId: data.businessUnitId,
    }).$returningId();
    
    // Create Big Five personality with randomized traits
    await db.insert(agentBigFivePersonality).values({
      agentId: result.id,
      openness: this.randomTrait(),
      conscientiousness: this.randomTrait(),
      extraversion: this.randomTrait(),
      agreeableness: this.randomTrait(),
      neuroticism: this.randomTrait(),
      impulsiveness: this.randomTrait(),
      riskTaking: this.randomTrait(),
      empathy: this.randomTrait(),
      leadership: this.randomTrait(),
      independence: this.randomTrait(),
    });
    
    // Create default motivations based on type
    const motivations = this.getDefaultMotivations(data.type);
    for (const m of motivations) {
      await db.insert(agentMotivations).values({ 
        agentId: result.id, 
        motivationType: m.motivationType!, 
        description: m.description!, 
        priority: m.priority 
      });
    }
    
    const [createdAgent] = await db.select().from(agents).where(eq(agents.id, result.id)).limit(1);
    return createdAgent;
  }
  
  private randomTrait(): number {
    const u1 = Math.random(), u2 = Math.random();
    const normal = Math.sqrt(-2 * Math.log(u1)) * Math.cos(2 * Math.PI * u2);
    return Math.max(0, Math.min(100, Math.round(50 + normal * 15)));
  }
  
  private getDefaultMotivations(type: Agent["type"]): Partial<InsertAgentMotivation>[] {
    const motivations: Record<string, Partial<InsertAgentMotivation>[]> = {
      customer: [
        { motivationType: "short_term", description: "Find quality products at good prices", priority: 80 }, 
        { motivationType: "long_term", description: "Build relationships with reliable suppliers", priority: 60 }
      ],
      supplier: [
        { motivationType: "short_term", description: "Maximize sales volume", priority: 80 }, 
        { motivationType: "long_term", description: "Establish market dominance", priority: 70 }, 
        { motivationType: "core_value", description: "Maintain product quality reputation", priority: 90 }
      ],
      employee: [
        { motivationType: "short_term", description: "Perform well in current role", priority: 70 }, 
        { motivationType: "long_term", description: "Advance career and increase income", priority: 80 }, 
        { motivationType: "core_value", description: "Work-life balance", priority: 60 }
      ],
      partner: [
        { motivationType: "short_term", description: "Successful joint ventures", priority: 75 }, 
        { motivationType: "long_term", description: "Build lasting business alliance", priority: 85 }
      ],
      investor: [
        { motivationType: "short_term", description: "Positive quarterly returns", priority: 85 }, 
        { motivationType: "long_term", description: "Portfolio growth and diversification", priority: 90 }, 
        { motivationType: "core_value", description: "Risk-adjusted returns", priority: 80 }
      ],
      competitor: [
        { motivationType: "short_term", description: "Gain market share", priority: 90 }, 
        { motivationType: "long_term", description: "Industry leadership", priority: 85 }, 
        { motivationType: "core_value", description: "Innovation and differentiation", priority: 75 }
      ],
    };
    return motivations[type] || [];
  }
}

export const agentBrain = new AgentBrainService();
