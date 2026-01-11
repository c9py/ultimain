# DreamCog Integration - Advanced AI Storytelling Features

## Overview

This document describes the integration of advanced AI storytelling features from the [DreamCog platform](https://github.com/o9nn/dreamcog) into the Vorticog simulation engine. These enhancements enable richer character development, world building, and narrative generation capabilities.

## New Features

### 1. Big Five Personality Model

Extends agent personalities with the scientifically-validated Big Five personality model:

#### Personality Traits (0-100 scale)
- **Openness**: Creativity, curiosity, openness to experience
- **Conscientiousness**: Organization, dependability, self-discipline
- **Extraversion**: Sociability, assertiveness, energy level
- **Agreeableness**: Compassion, cooperation, trust
- **Neuroticism**: Emotional stability and resilience

#### Communication Style Attributes (0-100 scale)
- **Formality Level**: 0=casual, 100=formal
- **Verbosity Level**: 0=concise, 100=verbose
- **Emotional Expression**: 0=reserved, 100=expressive
- **Humor Level**: Use of humor in communication
- **Directness Level**: How direct vs indirect in communication

#### Behavioral Tendencies (0-100 scale)
- **Impulsiveness**: Tendency to act without thinking
- **Risk Taking**: Willingness to take chances
- **Empathy**: Ability to understand others' emotions
- **Leadership**: Natural leadership qualities
- **Independence**: Preference for autonomy

**Database Table**: `agent_big_five_personality`

**API Endpoints**:
- `personality.get` - Get personality profile for an agent
- `personality.create` - Create personality profile
- `personality.update` - Update personality attributes

### 2. Agent Motivations System

Track short-term goals, long-term aspirations, and core values for each agent.

#### Motivation Types
- **Short-term**: Immediate goals and tasks
- **Long-term**: Aspirations and life goals
- **Core values**: Fundamental beliefs and principles

#### Attributes
- **Priority**: 1-10 importance ranking
- **Progress**: 0-100% completion
- **Active Status**: Whether currently pursuing

**Database Table**: `agent_motivations`

**API Endpoints**:
- `motivation.create` - Create new motivation
- `motivation.byAgent` - Get agent motivations (with optional activeOnly filter)
- `motivation.update` - Update motivation progress and status

### 3. Agent Memories System

Store and track significant experiences, knowledge, and emotional moments for agents.

#### Memory Types
- **Event**: Significant occurrences
- **Interaction**: Social encounters
- **Knowledge**: Learned information
- **Emotion**: Emotional experiences
- **Skill**: Acquired abilities
- **Trauma**: Negative impactful events
- **Achievement**: Accomplishments

#### Attributes
- **Emotional Impact**: -100 to +100 (how it affected emotions)
- **Importance**: 1-10 significance level
- **Repression Status**: Can be hidden/suppressed
- **Last Recalled**: Tracking memory access

**Database Table**: `agent_memories`

**API Endpoints**:
- `memory.create` - Create new memory
- `memory.byAgent` - Get agent memories (ordered by importance and recency)

### 4. Relationship Events System

Track significant moments in relationships between agents.

#### Event Types
- **First Meeting**: Initial encounter
- **Conflict**: Disagreement or fight
- **Bonding**: Positive connection moment
- **Betrayal**: Trust violation
- **Reconciliation**: Making amends
- **Milestone**: Significant achievement together
- **Other**: Custom events

#### Impact Tracking
Each event records its impact (-100 to +100) on:
- **Trust**: Belief in reliability
- **Affection**: Emotional warmth
- **Respect**: Admiration and regard

**Database Table**: `relationship_events`

**API Endpoints**:
- `relationshipEvent.create` - Record relationship event
- `relationshipEvent.byRelationship` - Get relationship history

### 5. World Building System

Create rich narrative contexts for simulation scenarios.

#### World Attributes
- **Genre**: Fantasy, sci-fi, historical, etc.
- **Time Period**: Era or epoch
- **Technology Level**: Technological advancement stage
- **Magic System**: How magic works (if applicable)
- **Cultural Notes**: Social and cultural context

#### World Rules
Configurable rule systems:
- **Physics Rules**: How the world's physics work
- **Social Rules**: Cultural norms and expectations
- **Magic Rules**: Magical system constraints

**Database Table**: `worlds`

**API Endpoints**:
- `world.create` - Create new world
- `world.byId` - Get world details
- `world.list` - Get user's worlds
- `world.update` - Update world properties

### 6. Locations System

Hierarchical place system with parent-child relationships.

#### Location Types
- **City**: Urban settlements
- **Building**: Structures and facilities
- **Wilderness**: Natural areas
- **Dungeon**: Underground/confined spaces
- **Realm**: Dimensional regions
- **Dimension**: Alternate realities
- **Other**: Custom types

#### Location Attributes
- **Climate**: Weather and environmental conditions
- **Population**: Size and density
- **Danger Level**: 0-100 risk factor
- **Resources**: Available materials and assets
- **Notable Features**: Distinctive characteristics

**Database Table**: `locations`

**API Endpoints**:
- `location.create` - Create new location
- `location.byId` - Get location details
- `location.byWorld` - Get all locations in a world
- `location.subLocations` - Get child locations

### 7. Lore Entries System

Knowledge database for world building and narrative depth.

#### Lore Categories
- **History**: Historical events and records
- **Legend**: Myths and legendary tales
- **Culture**: Cultural practices and norms
- **Religion**: Beliefs and spiritual systems
- **Politics**: Governance and power structures
- **Science**: Scientific knowledge
- **Magic**: Magical lore and systems
- **Species**: Races and creatures
- **Language**: Linguistic information
- **Artifact**: Significant objects
- **Other**: Custom categories

#### Attributes
- **Public/Secret Status**: Knowledge accessibility
- **Related Entities**: Links to locations, agents
- **Tags**: For categorization and search

**Database Table**: `lore_entries`

**API Endpoints**:
- `lore.create` - Create lore entry
- `lore.byId` - Get lore details
- `lore.byWorld` - Get world lore (with optional category filter)

### 8. World Events System

Historical timeline tracking for worlds.

#### Event Types
- **Battle**: Military conflicts
- **Discovery**: New findings or revelations
- **Political**: Government and power changes
- **Natural**: Natural disasters or phenomena
- **Magical**: Magical occurrences
- **Social**: Social movements or changes
- **Economic**: Economic events and shifts
- **Other**: Custom events

#### Attributes
- **Importance**: 1-10 historical significance
- **Event Date**: Flexible format for fictional calendars
- **Duration**: How long it lasted
- **Involved Entities**: Characters, groups, locations
- **Consequences**: Long-term effects

**Database Table**: `world_events`

**API Endpoints**:
- `worldEvent.create` - Create world event
- `worldEvent.byId` - Get event details
- `worldEvent.byWorld` - Get world timeline

### 9. Scheduled World Events System

Future event planning with condition-based triggers.

#### Trigger Types
- **Time-based**: Occur at specific time
- **Condition-based**: Triggered by conditions
- **Manual**: User-initiated

#### Attributes
- **Priority**: 1-10 execution order
- **Recurring**: Can repeat automatically
- **Status**: pending, active, completed, cancelled
- **Target Entities**: Affected agents and locations

**Database Table**: `scheduled_world_events`

**API Endpoints**:
- `scheduledWorldEvent.create` - Schedule future event
- `scheduledWorldEvent.byId` - Get scheduled event
- `scheduledWorldEvent.pending` - Get pending events for world
- `scheduledWorldEvent.update` - Update event status

## Integration Benefits

### For Business Simulation
1. **Richer NPCs**: Customers, suppliers, and employees with deeper personalities
2. **Narrative Context**: Business events within larger world narrative
3. **Market Dynamics**: Economic events tied to world history
4. **Character Development**: Employees grow and change over time
5. **Relationship Depth**: Business relationships have history and meaning

### For AI Storytelling
1. **Autonomous Agents**: Characters with goals, memories, and personalities
2. **Event-Driven Narratives**: Stories shaped by scheduled and triggered events
3. **World Consistency**: Lore database ensures narrative coherence
4. **Emotional Depth**: Rich emotional modeling for realistic responses
5. **Social Networks**: Complex relationship graphs drive story dynamics

## Usage Examples

### Creating a Character with Full Personality

```typescript
// Create agent
const agent = await trpc.agent.create.mutate({
  type: "employee",
  name: "Elena Rothschild",
  personaId: 1,
  cityId: 1,
  companyId: myCompanyId,
});

// Add Big Five personality
await trpc.personality.create.mutate({
  agentId: agent.id,
  openness: 85,              // Very creative
  conscientiousness: 70,      // Organized
  extraversion: 45,           // Somewhat introverted
  agreeableness: 80,          // Very cooperative
  neuroticism: 35,            // Emotionally stable
  formalityLevel: 60,         // Moderately formal
  verbosityLevel: 40,         // Relatively concise
  emotionalExpression: 50,    // Balanced
  leadership: 75,             // Strong leadership
  empathy: 85,                // Highly empathetic
});

// Add motivations
await trpc.motivation.create.mutate({
  agentId: agent.id,
  motivationType: "long_term",
  description: "Become the company's Chief Technology Officer",
  priority: 9,
});

await trpc.motivation.create.mutate({
  agentId: agent.id,
  motivationType: "core_value",
  description: "Innovation and continuous learning",
  priority: 10,
});
```

### Building a Game World

```typescript
// Create world
const world = await trpc.world.create.mutate({
  name: "Technotopia",
  description: "A world where technology and magic coexist",
  genre: "cyberpunk-fantasy",
  timePeriod: "2150",
  technologyLevel: "advanced",
  magicSystem: "Digital magic channeled through neural implants",
  rules: {
    physicsRules: ["Gravity manipulation possible with tech"],
    socialRules: ["Megacorps control city-states"],
    magicRules: ["Magic requires cybernetic enhancement"],
  },
});

// Create capital city
const city = await trpc.location.create.mutate({
  worldId: world.id,
  name: "Neo-Singapore",
  locationType: "city",
  description: "Sprawling megacity with towering arcologies",
  attributes: {
    climate: "tropical, climate-controlled",
    population: "50 million",
    dangerLevel: 40,
    resources: ["rare earth metals", "quantum processors", "bio-fuel"],
    notableFeatures: [
      "The Sky Garden - floating agricultural district",
      "Deep Core - underground hacker haven",
    ],
  },
});

// Add lore
await trpc.lore.create.mutate({
  worldId: world.id,
  category: "history",
  title: "The Digital Awakening",
  content: "In 2085, the first successful human-AI neural merge...",
  isPublic: true,
  relatedLocationId: city.id,
  tags: ["history", "technology", "magic"],
});

// Create historical event
await trpc.worldEvent.create.mutate({
  worldId: world.id,
  title: "The Corporate Wars",
  description: "Three megacorps battled for control of Neo-Singapore",
  eventType: "battle",
  importance: 9,
  eventDate: "2145-03-15",
  duration: "6 months",
  locationId: city.id,
  consequences: "Established the current tripartite power structure",
});
```

### Recording Character Growth

```typescript
// Record a memory after major event
await trpc.memory.create.mutate({
  agentId: elena.id,
  memoryType: "achievement",
  content: "Successfully led the team through the Q3 product launch",
  emotionalImpact: 75,  // Very positive
  importance: 8,
  memoryDate: new Date(),
  eventId: launchEventId,
});

// Update motivation progress
await trpc.motivation.update.mutate({
  id: ctoMotivationId,
  updates: {
    progress: 45,  // 45% toward becoming CTO
  },
});

// Record relationship event with manager
await trpc.relationshipEvent.create.mutate({
  relationshipId: elenaManagerRelId,
  eventType: "milestone",
  description: "Received promotion to Senior Engineer",
  impactOnTrust: 15,
  impactOnAffection: 10,
  impactOnRespect: 20,
  eventDate: new Date(),
});
```

### Scheduling Future Events

```typescript
// Schedule quarterly review
await trpc.scheduledWorldEvent.create.mutate({
  worldId: world.id,
  eventName: "Quarterly Business Review",
  description: "Company-wide performance evaluation",
  scheduledFor: new Date("2025-04-01"),
  eventTrigger: {
    triggerType: "time",
  },
  targetAgentIds: [elena.id, managerId, ceoId],
  priority: 7,
  isRecurring: true,
});

// Schedule condition-based event
await trpc.scheduledWorldEvent.create.mutate({
  worldId: world.id,
  eventName: "Market Opportunity",
  description: "Emerging market opportunity detected",
  scheduledFor: new Date("2025-02-15"),
  eventTrigger: {
    triggerType: "condition",
    conditions: [
      {
        type: "threshold",
        metric: "market_demand",
        operator: ">",
        value: 75,
      },
    ],
  },
  targetLocationId: city.id,
  priority: 8,
});
```

## Database Schema

All new tables follow Drizzle ORM conventions with proper type safety:

### Schema Files
- **Schema Definitions**: `/drizzle/schema.ts`
- **Database Functions**: `/server/db.ts`
- **API Routes**: `/server/routers.ts`

### Key Tables
1. `agent_big_five_personality` - Personality profiles
2. `agent_motivations` - Goals and aspirations
3. `agent_memories` - Experience tracking
4. `relationship_events` - Relationship history
5. `worlds` - World definitions
6. `locations` - Place hierarchy
7. `lore_entries` - Knowledge database
8. `world_events` - Historical timeline
9. `scheduled_world_events` - Future planning

### Relations
All tables have proper Drizzle ORM relations defined for:
- Type-safe joins
- Efficient queries
- Referential integrity

## Type Safety

All schemas include full TypeScript type definitions:

```typescript
import type {
  AgentBigFivePersonality,
  InsertAgentBigFivePersonality,
  AgentMotivation,
  InsertAgentMotivation,
  AgentMemory,
  InsertAgentMemory,
  RelationshipEvent,
  InsertRelationshipEvent,
  World,
  InsertWorld,
  Location,
  InsertLocation,
  LoreEntry,
  InsertLoreEntry,
  WorldEvent,
  InsertWorldEvent,
  ScheduledWorldEvent,
  InsertScheduledWorldEvent,
} from '../drizzle/schema';
```

## Migration

To apply these schema changes:

```bash
npm run db:push
```

This will:
1. Generate migration files in `/drizzle/meta`
2. Apply changes to your database
3. Maintain backward compatibility with existing data

## Performance Considerations

- **Indexed Foreign Keys**: All foreign keys are indexed for fast joins
- **Efficient Queries**: Database functions use proper filtering and ordering
- **Pagination Support**: Memory and history queries support limits
- **Lazy Loading**: Related entities loaded on demand

## Testing

A comprehensive test suite should be added to validate:
- Schema integrity
- API endpoint functionality
- Type safety
- Data validation
- Authorization checks

## Future Enhancements

Potential additions for deeper integration:

1. **AI-Driven Narratives**: Use agent personalities to generate stories
2. **Dynamic Event Generation**: Automatically create events based on agent state
3. **Behavior Trees**: Structured decision-making for autonomous agents
4. **Influence Maps**: Track agent influence across locations
5. **Skill Trees**: Detailed competency tracking
6. **Dialogue System**: Conversation tracking and generation
7. **Reputation System**: Public opinion tracking
8. **Cultural Systems**: Shared beliefs and norms for groups

## Integration with Existing Systems

The DreamCog features integrate seamlessly with existing Vorticog systems:

### Agents
- Enhanced with personality profiles
- Motivations drive behavior
- Memories inform decisions
- Relationships have depth

### Business Units
- Can be locations in worlds
- Events happen at units
- Employees are fully-realized agents

### Cities
- Can be locations in worlds
- Have lore and history
- Host world events

### Companies
- Company lore in knowledge base
- Company history as world events
- Strategic goals as motivations

### Transactions
- Create memories for participants
- Impact relationships
- Part of world timeline

## Conclusion

The DreamCog integration transforms Vorticog from a business simulation into a rich narrative-driven experience. Agents become fully-realized characters with depth, worlds provide context for all actions, and the combination enables emergent storytelling alongside strategic gameplay.

For questions or contributions, see the main [README.md](../README.md) and [AGENTIC_SIMULATION.md](./AGENTIC_SIMULATION.md).
