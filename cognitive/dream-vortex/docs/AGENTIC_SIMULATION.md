# Agentic Simulation System Documentation

## Overview

The Agentic Simulation System implements advanced schemas and models for semi-autonomous entities in the business simulation game. These entities (agents) include customers, suppliers, employees, partners, investors, and competitors, each with unique emotional needs, motivational personas, character traits, and communication styles.

## Core Concepts

### 1. Character Personas

Character personas define the fundamental personality and behavioral patterns of agents. Each persona has:

- **Motivational Levels** (0-100 scale):
  - `ambitionLevel`: Drive for success and growth
  - `cautionLevel`: Risk aversion tendency
  - `socialLevel`: Preference for collaboration
  - `analyticalLevel`: Data-driven decision making

- **Communication Style**: formal, casual, aggressive, passive, diplomatic

- **Decision Speed**: impulsive, quick, moderate, deliberate, very_slow

#### Pre-seeded Personas:

1. **Ambitious Leader**: High ambition, quick decisions, formal communication
2. **Cautious Analyst**: High caution/analytical, deliberate decisions
3. **Social Connector**: High social, casual communication
4. **Aggressive Competitor**: High ambition, impulsive, aggressive style
5. **Diplomatic Mediator**: Balanced, diplomatic communication

### 2. Character Traits

Traits are specific characteristics that modify agent behavior. They are categorized as:

- **Professional**: reliable, innovative, detail-oriented, procrastinator
- **Social**: charismatic, empathetic, introverted, conflict-averse
- **Cognitive**: strategic thinker, analytical, creative, impulsive
- **Emotional**: optimistic, resilient, anxious, hot-tempered

Each trait has:
- An intensity level (0-100) when assigned to an agent
- A positive/negative effect indicator

### 3. Agents

Agents are the core simulation entities with:

#### Types:
- `customer`: End users of products/services
- `supplier`: Providers of resources
- `employee`: Workers at business units
- `partner`: Business partners
- `investor`: Financial stakeholders
- `competitor`: Rival companies

#### Emotional State (0-100):
- `happiness`: Overall satisfaction with current situation
- `satisfaction`: Contentment with recent outcomes
- `stress`: Current pressure and anxiety level
- `loyalty`: Commitment to company/relationship
- `trust`: Confidence in partners/employers

#### Needs (0-100, higher = greater need):
- `financialNeed`: Desire for monetary rewards
- `securityNeed`: Need for stability
- `recognitionNeed`: Desire for acknowledgment
- `autonomyNeed`: Need for independence
- `socialNeed`: Need for social interaction

#### Attributes (decimal scale):
- `reliability`: Consistency in performance
- `negotiationSkill`: Ability to bargain
- `adaptability`: Flexibility to change
- `expertise`: Domain knowledge level

### 4. Relationships

Relationships track connections between agents with:

#### Types:
- `business`: Professional business relationships
- `personal`: Personal friendships
- `professional`: Work-related connections
- `familial`: Family ties
- `competitive`: Rivalry relationships

#### Metrics (0-100):
- `strength`: Overall relationship intensity
- `positivity`: Positive vs negative sentiment
- `frequency`: Interaction rate
- `interactionCount`: Total interactions

#### Status:
- `active`: Ongoing relationship
- `dormant`: Inactive but exists
- `strained`: Under tension
- `broken`: Severed relationship

### 5. Groups

Groups organize agents with shared interests:

#### Types:
- `department`: Organizational divisions
- `team`: Project teams
- `union`: Labor unions
- `association`: Professional associations
- `club`: Social clubs
- `network`: Professional networks

#### Dynamics (0-100):
- `cohesion`: Unity of the group
- `influence`: Impact on members/external
- `morale`: Overall group satisfaction

#### Membership Roles:
- `leader`: Group leader
- `core_member`: Core contributor
- `member`: Regular member
- `associate`: Affiliated member

### 6. Communities

Communities are larger social structures within cities:

#### Types:
- `residential`: Living areas
- `business`: Commercial districts
- `industrial`: Manufacturing zones
- `cultural`: Arts and culture areas
- `virtual`: Online communities

#### Characteristics (0-100):
- `prosperity`: Economic wellbeing
- `harmony`: Social cohesion
- `growth`: Development rate

### 7. Events

Events are time-sensitive occurrences that affect agents:

#### Types:
- `interaction`: Social interactions
- `transaction`: Business deals
- `milestone`: Achievement moments
- `crisis`: Emergency situations
- `celebration`: Positive events
- `conflict`: Disagreements
- `negotiation`: Bargaining sessions
- `collaboration`: Joint efforts

#### Impacts:

**Emotional Impact**: Changes to agent emotions
```typescript
{
  happiness?: number,
  satisfaction?: number,
  stress?: number,
  loyalty?: number,
  trust?: number
}
```

**Relationship Impact**: Changes to relationships
```typescript
{
  agentIds: number[],
  strengthChange: number,
  positivityChange: number
}
```

#### Event Lifecycle:
1. `scheduled`: Event is planned
2. `in_progress`: Event is happening
3. `completed`: Event finished successfully
4. `cancelled`: Event was cancelled
5. `failed`: Event failed to complete

### 8. Event Triggers

Automatic event generators based on conditions:

```typescript
conditions: [
  {
    type: "threshold" | "time" | "relationship" | "emotion",
    metric: string,
    operator: ">" | "<" | "==" | "!=" | "between",
    value: number | string | number[]
  }
]
```

### 9. Agent History

Historical tracking of agent state changes over time, recording:
- Emotional state snapshots
- Related events
- Related relationships
- Contextual notes

## API Usage

### Character Personas

```typescript
// List all personas
const personas = await trpc.persona.list.query();

// Get specific persona
const persona = await trpc.persona.byId.query({ id: 1 });
```

### Character Traits

```typescript
// List all traits
const traits = await trpc.trait.list.query();

// Filter by category
const professionalTraits = await trpc.trait.byCategory.query({ 
  category: "professional" 
});
```

### Agents

```typescript
// Create an agent
const customer = await trpc.agent.create.mutate({
  type: "customer",
  name: "John Doe",
  personaId: 1,
  cityId: 1
});

// Get agents by type
const customers = await trpc.agent.byType.query({ type: "customer" });

// Update emotional state
await trpc.agent.updateEmotions.mutate({
  id: 1,
  emotions: {
    happiness: 80,
    satisfaction: 75,
    stress: 25
  }
});

// Add trait to agent
await trpc.agent.addTrait.mutate({
  agentId: 1,
  traitId: 1,
  intensity: 80
});

// Get agent history
const history = await trpc.agent.history.query({ 
  agentId: 1, 
  limit: 50 
});
```

### Relationships

```typescript
// Create relationship
const relationship = await trpc.relationship.create.mutate({
  agent1Id: 1,
  agent2Id: 2,
  type: "business"
});

// Get agent relationships
const relationships = await trpc.relationship.byAgent.query({ 
  agentId: 1 
});

// Record interaction
await trpc.relationship.recordInteraction.mutate({
  agent1Id: 1,
  agent2Id: 2,
  strengthChange: 5,
  positivityChange: 3
});
```

### Groups

```typescript
// Create group
const team = await trpc.group.create.mutate({
  name: "Sales Team",
  type: "team",
  description: "Company sales team"
});

// Add member
await trpc.group.addMember.mutate({
  groupId: 1,
  agentId: 1,
  role: "leader"
});

// Get members
const members = await trpc.group.members.query({ groupId: 1 });
```

### Communities

```typescript
// Create community
const community = await trpc.community.create.mutate({
  name: "Tech District",
  cityId: 1,
  type: "business",
  description: "Technology business district"
});

// Get communities in city
const communities = await trpc.community.byCity.query({ cityId: 1 });

// Add agent to community
await trpc.community.addMember.mutate({
  communityId: 1,
  agentId: 1
});
```

### Events

```typescript
// Create event
const event = await trpc.event.create.mutate({
  type: "interaction",
  initiatorAgentId: 1,
  targetAgentId: 2,
  title: "Business Meeting",
  description: "Quarterly review",
  scheduledAt: new Date(),
  duration: 60,
  emotionalImpact: {
    satisfaction: 5,
    stress: -3
  },
  relationshipImpact: {
    agentIds: [1, 2],
    strengthChange: 5,
    positivityChange: 3
  }
});

// Get agent events
const events = await trpc.event.byAgent.query({ agentId: 1 });

// Get scheduled events
const scheduled = await trpc.event.scheduled.query();

// Process event (apply impacts)
await trpc.event.process.mutate({ eventId: 1 });
```

## Database Schema

All schemas are defined in `/drizzle/schema.ts` using Drizzle ORM. Key tables:

- `character_personas`: Personality templates
- `character_traits`: Behavioral characteristics
- `agents`: Core simulation entities
- `agent_traits`: Agent-trait associations
- `relationships`: Agent connections
- `agent_groups`: Group definitions
- `group_memberships`: Agent-group associations
- `communities`: City-level social structures
- `community_memberships`: Agent-community associations
- `agent_events`: Time-sensitive occurrences
- `event_triggers`: Automatic event generation rules
- `agent_histories`: Historical state tracking

## Implementation Notes

### Time-Sensitive Behavior

Events are scheduled with `scheduledAt` timestamps. A game turn processor should:
1. Query scheduled events due for processing
2. Call `processAgentEvent()` for each event
3. Apply emotional and relationship impacts
4. Record historical snapshots

### Semi-Autonomy

Agents make decisions based on:
- Their persona's motivational levels
- Current emotional state
- Current needs levels
- Relationships with other agents
- Group and community influences

### Event Impact Processing

When an event is processed:
1. Emotional impacts are applied to participating agents
2. Relationship changes are recorded
3. Historical snapshots are created
4. Event status is updated to "completed"

### Relationship Management

Relationships are bidirectional but stored once:
- `agent1Id` always contains the lower ID
- `agent2Id` always contains the higher ID
- This ensures consistency and prevents duplicates

## Testing

Comprehensive test suite in `/server/agentic-simulation.test.ts` covers:
- Persona and trait management
- Agent creation and updates
- Emotional state changes
- Relationship dynamics
- Group membership
- Community participation
- Event creation and processing

Run tests with:
```bash
npm run test
```

## Future Enhancements

Potential additions to the system:
- Machine learning for agent behavior prediction
- Natural language generation for agent interactions
- Complex event chains and cascading effects
- Agent skill development over time
- Dynamic persona evolution
- Market sentiment influenced by agent emotions
- Agent-driven economic cycles
- Social network analysis tools
- Agent negotiation AI
- Cultural factors influencing behavior
