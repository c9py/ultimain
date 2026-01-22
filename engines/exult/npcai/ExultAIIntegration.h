/**
 * ExultAIIntegration.h - Drop-in integration macros for Exult NPC AI
 *
 * Include this header in Exult source files that need AI integration.
 * Provides macros that can be inserted into existing code with minimal changes.
 *
 * Usage:
 *   1. Include this header in the relevant Exult source files
 *   2. Call NPCAI_INIT() during game startup
 *   3. Call NPCAI_SHUTDOWN() during game shutdown
 *   4. Use the hook macros in conversation.cc and schedule.cc
 *
 * Part of the Ultima Integration Project
 */

#ifndef EXULT_AI_INTEGRATION_H
#define EXULT_AI_INTEGRATION_H

#include "ExultNPCBridge.h"
#include "NPCProfileLoader.h"
#include "DialogueHooks.h"
#include "ScheduleIntegration.h"

// =============================================================================
// INITIALIZATION MACROS
// =============================================================================

/**
 * Initialize the NPC AI system
 * Call this during Exult startup (e.g., in Game::init() or similar)
 *
 * @param data_dir Path to NPC AI data directory
 */
#define NPCAI_INIT(data_dir) \
    do { \
        if (!Ultima::Exult::NPCAIInitializer::initialize(data_dir)) { \
            /* Log warning but don't fail - AI is optional */ \
        } \
    } while(0)

/**
 * Shutdown the NPC AI system
 * Call this during Exult shutdown
 */
#define NPCAI_SHUTDOWN() \
    Ultima::Exult::NPCAIInitializer::shutdown()

/**
 * Update NPC AI each game tick
 * Call this in the main game loop
 *
 * @param delta_time Time since last update in seconds
 */
#define NPCAI_UPDATE(delta_time) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            Ultima::Exult::NPCAIInitializer::update(delta_time); \
        } \
    } while(0)

// =============================================================================
// DIALOGUE INTEGRATION MACROS
// =============================================================================

/**
 * Hook for NPC message processing
 * Use in conversation.cc where NPCs speak
 *
 * @param npc Pointer to Actor speaking
 * @param msg The message text (will be modified if AI has enhancement)
 */
#define NPCAI_HOOK_NPC_MESSAGE(npc, msg) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            msg = Ultima::Exult::ConversationAIHook::processNPCMessage(npc, msg); \
        } \
    } while(0)

/**
 * Hook for generating dialogue choices
 * Use where player dialogue options are presented
 *
 * @param npc Pointer to Actor being talked to
 * @param choices Vector of choice strings (may be modified)
 */
#define NPCAI_HOOK_DIALOGUE_CHOICES(npc, choices) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            choices = Ultima::Exult::ConversationAIHook::generateChoices(npc, choices); \
        } \
    } while(0)

/**
 * Hook for player choice selection
 * Use when player selects a dialogue option
 *
 * @param npc Pointer to Actor being talked to
 * @param choice_idx Index of selected choice
 * @param choice_text Text of selected choice
 */
#define NPCAI_HOOK_PLAYER_CHOICE(npc, choice_idx, choice_text) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            Ultima::Exult::ConversationAIHook::handleChoice(npc, choice_idx, choice_text); \
        } \
    } while(0)

/**
 * Hook for conversation start
 * Use when conversation with NPC begins
 *
 * @param npc Pointer to Actor being talked to
 */
#define NPCAI_HOOK_CONV_START(npc) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            Ultima::Exult::ExultNPCBridge::getInstance().hookInitConversation(npc); \
        } \
    } while(0)

/**
 * Hook for conversation end
 * Use when conversation with NPC ends
 *
 * @param npc Pointer to Actor that was talked to
 */
#define NPCAI_HOOK_CONV_END(npc) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            Ultima::Exult::ExultNPCBridge::getInstance().hookEndConversation(npc); \
        } \
    } while(0)

// =============================================================================
// SCHEDULE/BEHAVIOR INTEGRATION MACROS
// =============================================================================

/**
 * Hook for Schedule::now_what() - AI behavior suggestions
 * Use at the start of now_what() implementation
 *
 * @param npc Pointer to Actor
 * @param schedule_type Current schedule type (int)
 * @param ai_handled Set to true if AI handled the behavior (caller should return)
 */
#define NPCAI_HOOK_NOW_WHAT(npc, schedule_type, ai_handled) \
    do { \
        ai_handled = false; \
        if (Ultima::Exult::NPCAIInitializer::isInitialized() && \
            Ultima::Exult::ExultNPCBridge::getInstance().isRegistered(npc)) { \
            Ultima::Exult::BehaviorSuggestion suggestion = \
                Ultima::Exult::ExultNPCBridge::getInstance().suggestBehavior( \
                    npc, schedule_type, {}, {}); \
            if (suggestion.confidence > 0.7f && \
                suggestion.type != Ultima::Exult::BehaviorSuggestion::Type::CONTINUE_CURRENT) { \
                /* Apply AI suggestion - implementation depends on schedule system */ \
                ai_handled = true; \
            } \
        } \
    } while(0)

/**
 * Notify AI of NPC event
 * Use when significant events happen to NPCs
 *
 * @param npc Pointer to Actor
 * @param event_type String describing event type
 */
#define NPCAI_NOTIFY_EVENT(npc, event_type) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized() && \
            Ultima::Exult::ExultNPCBridge::getInstance().isRegistered(npc)) { \
            std::map<std::string, std::string> data; \
            Ultima::Exult::ExultNPCBridge::getInstance().notifyEvent(npc, event_type, data); \
        } \
    } while(0)

/**
 * Notify AI of NPC event with data
 *
 * @param npc Pointer to Actor
 * @param event_type String describing event type
 * @param event_data Map of event data
 */
#define NPCAI_NOTIFY_EVENT_DATA(npc, event_type, event_data) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized() && \
            Ultima::Exult::ExultNPCBridge::getInstance().isRegistered(npc)) { \
            Ultima::Exult::ExultNPCBridge::getInstance().notifyEvent(npc, event_type, event_data); \
        } \
    } while(0)

// =============================================================================
// NPC REGISTRATION MACROS
// =============================================================================

/**
 * Register an NPC with the AI system
 * Call when an NPC is loaded/created that should have AI behavior
 *
 * @param npc Pointer to Actor
 * @param profile NPCProfile for this NPC
 */
#define NPCAI_REGISTER_NPC(npc, profile) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            Ultima::Exult::ExultNPCBridge::getInstance().registerNPC(npc, profile); \
        } \
    } while(0)

/**
 * Register an NPC with auto-generated profile based on shape
 * Useful for NPCs not in the loaded profile set
 *
 * @param npc Pointer to Actor
 * @param shape NPC shape number
 * @param tile_x Tile X coordinate
 * @param tile_y Tile Y coordinate
 */
#define NPCAI_REGISTER_NPC_AUTO(npc, shape, tile_x, tile_y) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            Ultima::Exult::NPCProfile profile = \
                Ultima::Exult::NPCProfileLoader::generateDefaultProfile(shape, tile_x, tile_y); \
            Ultima::Exult::ExultNPCBridge::getInstance().registerNPC(npc, profile); \
        } \
    } while(0)

/**
 * Unregister an NPC from the AI system
 * Call when an NPC is removed/destroyed
 *
 * @param npc Pointer to Actor
 */
#define NPCAI_UNREGISTER_NPC(npc) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            Ultima::Exult::ExultNPCBridge::getInstance().unregisterNPC(npc); \
        } \
    } while(0)

// =============================================================================
// MEMORY AND RELATIONSHIP MACROS
// =============================================================================

/**
 * Record a memory for an NPC
 *
 * @param npc Pointer to Actor
 * @param memory_type Type of memory (e.g., "dialogue", "combat", "trade")
 * @param content Memory content string
 */
#define NPCAI_RECORD_MEMORY(npc, memory_type, content) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized() && \
            Ultima::Exult::ExultNPCBridge::getInstance().isRegistered(npc)) { \
            Ultima::Exult::ExultNPCBridge::getInstance().recordMemory( \
                npc, memory_type, content, 0.5f); \
        } \
    } while(0)

/**
 * Modify relationship between two NPCs
 *
 * @param npc1 First NPC
 * @param npc2 Second NPC
 * @param delta Change in relationship value (positive = more friendly)
 */
#define NPCAI_MODIFY_RELATIONSHIP(npc1, npc2, delta) \
    do { \
        if (Ultima::Exult::NPCAIInitializer::isInitialized()) { \
            Ultima::Exult::ExultNPCBridge::getInstance().modifyRelationship(npc1, npc2, delta); \
        } \
    } while(0)

// =============================================================================
// UTILITY MACROS
// =============================================================================

/**
 * Check if NPC AI is available
 */
#define NPCAI_IS_AVAILABLE() \
    Ultima::Exult::NPCAIInitializer::isInitialized()

/**
 * Check if a specific NPC has AI registered
 */
#define NPCAI_HAS_AI(npc) \
    (Ultima::Exult::NPCAIInitializer::isInitialized() && \
     Ultima::Exult::ExultNPCBridge::getInstance().isRegistered(npc))

/**
 * Get AI response for player input (use in custom dialogue handling)
 *
 * @param npc Pointer to Actor
 * @param input Player's input text
 * @param context DialogueContext with current state
 * @return AI-generated response string
 */
#define NPCAI_GET_RESPONSE(npc, input, context) \
    (Ultima::Exult::NPCAIInitializer::isInitialized() ? \
        Ultima::Exult::ExultNPCBridge::getInstance().processDialogue(npc, input, context) : \
        std::string())

#endif // EXULT_AI_INTEGRATION_H
