# Exult NPC Architecture Analysis

## Key Integration Points

### 1. Actor Class (actors.h)
The main NPC class with:
- **Schedule system** - Controls NPC behavior (sleep, work, patrol, etc.)
- **Attack modes** - Combat AI (nearest, weakest, strongest, berserk, etc.)
- **Frame sequences** - Animation control
- **Spots system** - Equipment/inventory slots (18 spots)
- **Timers** - Poison, hunger, etc.
- **Action system** - Current animation/behavior control

### 2. Schedule System (schedule.h)
32+ schedule types including:
- `combat`, `talk`, `dance`, `eat`, `farm`
- `tend_shop`, `miner`, `blacksmith`, `sleep`
- `loiter`, `wander`, `patrol`, `follow_avatar`
- `street_maintenance`, `arrest_avatar`

Key methods:
- `now_what()` - Called when NPC completes current task
- `im_dormant()` - Called when NPC goes off-screen
- `ending()` - Called when switching schedules

### 3. Conversation System (usecode/conversation.h)
- Face display system (up to 2 NPC faces)
- Answer/choice management
- Text display for NPC dialogue
- Usecode integration for scripted conversations

Key methods:
- `show_face()` - Display NPC portrait
- `show_npc_message()` - Display dialogue text
- `show_avatar_choices()` - Display player response options
- `add_answer()` / `remove_answer()` - Manage dialogue choices

### 4. Usecode System (usecode/ucinternal.h)
Intrinsics for NPC interaction:
- `item_say()` - Make item/NPC speak
- `init_conversation()` - Start dialogue
- `end_conversation()` - End dialogue
- `set_conversation_slot()` - Manage conversation state

## Integration Strategy

### Bridge Layer Components

1. **ExultNPCBridge** - Connects Exult Actor to our NPCEntity
2. **DialogueHook** - Intercepts conversation system calls
3. **ScheduleOverride** - Allows cognitive AI to influence schedules
4. **BehaviorInjector** - Injects AI-driven behaviors into now_what()

### Data Flow

```
Player Input → Exult Conversation System
                    ↓
              DialogueHook
                    ↓
         HybridDialogueEngine
                    ↓
    [AIML Patterns] ←→ [TinyLLM]
                    ↓
         Response Generation
                    ↓
              DialogueHook
                    ↓
         Exult show_npc_message()
```

### Schedule Integration

```
Exult Schedule::now_what()
           ↓
    ScheduleOverride
           ↓
    NPCEntity::makeDecision()
           ↓
    [TensorLogic + Persona + Memory]
           ↓
    New Schedule/Action
           ↓
    Exult Actor::set_schedule()
```
