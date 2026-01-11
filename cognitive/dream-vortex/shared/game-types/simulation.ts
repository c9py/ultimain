/**
 * Simulation Game Types
 * Inspired by Sims FreePlay architecture patterns
 * Integrated into Dream Vortex for enhanced life simulation features
 */

// Layout and UI Schema Types (from layout_schema.lua patterns)
export interface LayoutElement {
  id: string;
  type: 'panel' | 'button' | 'text' | 'image' | 'container' | 'list' | 'grid';
  position: { x: number; y: number };
  size: { width: number; height: number };
  anchor?: { x: number; y: number };
  children?: LayoutElement[];
  properties?: Record<string, unknown>;
}

export interface UIScreen {
  id: string;
  name: string;
  layout: LayoutElement[];
  transitions?: ScreenTransition[];
}

export interface ScreenTransition {
  from: string;
  to: string;
  trigger: string;
  animation?: string;
  duration?: number;
}

// Sim Character Types
export interface SimCharacter {
  id: number;
  name: string;
  age: 'baby' | 'toddler' | 'child' | 'teen' | 'adult' | 'senior';
  gender: 'male' | 'female' | 'nonbinary';
  
  // Appearance
  appearance: {
    skinTone: number;
    hairStyle: string;
    hairColor: string;
    eyeColor: string;
    outfit: string;
    accessories: string[];
  };
  
  // Needs (0-100 scale)
  needs: {
    hunger: number;
    energy: number;
    hygiene: number;
    bladder: number;
    social: number;
    fun: number;
    comfort: number;
  };
  
  // Skills (0-10 scale)
  skills: {
    cooking: number;
    gardening: number;
    fishing: number;
    handiness: number;
    charisma: number;
    creativity: number;
    fitness: number;
    logic: number;
    writing: number;
    music: number;
  };
  
  // Relationships
  relationships: SimRelationship[];
  
  // Career
  career?: SimCareer;
  
  // Traits
  traits: SimTrait[];
  
  // Aspirations
  aspirations: SimAspiration[];
  
  // Current state
  currentAction?: SimAction;
  currentLocation?: string;
  mood: SimMood;
  
  // Lifetime
  lifetimePoints: number;
  achievements: string[];
}

export interface SimRelationship {
  targetSimId: number;
  type: 'family' | 'friend' | 'romantic' | 'acquaintance' | 'enemy';
  familyRelation?: 'parent' | 'child' | 'sibling' | 'spouse' | 'grandparent' | 'grandchild' | 'cousin';
  friendshipLevel: number; // -100 to 100
  romanticLevel: number; // 0 to 100
  lastInteraction: Date;
}

export interface SimCareer {
  id: string;
  name: string;
  level: number;
  maxLevel: number;
  salary: number;
  hoursPerDay: number;
  daysPerWeek: number[];
  performance: number;
  coworkers: number[];
  boss?: number;
}

export interface SimTrait {
  id: string;
  name: string;
  category: 'emotional' | 'social' | 'lifestyle' | 'hobby';
  effects: TraitEffect[];
}

export interface TraitEffect {
  type: 'need_decay' | 'skill_gain' | 'relationship' | 'mood';
  target: string;
  modifier: number;
}

export interface SimAspiration {
  id: string;
  name: string;
  category: 'family' | 'fortune' | 'knowledge' | 'love' | 'popularity' | 'creativity';
  milestones: AspirationMilestone[];
  currentMilestone: number;
  reward?: string;
}

export interface AspirationMilestone {
  id: string;
  description: string;
  requirements: MilestoneRequirement[];
  completed: boolean;
  points: number;
}

export interface MilestoneRequirement {
  type: 'skill' | 'relationship' | 'career' | 'item' | 'action' | 'money';
  target: string;
  value: number;
  current?: number;
}

export interface SimAction {
  id: string;
  name: string;
  type: 'autonomous' | 'directed' | 'social' | 'object';
  duration: number; // in game minutes
  startTime: Date;
  targetObject?: string;
  targetSim?: number;
  effects: ActionEffect[];
  interruptible: boolean;
}

export interface ActionEffect {
  type: 'need' | 'skill' | 'relationship' | 'mood' | 'money';
  target: string;
  change: number;
  perMinute?: boolean;
}

export interface SimMood {
  overall: 'very_happy' | 'happy' | 'fine' | 'sad' | 'very_sad' | 'angry' | 'embarrassed' | 'tense' | 'focused' | 'inspired' | 'playful' | 'flirty' | 'confident' | 'bored' | 'uncomfortable';
  moodlets: Moodlet[];
}

export interface Moodlet {
  id: string;
  name: string;
  emotion: string;
  intensity: number; // 1-4
  duration: number; // in game minutes, -1 for permanent
  source: string;
  startTime: Date;
}

// Lot and Building Types
export interface SimLot {
  id: number;
  name: string;
  type: 'residential' | 'community' | 'commercial' | 'park';
  size: { width: number; height: number };
  address: string;
  neighborhood: string;
  value: number;
  residents: number[];
  rooms: SimRoom[];
  objects: SimObject[];
  outdoor: OutdoorArea;
}

export interface SimRoom {
  id: string;
  name: string;
  type: 'bedroom' | 'bathroom' | 'kitchen' | 'living' | 'dining' | 'garage' | 'office' | 'nursery' | 'basement' | 'attic';
  floor: number;
  bounds: { x: number; y: number; width: number; height: number };
  wallColor?: string;
  floorType?: string;
  lighting: number;
  cleanliness: number;
}

export interface SimObject {
  id: string;
  catalogId: string;
  name: string;
  category: 'furniture' | 'appliance' | 'decoration' | 'electronics' | 'plumbing' | 'lighting' | 'outdoor';
  position: { x: number; y: number; z: number };
  rotation: number;
  roomId?: string;
  condition: number; // 0-100
  value: number;
  interactions: ObjectInteraction[];
  upgrades?: string[];
}

export interface ObjectInteraction {
  id: string;
  name: string;
  requirements?: InteractionRequirement[];
  effects: ActionEffect[];
  duration: number;
  autonomous: boolean;
  socialAllowed: boolean;
}

export interface InteractionRequirement {
  type: 'skill' | 'trait' | 'need' | 'age' | 'relationship';
  target: string;
  minValue?: number;
  maxValue?: number;
}

export interface OutdoorArea {
  terrain: TerrainTile[][];
  plants: Plant[];
  pools: Pool[];
  paths: Path[];
}

export interface TerrainTile {
  type: 'grass' | 'dirt' | 'sand' | 'stone' | 'water';
  elevation: number;
}

export interface Plant {
  id: string;
  type: 'tree' | 'bush' | 'flower' | 'vegetable' | 'fruit';
  species: string;
  position: { x: number; y: number };
  growthStage: number;
  health: number;
  harvestable: boolean;
}

export interface Pool {
  id: string;
  bounds: { x: number; y: number; width: number; height: number };
  depth: number;
  features: string[];
}

export interface Path {
  id: string;
  type: 'stone' | 'brick' | 'concrete' | 'gravel';
  points: { x: number; y: number }[];
}

// Economy Types
export interface SimEconomy {
  householdFunds: number;
  bills: Bill[];
  income: IncomeSource[];
  expenses: Expense[];
  investments: Investment[];
}

export interface Bill {
  id: string;
  type: 'utilities' | 'taxes' | 'services' | 'mortgage';
  amount: number;
  dueDate: Date;
  paid: boolean;
}

export interface IncomeSource {
  type: 'salary' | 'business' | 'royalties' | 'investments' | 'other';
  amount: number;
  frequency: 'daily' | 'weekly' | 'monthly';
  source: string;
}

export interface Expense {
  type: 'food' | 'bills' | 'entertainment' | 'shopping' | 'services';
  amount: number;
  date: Date;
  description: string;
}

export interface Investment {
  id: string;
  type: 'stocks' | 'property' | 'business';
  value: number;
  purchasePrice: number;
  purchaseDate: Date;
  returns: number;
}

// Event System Types
export interface SimEvent {
  id: string;
  type: 'birthday' | 'wedding' | 'party' | 'holiday' | 'career' | 'random';
  name: string;
  description: string;
  startTime: Date;
  duration: number;
  location?: number;
  participants: number[];
  requirements?: EventRequirement[];
  rewards?: EventReward[];
  triggered: boolean;
}

export interface EventRequirement {
  type: 'item' | 'sim' | 'money' | 'time' | 'relationship';
  target: string;
  value: number;
}

export interface EventReward {
  type: 'money' | 'item' | 'relationship' | 'skill' | 'aspiration';
  target: string;
  value: number;
}

// Time Management
export interface SimTime {
  day: number;
  hour: number;
  minute: number;
  dayOfWeek: 'monday' | 'tuesday' | 'wednesday' | 'thursday' | 'friday' | 'saturday' | 'sunday';
  season: 'spring' | 'summer' | 'fall' | 'winter';
  weather: 'sunny' | 'cloudy' | 'rainy' | 'stormy' | 'snowy' | 'foggy';
  speed: 0 | 1 | 2 | 3; // pause, normal, fast, ultra
}

// Save/Load Types
export interface SimSaveData {
  version: string;
  timestamp: Date;
  household: {
    name: string;
    funds: number;
    sims: SimCharacter[];
    lot: SimLot;
  };
  world: {
    time: SimTime;
    events: SimEvent[];
    relationships: SimRelationship[];
  };
  progress: {
    achievements: string[];
    aspirationsCompleted: string[];
    totalPlayTime: number;
  };
}

// Integration with Dream Vortex Agent System
export interface SimAgentBridge {
  simId: number;
  agentId: number;
  syncMode: 'sim_to_agent' | 'agent_to_sim' | 'bidirectional';
  mappings: {
    needs: Record<string, string>;
    skills: Record<string, string>;
    emotions: Record<string, string>;
  };
}
