/**
 * Simulation Engine Service
 * Provides life simulation mechanics inspired by Sims FreePlay
 * Integrated with Dream Vortex agent and world systems
 */

import { getDb } from '../db';
import { agents, worlds, worldEvents, agentMemories } from '../../drizzle/schema';
import { eq, and, desc, sql } from 'drizzle-orm';

// Need decay rates per game hour
const NEED_DECAY_RATES = {
  hunger: 2.5,
  energy: 1.8,
  hygiene: 1.2,
  bladder: 3.0,
  social: 1.5,
  fun: 2.0,
  comfort: 1.0,
};

// Mood thresholds
const MOOD_THRESHOLDS = {
  very_happy: 80,
  happy: 60,
  fine: 40,
  sad: 20,
  very_sad: 0,
};

// Skill gain rates
const SKILL_GAIN_RATES = {
  cooking: 0.5,
  gardening: 0.4,
  fishing: 0.3,
  handiness: 0.4,
  charisma: 0.6,
  creativity: 0.5,
  fitness: 0.4,
  logic: 0.5,
  writing: 0.4,
  music: 0.5,
};

export interface SimulationState {
  agentId: number;
  needs: Record<string, number>;
  skills: Record<string, number>;
  mood: string;
  moodlets: Array<{
    id: string;
    name: string;
    emotion: string;
    intensity: number;
    expiresAt: Date | null;
  }>;
  currentAction: string | null;
  actionProgress: number;
}

export interface SimulationAction {
  id: string;
  name: string;
  category: 'self_care' | 'social' | 'work' | 'leisure' | 'skill';
  duration: number; // minutes
  needEffects: Record<string, number>;
  skillEffects?: Record<string, number>;
  requirements?: {
    needs?: Record<string, number>;
    skills?: Record<string, number>;
  };
}

// Available actions for simulation
const SIMULATION_ACTIONS: SimulationAction[] = [
  // Self Care
  {
    id: 'eat_meal',
    name: 'Eat a Meal',
    category: 'self_care',
    duration: 30,
    needEffects: { hunger: 50, energy: 5, comfort: 10 },
  },
  {
    id: 'quick_snack',
    name: 'Have a Snack',
    category: 'self_care',
    duration: 10,
    needEffects: { hunger: 20 },
  },
  {
    id: 'sleep',
    name: 'Sleep',
    category: 'self_care',
    duration: 480,
    needEffects: { energy: 100, comfort: 30 },
  },
  {
    id: 'nap',
    name: 'Take a Nap',
    category: 'self_care',
    duration: 120,
    needEffects: { energy: 40, comfort: 15 },
  },
  {
    id: 'shower',
    name: 'Take a Shower',
    category: 'self_care',
    duration: 30,
    needEffects: { hygiene: 80, comfort: 10 },
  },
  {
    id: 'use_bathroom',
    name: 'Use Bathroom',
    category: 'self_care',
    duration: 5,
    needEffects: { bladder: 100 },
  },
  
  // Social
  {
    id: 'chat',
    name: 'Have a Chat',
    category: 'social',
    duration: 30,
    needEffects: { social: 25, fun: 10 },
    skillEffects: { charisma: 0.2 },
  },
  {
    id: 'deep_conversation',
    name: 'Deep Conversation',
    category: 'social',
    duration: 60,
    needEffects: { social: 50, fun: 15 },
    skillEffects: { charisma: 0.5 },
    requirements: { skills: { charisma: 3 } },
  },
  {
    id: 'throw_party',
    name: 'Throw a Party',
    category: 'social',
    duration: 180,
    needEffects: { social: 80, fun: 60, energy: -20 },
    skillEffects: { charisma: 1.0 },
    requirements: { needs: { energy: 40 } },
  },
  
  // Leisure
  {
    id: 'watch_tv',
    name: 'Watch TV',
    category: 'leisure',
    duration: 60,
    needEffects: { fun: 30, comfort: 15, energy: -5 },
  },
  {
    id: 'play_games',
    name: 'Play Video Games',
    category: 'leisure',
    duration: 90,
    needEffects: { fun: 50, social: 10 },
    skillEffects: { logic: 0.2 },
  },
  {
    id: 'read_book',
    name: 'Read a Book',
    category: 'leisure',
    duration: 60,
    needEffects: { fun: 20, comfort: 10 },
    skillEffects: { logic: 0.3, writing: 0.1 },
  },
  {
    id: 'exercise',
    name: 'Exercise',
    category: 'leisure',
    duration: 60,
    needEffects: { fun: 20, energy: -15, hygiene: -20 },
    skillEffects: { fitness: 0.8 },
  },
  {
    id: 'meditate',
    name: 'Meditate',
    category: 'leisure',
    duration: 30,
    needEffects: { comfort: 30, energy: 10 },
  },
  
  // Skill Building
  {
    id: 'practice_cooking',
    name: 'Practice Cooking',
    category: 'skill',
    duration: 60,
    needEffects: { fun: 15, hunger: 20 },
    skillEffects: { cooking: 1.0 },
  },
  {
    id: 'practice_music',
    name: 'Practice Music',
    category: 'skill',
    duration: 60,
    needEffects: { fun: 25 },
    skillEffects: { music: 1.0, creativity: 0.3 },
  },
  {
    id: 'write',
    name: 'Write',
    category: 'skill',
    duration: 90,
    needEffects: { fun: 20 },
    skillEffects: { writing: 1.0, creativity: 0.5 },
    requirements: { skills: { writing: 1 } },
  },
  {
    id: 'garden',
    name: 'Garden',
    category: 'skill',
    duration: 45,
    needEffects: { fun: 15, hygiene: -10 },
    skillEffects: { gardening: 0.8 },
  },
  {
    id: 'repair_things',
    name: 'Repair Things',
    category: 'skill',
    duration: 60,
    needEffects: { fun: 10 },
    skillEffects: { handiness: 1.0 },
    requirements: { skills: { handiness: 1 } },
  },
  
  // Work
  {
    id: 'work_from_home',
    name: 'Work from Home',
    category: 'work',
    duration: 240,
    needEffects: { energy: -30, fun: -20, social: -10 },
    skillEffects: { logic: 0.3 },
  },
  {
    id: 'attend_meeting',
    name: 'Attend Meeting',
    category: 'work',
    duration: 60,
    needEffects: { energy: -15, social: 20 },
    skillEffects: { charisma: 0.2 },
  },
];

/**
 * Get the current simulation state for an agent
 */
export async function getSimulationState(agentId: number): Promise<SimulationState | null> {
  const db = await getDb();
  if (!db) return null;

  const [agent] = await db.select().from(agents).where(eq(agents.id, agentId)).limit(1);

  if (!agent) return null;
  
  // Map agent emotional state to simulation needs
  const needs = {
    hunger: 100 - (agent.financialNeed || 50), // Using financial as proxy
    energy: 100 - (agent.stress || 30),
    hygiene: agent.satisfaction || 70,
    bladder: 100, // Default high
    social: 100 - (agent.socialNeed || 50),
    fun: agent.happiness || 70,
    comfort: agent.satisfaction || 70,
  };
  
  // Calculate skills from agent attributes
  const skills = {
    cooking: Number(agent.adaptability) * 3 || 3,
    gardening: Number(agent.adaptability) * 2 || 2,
    fishing: 1,
    handiness: Number(agent.expertise) * 3 || 3,
    charisma: Number(agent.negotiationSkill) * 3 || 3,
    creativity: Number(agent.adaptability) * 2 || 2,
    fitness: 3,
    logic: Number(agent.expertise) * 3 || 3,
    writing: Number(agent.adaptability) * 2 || 2,
    music: 1,
  };
  
  // Calculate mood based on average needs
  const avgNeeds = Object.values(needs).reduce((a, b) => a + b, 0) / Object.values(needs).length;
  let mood = 'fine';
  if (avgNeeds >= MOOD_THRESHOLDS.very_happy) mood = 'very_happy';
  else if (avgNeeds >= MOOD_THRESHOLDS.happy) mood = 'happy';
  else if (avgNeeds >= MOOD_THRESHOLDS.fine) mood = 'fine';
  else if (avgNeeds >= MOOD_THRESHOLDS.sad) mood = 'sad';
  else mood = 'very_sad';
  
  // Generate moodlets based on extreme needs
  const moodlets: SimulationState['moodlets'] = [];
  
  if (needs.hunger < 20) {
    moodlets.push({
      id: 'starving',
      name: 'Starving',
      emotion: 'uncomfortable',
      intensity: 3,
      expiresAt: null,
    });
  }
  
  if (needs.energy < 20) {
    moodlets.push({
      id: 'exhausted',
      name: 'Exhausted',
      emotion: 'tired',
      intensity: 3,
      expiresAt: null,
    });
  }
  
  if (needs.social < 20) {
    moodlets.push({
      id: 'lonely',
      name: 'Lonely',
      emotion: 'sad',
      intensity: 2,
      expiresAt: null,
    });
  }
  
  if (needs.fun > 80) {
    moodlets.push({
      id: 'having_fun',
      name: 'Having a Blast',
      emotion: 'happy',
      intensity: 2,
      expiresAt: null,
    });
  }
  
  return {
    agentId,
    needs,
    skills,
    mood,
    moodlets,
    currentAction: null,
    actionProgress: 0,
  };
}

/**
 * Get available actions for an agent based on their current state
 */
export function getAvailableActions(state: SimulationState): SimulationAction[] {
  return SIMULATION_ACTIONS.filter(action => {
    if (!action.requirements) return true;
    
    // Check need requirements
    if (action.requirements.needs) {
      for (const [need, minValue] of Object.entries(action.requirements.needs)) {
        if ((state.needs[need] || 0) < minValue) return false;
      }
    }
    
    // Check skill requirements
    if (action.requirements.skills) {
      for (const [skill, minValue] of Object.entries(action.requirements.skills)) {
        if ((state.skills[skill] || 0) < minValue) return false;
      }
    }
    
    return true;
  });
}

/**
 * Execute an action and update agent state
 */
export async function executeAction(
  agentId: number,
  actionId: string
): Promise<{ success: boolean; newState?: SimulationState; error?: string }> {
  const db = await getDb();
  
  const state = await getSimulationState(agentId);
  if (!state) {
    return { success: false, error: 'Agent not found' };
  }
  
  const action = SIMULATION_ACTIONS.find(a => a.id === actionId);
  if (!action) {
    return { success: false, error: 'Action not found' };
  }
  
  const availableActions = getAvailableActions(state);
  if (!availableActions.find(a => a.id === actionId)) {
    return { success: false, error: 'Action requirements not met' };
  }
  
  // Apply need effects
  const newNeeds = { ...state.needs };
  for (const [need, change] of Object.entries(action.needEffects)) {
    newNeeds[need] = Math.max(0, Math.min(100, (newNeeds[need] || 50) + change));
  }
  
  // Apply skill effects
  const newSkills = { ...state.skills };
  if (action.skillEffects) {
    for (const [skill, gain] of Object.entries(action.skillEffects)) {
      newSkills[skill] = Math.min(10, (newSkills[skill] || 0) + gain);
    }
  }
  
  // Update agent in database
  if (!db) return { success: false, error: 'Database not available' };

  const [agent] = await db.select().from(agents).where(eq(agents.id, agentId)).limit(1);

  if (agent) {
    await db.update(agents)
      .set({
        happiness: Math.round(newNeeds.fun),
        satisfaction: Math.round(newNeeds.comfort),
        stress: Math.round(100 - newNeeds.energy),
        socialNeed: Math.round(100 - newNeeds.social),
        financialNeed: Math.round(100 - newNeeds.hunger),
        updatedAt: new Date(),
      })
      .where(eq(agents.id, agentId));

    // Create memory of action (using 'event' type since 'action' is not in schema)
    await db.insert(agentMemories).values({
      agentId,
      memoryType: 'event',
      content: `Performed action: ${action.name}`,
      importance: 3,
      emotionalImpact: action.needEffects.fun > 0 ? 20 : 0,
      memoryDate: new Date(),
    });
  }
  
  // Return updated state
  const newState = await getSimulationState(agentId);
  return { success: true, newState: newState || undefined };
}

/**
 * Simulate time passing and decay needs
 */
export async function simulateTimePassage(
  agentId: number,
  minutesPassed: number
): Promise<SimulationState | null> {
  const db = await getDb();
  if (!db) return null;

  const state = await getSimulationState(agentId);
  if (!state) return null;

  const hoursPassed = minutesPassed / 60;

  // Apply need decay
  const newNeeds = { ...state.needs };
  for (const [need, rate] of Object.entries(NEED_DECAY_RATES)) {
    const decay = rate * hoursPassed;
    newNeeds[need] = Math.max(0, (newNeeds[need] || 50) - decay);
  }

  // Update agent
  await db.update(agents)
    .set({
      happiness: Math.round(newNeeds.fun),
      satisfaction: Math.round(newNeeds.comfort),
      stress: Math.round(100 - newNeeds.energy),
      socialNeed: Math.round(100 - newNeeds.social),
      financialNeed: Math.round(100 - newNeeds.hunger),
      updatedAt: new Date(),
    })
    .where(eq(agents.id, agentId));

  return getSimulationState(agentId);
}

/**
 * Get recommended actions based on current needs
 */
export function getRecommendedActions(state: SimulationState): SimulationAction[] {
  const available = getAvailableActions(state);
  
  // Find lowest needs
  const needPriorities: { need: string; value: number }[] = [];
  for (const [need, value] of Object.entries(state.needs)) {
    needPriorities.push({ need, value });
  }
  needPriorities.sort((a, b) => a.value - b.value);
  
  // Score actions based on how much they help lowest needs
  const scoredActions = available.map(action => {
    let score = 0;
    for (const { need, value } of needPriorities.slice(0, 3)) {
      const effect = action.needEffects[need] || 0;
      if (effect > 0) {
        // Higher score for actions that help low needs
        score += effect * (100 - value) / 100;
      }
    }
    return { action, score };
  });
  
  // Sort by score and return top recommendations
  scoredActions.sort((a, b) => b.score - a.score);
  return scoredActions.slice(0, 5).map(s => s.action);
}

/**
 * Generate autonomous action for agent
 */
export async function generateAutonomousAction(agentId: number): Promise<SimulationAction | null> {
  const state = await getSimulationState(agentId);
  if (!state) return null;
  
  const recommended = getRecommendedActions(state);
  if (recommended.length === 0) return null;
  
  // Add some randomness - 70% chance to pick best action, 30% random
  if (Math.random() < 0.7) {
    return recommended[0];
  } else {
    const randomIndex = Math.floor(Math.random() * recommended.length);
    return recommended[randomIndex];
  }
}

/**
 * Calculate relationship effects from social actions
 */
export function calculateRelationshipEffect(
  action: SimulationAction,
  currentRelationship: number
): number {
  if (action.category !== 'social') return 0;
  
  const socialGain = action.needEffects.social || 0;
  const relationshipChange = socialGain * 0.3; // 30% of social gain affects relationship
  
  // Diminishing returns at high relationship levels
  const diminishingFactor = 1 - (currentRelationship / 200);
  
  return Math.round(relationshipChange * diminishingFactor);
}

export default {
  getSimulationState,
  getAvailableActions,
  executeAction,
  simulateTimePassage,
  getRecommendedActions,
  generateAutonomousAction,
  calculateRelationshipEffect,
  SIMULATION_ACTIONS,
};
