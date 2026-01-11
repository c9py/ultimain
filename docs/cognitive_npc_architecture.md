# Cognitive NPC Architecture for Ultima

## Executive Summary

This document outlines an integration architecture for a **neuro-symbolic cognitive NPC system** that combines multiple AI frameworks to create deeply intelligent, socially dynamic game characters. The system integrates:

- **Tensor Logic** for neuro-symbolic reasoning
- **4th-Order AIML** for meta-cognitive dialogue
- **Tiny Native LLM** for novelty generation
- **Sims-style Social Dynamics** for NPC relationships
- **Dream Vortex Persona Models** for psychological depth
- **Virtual Economics** for emergent market behavior
- **Urban Sprawl Models** for procedural city generation

---

## Resource Analysis

### 1. AGML (PandaMania) - 4th Order Meta-Cognitive AIML

**Source**: `agml-main/`

This is an advanced AIML implementation with **five nested cognitive layers**:

| Layer | Name | Function |
|-------|------|----------|
| 0 | Base Processing | Pattern matching, response generation |
| 1 | First-Order Meta-Cognition | Self-awareness, monitoring |
| 2 | Second-Order Meta-Cognition | Thinking about thinking |
| 3 | Third-Order Meta-Cognition | Reasoning about reasoning |
| 4 | Fourth-Order Meta-Cognition | Meta-meta-cognitive architectural reasoning |

**Key Features**:
- **Autognosis**: Hierarchical self-image building with "grip" metrics
- **Epistemic Reasoning**: Knowledge assessment and uncertainty
- **Counterfactual Reasoning**: Hypothetical scenario evaluation
- **Theory of Mind**: User cognitive state simulation

**Integration Point**: NPC dialogue system with self-aware responses that adapt based on conversation history and relationship state.

---

### 2. GNeural-Net - GNU Neural Network

**Source**: `gneural-net-main/`

A lightweight, scriptable neural network library with multiple training algorithms:

| Algorithm | Use Case |
|-----------|----------|
| Simulated Annealing | Global optimization, avoiding local minima |
| Genetic Algorithm | Evolving NPC behaviors over time |
| Gradient Descent | Fast learning for simple patterns |
| MSMCO | Multi-scale optimization for complex behaviors |

**Integration Point**: Lightweight on-device learning for NPC behavior adaptation without requiring external LLM calls.

---

### 3. Tensor Logic (arXiv Paper)

**Source**: `main.tex`, `2510.12269v3.pdf`

A groundbreaking framework that bridges **Datalog rules** and **tensor operations**:

> "A Datalog rule is an einsum over Boolean tensors, with a step function applied elementwise to the result."

**Key Concepts**:
- Relations as sparse Boolean tensors
- Rules as einsum operations
- Tucker decomposition for latent variables
- Differentiable logic for gradient-based learning

**Example**: The rule `Aunt(x, z) ← Sister(x, y), Parent(y, z)` becomes:
```
A_xz = H(S_xy P_yz) = H(Σ_y S_xy P_yz)
```

**Integration Point**: NPC reasoning engine that combines logical inference with neural learning. NPCs can learn new rules from experience while maintaining logical consistency.

---

### 4. Dream Vortex / DreamCog - Persona Modeling

**Source**: `dream-vortex-main/`, `dreamcog-main/`

A comprehensive agent simulation framework with:

#### Character System
- **Big Five Personality**: Openness, Conscientiousness, Extraversion, Agreeableness, Neuroticism
- **Communication Styles**: Formality, verbosity, emotional expression
- **Behavioral Tendencies**: Impulsiveness, risk-taking, empathy, leadership

#### Emotional Intelligence
- **Emotional States**: Happiness, satisfaction, stress, loyalty, trust
- **Psychological Needs**: Financial security, recognition, autonomy, social connection
- **Memory System**: Experience tracking, trauma, achievements

#### Relationship Dynamics
- **Dynamic Metrics**: Trust, affection, respect, loyalty, dependency, tension
- **Relationship Events**: Significant moments and their impacts
- **Social Networks**: Multi-agent relationship graphs

**Integration Point**: Deep psychological modeling for NPCs that remember past interactions, form opinions, and evolve relationships over time.

---

### 5. Virtunomicog - Virtual Economics

**Source**: `virtunomicog-main/`

Economic simulation engine for:
- Resource production and consumption
- Market dynamics with supply/demand
- Currency flow and inflation
- Business cycles and economic agents

**Integration Point**: Emergent economy where NPC merchants respond to player actions, prices fluctuate based on scarcity, and economic decisions have consequences.

---

### 6. SimsFreePlay Extension - Social Simulation Patterns

**Source**: `simsfreeplay-ext-main/`

Extracted patterns from professional game development:
- Lua layout schemas for UI
- Native library architecture
- Social simulation patterns

**Integration Point**: Reference architecture for implementing Sims-style social meters and relationship systems.

---

### 7. LLM Package - Tiny Native Language Model

**Source**: `llm-1.1.1/`

A lightweight LLM implementation for:
- Novel response generation when AIML patterns fail
- Creative dialogue for unique situations
- Procedural quest generation

**Integration Point**: Fallback system for when deterministic AIML patterns don't match, ensuring NPCs always have something meaningful to say.

---

### 8. UrbanSuite - Urban Sprawl Model

**Source**: `UrbanSuite-TijuanaBordertowns.nlogox`

A NetLogo agent-based model for urban development:
- Employment and service center placement
- Builder agents that create road networks
- Migrant agents with income, savings, and living expenses
- Land value dynamics based on proximity to services

**Integration Point**: Procedural city generation that creates realistic urban layouts with economic logic, complementing the OSM-to-Ultima converter.

---

### 9. THNN / ELU - Neural Network Primitives

**Source**: `THNN(3).h`, `ELU.c`

PyTorch-style neural network primitives:
- Activation functions (ELU, ReLU, etc.)
- Loss functions (BCE, NLL, KL Divergence)
- Gradient computation

**Integration Point**: Low-level building blocks for custom neural network layers in the cognitive system.

---

## Proposed Architecture

### System Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                     ULTIMA COGNITIVE NPC ENGINE                      │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐          │
│  │   PERCEIVE   │───▶│    THINK     │───▶│     ACT      │          │
│  │              │    │              │    │              │          │
│  │ • World State│    │ • Tensor     │    │ • Dialogue   │          │
│  │ • Social     │    │   Logic      │    │ • Movement   │          │
│  │   Context    │    │ • AIML Meta  │    │ • Economy    │          │
│  │ • Memory     │    │ • LLM Novel  │    │ • Combat     │          │
│  └──────────────┘    └──────────────┘    └──────────────┘          │
│         │                   │                   │                   │
│         └───────────────────┼───────────────────┘                   │
│                             │                                       │
│                    ┌────────▼────────┐                              │
│                    │  PERSONA CORE   │                              │
│                    │                 │                              │
│                    │ • Big Five      │                              │
│                    │ • Emotions      │                              │
│                    │ • Relationships │                              │
│                    │ • Memory        │                              │
│                    │ • Goals         │                              │
│                    └─────────────────┘                              │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Layer 1: Perception System

The NPC perceives the world through multiple channels:

```python
class NPCPerception:
    def perceive(self, world_state):
        return {
            'visible_entities': self.get_visible_entities(world_state),
            'audible_events': self.get_audible_events(world_state),
            'social_context': self.get_social_context(world_state),
            'economic_state': self.get_economic_state(world_state),
            'memory_associations': self.recall_relevant_memories()
        }
```

### Layer 2: Neuro-Symbolic Reasoning (Tensor Logic)

The core reasoning engine uses tensor logic for fast, differentiable inference:

```python
class TensorLogicEngine:
    def __init__(self):
        self.relations = {}  # Sparse Boolean tensors
        self.rules = []      # Datalog rules as einsums
        
    def add_rule(self, head, body):
        """Add a Datalog rule: head ← body"""
        # Convert to einsum operation
        self.rules.append(TensorRule(head, body))
        
    def infer(self, query):
        """Infer new facts using tensor operations"""
        for rule in self.rules:
            result = rule.apply(self.relations)
            if result.matches(query):
                return result
        return None
        
    def learn(self, examples, labels):
        """Learn rule weights from examples"""
        # Gradient descent on rule parameters
        pass
```

**Example Rules for NPCs**:

```datalog
% Social reasoning
Friend(x, z) ← Friend(x, y), Friend(y, z), ¬Enemy(x, z).
Suspicious(x, y) ← Thief(x), Witnessed(y, x, steal).
Grateful(x, y) ← Helped(y, x), ¬Enemy(x, y).

% Economic reasoning
WantsToBuy(x, item) ← Needs(x, item), CanAfford(x, item).
WillBargain(x) ← LowFunds(x), HighChaos(world).

% Quest reasoning
QuestAvailable(x, quest) ← HasProblem(x, problem), SolvableBy(problem, quest).
```

### Layer 3: Meta-Cognitive Dialogue (4th Order AIML)

The dialogue system uses AIML with meta-cognitive awareness:

```xml
<!-- Layer 0: Base Pattern -->
<category>
    <pattern>HELLO</pattern>
    <template>
        <think><set name="greeting_count"><get name="greeting_count"/> + 1</set></think>
        <condition name="relationship_level">
            <li value="stranger">Greetings, traveler.</li>
            <li value="acquaintance">Ah, <get name="player_name"/>, good to see you.</li>
            <li value="friend">My friend! What brings you here?</li>
        </condition>
    </template>
</category>

<!-- Layer 4: Meta-Meta-Cognitive -->
<category>
    <pattern>EVALUATE CONVERSATION QUALITY</pattern>
    <template>
        <think>
            <set name="meta_analysis">
                Analyzing my own analysis of this conversation...
                Topic coherence: <get name="topic_coherence"/>
                Emotional alignment: <get name="emotional_alignment"/>
                Goal progress: <get name="goal_progress"/>
            </set>
        </think>
        <srai>OPTIMIZE DIALOGUE STRATEGY</srai>
    </template>
</category>
```

### Layer 4: Novelty Generation (Tiny LLM)

When AIML patterns fail, the tiny LLM generates novel responses:

```python
class NoveltyGenerator:
    def __init__(self, model_path):
        self.llm = TinyLLM.load(model_path)
        
    def generate(self, context, persona, constraints):
        prompt = f"""
        Character: {persona.name}
        Personality: {persona.big_five}
        Current emotion: {persona.emotional_state}
        Relationship with player: {persona.relationship_level}
        
        Context: {context}
        
        Generate a response that is:
        - In character
        - Emotionally appropriate
        - Advances the conversation
        """
        return self.llm.generate(prompt, max_tokens=100)
```

### Layer 5: Persona Core (Dream Vortex)

Each NPC has a deep psychological model:

```python
@dataclass
class NPCPersona:
    # Big Five Personality
    openness: float          # 0-1
    conscientiousness: float
    extraversion: float
    agreeableness: float
    neuroticism: float
    
    # Emotional State
    happiness: float
    stress: float
    trust: Dict[str, float]  # Trust per entity
    
    # Memory
    episodic_memory: List[Memory]
    semantic_memory: Dict[str, Any]
    trauma: List[TraumaticEvent]
    
    # Goals
    short_term_goals: List[Goal]
    long_term_aspirations: List[Aspiration]
    core_values: List[Value]
    
    # Relationships
    relationships: Dict[str, Relationship]
    
    def update_emotion(self, event):
        """Update emotional state based on event"""
        impact = self.calculate_emotional_impact(event)
        self.happiness += impact.happiness_delta
        self.stress += impact.stress_delta
        # ... etc
        
    def form_opinion(self, entity, action):
        """Form or update opinion about an entity"""
        if entity not in self.relationships:
            self.relationships[entity] = Relationship()
        
        rel = self.relationships[entity]
        rel.trust += self.calculate_trust_delta(action)
        rel.affection += self.calculate_affection_delta(action)
```

### Layer 6: Economic System (Virtunomicog)

NPCs participate in a dynamic economy:

```python
class EconomicAgent:
    def __init__(self, npc):
        self.npc = npc
        self.inventory = {}
        self.gold = 100
        self.profession = npc.profession
        
    def decide_price(self, item, buyer):
        base_price = item.base_value
        
        # Adjust for supply/demand
        supply_factor = self.calculate_supply(item)
        demand_factor = self.calculate_demand(item)
        
        # Adjust for relationship
        relationship = self.npc.persona.relationships.get(buyer)
        relationship_discount = relationship.affection * 0.1 if relationship else 0
        
        # Adjust for personality
        greed_factor = 1 - self.npc.persona.agreeableness * 0.2
        
        return base_price * supply_factor * demand_factor * greed_factor - relationship_discount
```

### Layer 7: Urban Dynamics (UrbanSuite)

Cities evolve based on agent-based modeling:

```python
class UrbanSimulation:
    def __init__(self, city_map):
        self.patches = city_map.patches
        self.employment_centers = []
        self.service_centers = []
        self.migrants = []
        
    def tick(self):
        # Update land values based on proximity to services
        for patch in self.patches:
            patch.land_value = self.calculate_land_value(patch)
            
        # Migrants make decisions
        for migrant in self.migrants:
            if migrant.savings < migrant.living_expenses:
                migrant.seek_employment()
            if migrant.wants_to_move():
                migrant.relocate()
                
        # Urban sprawl
        self.expand_city_boundaries()
```

---

## Integration with Ultima Engine

### Data Flow

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│   OSM Data      │────▶│  osm2ultima     │────▶│  Game Map       │
│   (Real World)  │     │  Converter      │     │  (IREG files)   │
└─────────────────┘     └─────────────────┘     └─────────────────┘
                                                        │
                                                        ▼
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Urban Suite    │────▶│  City Generator │────▶│  NPC Placement  │
│  (Sprawl Model) │     │                 │     │  & Schedules    │
└─────────────────┘     └─────────────────┘     └─────────────────┘
                                                        │
                                                        ▼
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Dream Vortex   │────▶│  Persona        │────▶│  NPC Behavior   │
│  (Personas)     │     │  Generator      │     │  & Dialogue     │
└─────────────────┘     └─────────────────┘     └─────────────────┘
                                                        │
                                                        ▼
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Tensor Logic   │────▶│  Reasoning      │────▶│  NPC Decisions  │
│  + AIML + LLM   │     │  Engine         │     │  & Actions      │
└─────────────────┘     └─────────────────┘     └─────────────────┘
```

### File Format Extensions

New file types for the cognitive system:

| Extension | Purpose |
|-----------|---------|
| `.persona` | NPC personality and memory data |
| `.relation` | Relationship graph data |
| `.economy` | Economic state and transactions |
| `.rules` | Tensor logic rules in Datalog format |
| `.aiml` | Dialogue patterns |

---

## Implementation Roadmap

### Phase 1: Foundation (Weeks 1-2)
- [ ] Integrate GNeural-Net for lightweight learning
- [ ] Port AIML engine to C++ for Ultima integration
- [ ] Create persona data structures

### Phase 2: Reasoning (Weeks 3-4)
- [ ] Implement tensor logic engine
- [ ] Define base rule set for NPC behavior
- [ ] Create memory system

### Phase 3: Social Dynamics (Weeks 5-6)
- [ ] Implement relationship system from Dream Vortex
- [ ] Create emotional state machine
- [ ] Build social network graph

### Phase 4: Economy (Weeks 7-8)
- [ ] Implement economic agents
- [ ] Create market simulation
- [ ] Integrate with NPC decision-making

### Phase 5: Urban Dynamics (Weeks 9-10)
- [ ] Port UrbanSuite model to C++
- [ ] Integrate with OSM converter
- [ ] Create procedural city evolution

### Phase 6: Integration & Testing (Weeks 11-12)
- [ ] Full system integration
- [ ] Performance optimization
- [ ] Playtesting and balancing

---

## Conclusion

This architecture creates NPCs that are not just reactive pattern-matchers, but **thinking, feeling, remembering agents** that:

1. **Reason** using tensor logic for fast, differentiable inference
2. **Converse** with 4th-order meta-cognitive awareness
3. **Improvise** using a tiny LLM for novel situations
4. **Feel** with a full emotional state machine
5. **Remember** with episodic and semantic memory
6. **Relate** with dynamic relationship modeling
7. **Trade** in an emergent economy
8. **Live** in procedurally evolving cities

The Avatar's journey through the New York Labyrinth will be populated by thousands of unique individuals, each with their own story, personality, and place in the world.

*"The city breathes. The people dream. The economy churns. And somewhere in the chaos, the Avatar seeks a way home."*
