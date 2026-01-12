/**
 * ExultNPCBridge.h - Bridge between Exult Actor system and Ultima NPC AI
 * 
 * This module provides the integration layer that connects Exult's native
 * Actor/NPC system with the cognitive NPC AI engine, enabling intelligent
 * dialogue, behavior, and decision-making for NPCs.
 * 
 * Part of the Ultima Integration Project
 */

#ifndef EXULT_NPC_BRIDGE_H
#define EXULT_NPC_BRIDGE_H

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>

// Forward declarations for Exult types
class Actor;
class Schedule;
class Conversation;
class Usecode_value;

// Forward declarations for NPC AI types
namespace Ultima {
namespace NPC {
    class NPCEntity;
    namespace Dialogue {
        class HybridDialogueEngine;
        struct HybridDialogueResult;
    }
    namespace Persona {
        class NPCPersona;
    }
}
}

namespace Ultima {
namespace Exult {

/**
 * NPCProfile - Configuration for a cognitive NPC
 */
struct NPCProfile {
    std::string id;                    // Unique NPC identifier
    std::string name;                  // Display name
    std::string profession;            // Job/role
    std::string personality;           // Personality type
    std::string backstory;             // Background narrative
    
    // Personality traits (Big Five)
    float openness = 0.5f;
    float conscientiousness = 0.5f;
    float extraversion = 0.5f;
    float agreeableness = 0.5f;
    float neuroticism = 0.5f;
    
    // Knowledge domains
    std::vector<std::string> knowledgeDomains;
    
    // Initial relationships
    std::map<std::string, float> relationships;
    
    // AIML patterns file (optional)
    std::string aimlPatterns;
};

/**
 * DialogueContext - Context for a conversation
 */
struct DialogueContext {
    std::string npcId;
    std::string playerId;
    std::string location;
    int gameHour;
    int gameDay;
    std::string currentQuest;
    std::vector<std::string> recentTopics;
    std::string mood;
    float relationshipLevel;
};

/**
 * BehaviorSuggestion - AI-suggested behavior for an NPC
 */
struct BehaviorSuggestion {
    enum class Type {
        CONTINUE_CURRENT,      // Keep doing current schedule
        CHANGE_SCHEDULE,       // Switch to different schedule
        INTERACT_WITH_OBJECT,  // Use/interact with nearby object
        APPROACH_NPC,          // Move toward another NPC
        FLEE,                  // Run away
        SPEAK_SPONTANEOUS,     // Say something unprompted
        CUSTOM_ACTION          // Engine-specific action
    };
    
    Type type;
    int scheduleType;              // For CHANGE_SCHEDULE
    std::string targetId;          // For INTERACT/APPROACH
    std::string message;           // For SPEAK_SPONTANEOUS
    std::string customAction;      // For CUSTOM_ACTION
    float confidence;
    std::string reasoning;
};

/**
 * ExultNPCBridge - Main bridge class
 * 
 * Singleton that manages the connection between Exult and the NPC AI system.
 */
class ExultNPCBridge {
public:
    static ExultNPCBridge& getInstance();
    
    // Initialization
    bool initialize(const std::string& configPath = "");
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // NPC Registration
    bool registerNPC(Actor* actor, const NPCProfile& profile);
    bool unregisterNPC(Actor* actor);
    bool isRegistered(Actor* actor) const;
    
    // Get the cognitive entity for an actor
    NPC::NPCEntity* getCognitiveNPC(Actor* actor);
    const NPC::NPCEntity* getCognitiveNPC(Actor* actor) const;
    
    // Dialogue Integration
    
    /**
     * Process player input and generate NPC response
     * Called when player speaks to an NPC
     */
    std::string processDialogue(
        Actor* npc,
        const std::string& playerInput,
        const DialogueContext& context
    );
    
    /**
     * Get dialogue choices for the player
     * Returns a list of contextual response options
     */
    std::vector<std::string> getDialogueChoices(
        Actor* npc,
        const DialogueContext& context
    );
    
    /**
     * Start a conversation with an NPC
     */
    void startConversation(Actor* npc, const DialogueContext& context);
    
    /**
     * End the current conversation
     */
    void endConversation(Actor* npc);
    
    // Behavior Integration
    
    /**
     * Get AI-suggested behavior for an NPC
     * Called from Schedule::now_what() to get intelligent behavior
     */
    BehaviorSuggestion suggestBehavior(
        Actor* npc,
        int currentSchedule,
        const std::vector<std::string>& nearbyObjects,
        const std::vector<std::string>& nearbyNPCs
    );
    
    /**
     * Notify the AI of an event affecting an NPC
     */
    void notifyEvent(
        Actor* npc,
        const std::string& eventType,
        const std::map<std::string, std::string>& eventData
    );
    
    /**
     * Update NPC state (called each game tick)
     */
    void update(Actor* npc, double deltaTime);
    
    // Relationship System
    
    /**
     * Get relationship level between two NPCs
     */
    float getRelationship(Actor* npc1, Actor* npc2) const;
    
    /**
     * Modify relationship between two NPCs
     */
    void modifyRelationship(Actor* npc1, Actor* npc2, float delta);
    
    // Memory System
    
    /**
     * Record a memory for an NPC
     */
    void recordMemory(
        Actor* npc,
        const std::string& memoryType,
        const std::string& content,
        float importance = 0.5f
    );
    
    /**
     * Query NPC's memory
     */
    std::vector<std::string> queryMemory(
        Actor* npc,
        const std::string& query,
        int maxResults = 5
    );
    
    // Usecode Integration Hooks
    
    /**
     * Hook for usecode item_say intrinsic
     * Allows AI to modify or enhance NPC speech
     */
    std::string hookItemSay(Actor* npc, const std::string& originalText);
    
    /**
     * Hook for conversation initialization
     */
    void hookInitConversation(Actor* npc);
    
    /**
     * Hook for conversation end
     */
    void hookEndConversation(Actor* npc);
    
    // Configuration
    
    /**
     * Set the LLM model path for creative dialogue
     */
    void setLLMModelPath(const std::string& path);
    
    /**
     * Set the AIML patterns directory
     */
    void setAIMLPatternsPath(const std::string& path);
    
    /**
     * Enable/disable LLM fallback for unknown inputs
     */
    void setLLMFallbackEnabled(bool enabled);
    
    // Callbacks for Exult integration
    using DialogueCallback = std::function<void(const std::string&)>;
    using BehaviorCallback = std::function<void(const BehaviorSuggestion&)>;
    
    void setDialogueCallback(DialogueCallback callback);
    void setBehaviorCallback(BehaviorCallback callback);

private:
    ExultNPCBridge();
    ~ExultNPCBridge();
    ExultNPCBridge(const ExultNPCBridge&) = delete;
    ExultNPCBridge& operator=(const ExultNPCBridge&) = delete;
    
    bool initialized_ = false;
    
    // Actor ID to NPCEntity mapping
    std::map<int, std::unique_ptr<NPC::NPCEntity>> cognitiveNPCs_;
    
    // Actor pointer to ID mapping
    std::map<Actor*, int> actorToId_;
    
    // Dialogue engine
    std::unique_ptr<NPC::Dialogue::HybridDialogueEngine> dialogueEngine_;
    
    // Active conversations
    std::map<int, DialogueContext> activeConversations_;
    
    // Configuration
    std::string llmModelPath_;
    std::string aimlPatternsPath_;
    bool llmFallbackEnabled_ = true;
    
    // Callbacks
    DialogueCallback dialogueCallback_;
    BehaviorCallback behaviorCallback_;
    
public:
    // Helper methods (public for hook access)
    int getActorId(Actor* actor) const;
    
private:
    std::string buildPromptContext(Actor* npc, const DialogueContext& context);
    BehaviorSuggestion convertDecisionToBehavior(
        const NPC::NPCEntity* entity,
        int currentSchedule
    );
};

/**
 * ScheduleAIOverride - Mixin for AI-enhanced schedules
 * 
 * Can be used to create schedule types that consult the AI
 * for decision-making.
 */
class ScheduleAIOverride {
public:
    virtual ~ScheduleAIOverride() = default;
    
    /**
     * Called before the normal now_what() processing
     * Return true to use AI suggestion, false for normal behavior
     */
    virtual bool consultAI(Actor* npc, BehaviorSuggestion& suggestion);
    
protected:
    /**
     * Apply an AI behavior suggestion
     */
    void applyBehaviorSuggestion(Actor* npc, const BehaviorSuggestion& suggestion);
};

/**
 * ConversationAIHook - Hook into the conversation system
 */
class ConversationAIHook {
public:
    /**
     * Process NPC message before display
     * Can modify or enhance the message
     */
    static std::string processNPCMessage(
        Actor* npc,
        const std::string& originalMessage
    );
    
    /**
     * Generate dynamic dialogue choices
     */
    static std::vector<std::string> generateChoices(
        Actor* npc,
        const std::vector<std::string>& originalChoices
    );
    
    /**
     * Handle player choice selection
     */
    static void handleChoice(
        Actor* npc,
        int choiceIndex,
        const std::string& choiceText
    );
};

} // namespace Exult
} // namespace Ultima

#endif // EXULT_NPC_BRIDGE_H
