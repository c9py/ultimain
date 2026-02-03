# NPC AI Integration Guide

This guide explains how to use the NPC AI system with the Exult (Ultima VII) engine.

## Overview

The NPC AI integration consists of three main components:

1. **ExultNPCBridge** - Connects Exult's Actor class to the NPC AI system
2. **DialogueHooks** - Intercepts and enhances NPC conversations with AIML and TinyLLM
3. **ScheduleIntegration** - Allows AI-driven NPC behaviors to influence schedules

## Architecture

```
Exult Engine
    ↓
ExultNPCBridge (npcai/ExultNPCBridge.cpp)
    ↓
NPCSystem (engines/npc/include/NPCSystem.h)
    ↓
[Persona] [TensorLogic] [AIML] [TinyLLM] [Memory] [Social] [Economy]
```

## Building with NPC AI Support

### Enable in CMake

The NPC AI integration is controlled by the `ENABLE_NPC_AI` option (enabled by default):

```bash
cd engines/exult
mkdir build && cd build
cmake .. -DENABLE_NPC_AI=ON
make -j$(nproc)
```

### Library Dependencies

The Exult build will automatically link:
- `libexult_npcai.a` - Exult-specific integration code
- `libultima_npc_ai.a` - Core NPC AI system

## Integration Points

### 1. NPC Profiles

NPC profiles are stored in `engines/exult/npcai/data/` directory:
- `npc_profiles.json` - Personality traits, dialogue patterns
- `aiml_patterns.aiml` - AIML dialogue templates

### 2. Schedule Integration

The AI system can suggest schedule changes based on NPC goals and personality:

```cpp
#include "ScheduleIntegration.h"

// In your Exult actor code:
auto suggestion = ScheduleIntegration::suggestSchedule(actor_id);
if (suggestion.has_value()) {
    // Apply the suggested schedule
    actor->set_schedule(suggestion.value());
}
```

### 3. Dialogue Hooks

Conversations are automatically intercepted and enhanced:

```cpp
#include "DialogueHooks.h"

// The system automatically hooks into:
// - show_npc_message()
// - init_conversation()
// - end_conversation()
```

## Testing the Integration

### 1. Build and Run NPC Demo

```bash
cd engines/npc/build
./examples/npc_demo
```

This demonstrates:
- NPC personality creation
- Social relationship dynamics
- Economic agent behaviors
- Dialogue generation

### 2. Build and Run Dialogue Demo

```bash
cd engines/npc/build
./examples/dialogue_demo
```

This demonstrates:
- AIML pattern matching
- TinyLLM response generation
- Hybrid dialogue system

### 3. Test with Exult (requires game data)

```bash
cd engines/exult/build
./exult
```

When talking to NPCs, the enhanced dialogue system will:
- Match player input against AIML patterns
- Generate contextual responses using personality traits
- Update NPC memory and relationships
- Track emotional states

## Configuration

### NPC Personality Profiles

Edit `engines/exult/npcai/data/npc_profiles.json`:

```json
{
  "npc_001": {
    "name": "Shopkeeper",
    "profession": "merchant",
    "personality": {
      "openness": 0.6,
      "conscientiousness": 0.8,
      "extraversion": 0.7,
      "agreeableness": 0.6,
      "neuroticism": 0.3
    },
    "knowledge_domains": ["trade", "prices", "inventory"],
    "dialogue_style": "friendly_business"
  }
}
```

### AIML Patterns

Edit `engines/exult/npcai/data/aiml_patterns.aiml`:

```xml
<category>
    <pattern>HELLO</pattern>
    <template>Welcome to my shop! How may I help thee?</template>
</category>
```

## Integration Status

### ✅ Completed
- Core NPC AI system (7,400+ lines of C++)
- ExultNPCBridge implementation
- DialogueHooks for conversation interception
- ScheduleIntegration framework
- CMake build integration

### 🔄 In Progress
- Runtime testing with Exult engine
- Game data integration
- Performance optimization

### 📋 Planned
- Full Exult actor integration
- Save/load NPC state
- Multiplayer NPC synchronization

## Troubleshooting

### Build Errors

If you encounter linking errors:
```bash
# Ensure NPC AI library is built first
cd engines/npc && mkdir build && cd build
cmake .. && make -j$(nproc)

# Then build Exult
cd ../../exult && mkdir build && cd build
cmake .. && make -j$(nproc)
```

### Runtime Issues

Enable debug logging:
```bash
export EXULT_DEBUG_NPC=1
./exult
```

## API Reference

### ExultNPCBridge

```cpp
class ExultNPCBridge {
public:
    static ExultNPCBridge& getInstance();
    
    void registerNPC(int actorId, const std::string& name);
    void updateNPCPosition(int actorId, int x, int y);
    std::optional<std::string> generateDialogue(int actorId, const std::string& input);
    void updateRelationship(int npc1, int npc2, float delta);
};
```

### DialogueHooks

```cpp
namespace DialogueHooks {
    void initialize();
    std::string processPlayerInput(int npcId, const std::string& input);
    void onConversationStart(int npcId);
    void onConversationEnd(int npcId);
}
```

### ScheduleIntegration

```cpp
namespace ScheduleIntegration {
    std::optional<int> suggestSchedule(int actorId);
    void onScheduleChange(int actorId, int newSchedule);
    void updateNPCGoals(int actorId, const std::vector<std::string>& goals);
}
```

## Performance Considerations

- **Memory**: Each NPC requires ~50-100KB for AI state
- **CPU**: Dialogue generation: 1-5ms per response
- **Disk**: NPC profiles: ~1-2KB per NPC

For best performance:
- Limit active AI NPCs to those near the player
- Use simplified logic for distant NPCs
- Cache frequently used dialogue patterns

## Further Reading

- [NPC AI Implementation Report](npc_ai_implementation_report.md)
- [Cognitive NPC Architecture](cognitive_npc_architecture.md)
- [Exult NPC Analysis](exult_npc_analysis.md)
