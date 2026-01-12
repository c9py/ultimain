/**
 * HybridDialogue.h - AIML + LLM Hybrid Dialogue System
 * 
 * Combines deterministic AIML pattern matching with creative LLM generation
 * for natural, contextual NPC dialogue.
 * 
 * Strategy:
 * 1. AIML handles common patterns (greetings, farewells, known topics)
 * 2. LLM handles novel situations requiring creativity
 * 3. Tensor logic validates responses for consistency
 * 4. Personality system modifies tone and style
 */

#pragma once

#include "aiml/AIMLEngine.h"
#include "llm/TinyLLM.h"
#include "reasoning/TensorLogic.h"
#include "persona/Persona.h"
#include <memory>
#include <functional>

namespace Ultima {
namespace NPC {
namespace Dialogue {

/**
 * Response source tracking
 */
enum class ResponseSource {
    AIML,           // Pattern-matched response
    LLM,            // Creatively generated response
    Hybrid,         // Combined AIML template + LLM fill
    Fallback,       // Default fallback response
    Cached          // Previously cached response
};

/**
 * Extended dialogue result with source tracking
 */
struct HybridDialogueResult {
    std::string response;
    ResponseSource source;
    float confidence;
    float aimlScore;
    float llmScore;
    std::string emotion;
    std::vector<std::string> suggestedTopics;
    std::vector<std::string> mentionedEntities;
    bool isConsistent;  // Validated by tensor logic
    int processingTimeMs;
};

/**
 * Dialogue context for maintaining conversation state
 */
struct DialogueContext {
    std::string npcId;
    std::string playerId;
    std::vector<std::pair<std::string, std::string>> recentExchanges;
    std::unordered_map<std::string, std::string> topicMemory;
    std::unordered_map<std::string, float> relationshipFactors;
    std::string currentQuest;
    std::string currentLocation;
    int turnCount = 0;
    
    void addExchange(const std::string& player, const std::string& npc) {
        recentExchanges.push_back({player, npc});
        if (recentExchanges.size() > 10) {
            recentExchanges.erase(recentExchanges.begin());
        }
        turnCount++;
    }
    
    void rememberTopic(const std::string& topic, const std::string& info) {
        topicMemory[topic] = info;
    }
    
    std::string getTopicMemory(const std::string& topic) const {
        auto it = topicMemory.find(topic);
        return it != topicMemory.end() ? it->second : "";
    }
};

/**
 * Configuration for the hybrid dialogue system
 */
struct HybridConfig {
    // Threshold below which AIML confidence triggers LLM
    float aimlConfidenceThreshold = 0.6f;
    
    // Weight for AIML vs LLM when both provide responses
    float aimlWeight = 0.7f;
    float llmWeight = 0.3f;
    
    // Enable tensor logic validation
    bool enableConsistencyCheck = true;
    
    // Enable personality post-processing
    bool enablePersonalityModifier = true;
    
    // Enable response caching
    bool enableCaching = true;
    int cacheMaxSize = 1000;
    
    // LLM configuration
    LLM::LLMConfig llmConfig;
    
    // Fallback responses when both systems fail
    std::vector<std::string> fallbackResponses = {
        "Hmm, I'm not sure what to say about that.",
        "Could you rephrase that?",
        "I don't quite follow. What do you mean?",
        "That's... interesting. Tell me more."
    };
};

/**
 * HybridDialogueEngine - Main dialogue generation system
 * 
 * Intelligently routes dialogue requests between AIML and LLM
 * based on confidence scores and context.
 */
class HybridDialogueEngine {
public:
    HybridDialogueEngine();
    ~HybridDialogueEngine();
    
    /**
     * Initialize the hybrid engine
     * @param config Configuration settings
     * @param aimlPath Path to AIML pattern files
     * @return true if initialization successful
     */
    bool initialize(const HybridConfig& config, const std::string& aimlPath = "");
    
    /**
     * Generate a response using the hybrid approach
     * @param input Player's input
     * @param npcContext NPC's context and personality
     * @param dialogueContext Conversation context
     * @return Hybrid dialogue result
     */
    HybridDialogueResult generateResponse(
        const std::string& input,
        const LLM::NPCContext& npcContext,
        DialogueContext& dialogueContext
    );
    
    /**
     * Generate response with explicit source preference
     */
    HybridDialogueResult generateFromAIML(
        const std::string& input,
        const LLM::NPCContext& npcContext
    );
    
    HybridDialogueResult generateFromLLM(
        const std::string& input,
        const LLM::NPCContext& npcContext,
        DialogueContext& dialogueContext
    );
    
    /**
     * Validate a response for logical consistency
     * @param response The response to validate
     * @param npcContext NPC's context
     * @return true if response is consistent
     */
    bool validateResponse(
        const std::string& response,
        const LLM::NPCContext& npcContext
    );
    
    /**
     * Get statistics about dialogue generation
     */
    struct Stats {
        int totalRequests = 0;
        int aimlResponses = 0;
        int llmResponses = 0;
        int hybridResponses = 0;
        int fallbackResponses = 0;
        int cacheHits = 0;
        int consistencyFailures = 0;
        double avgProcessingTimeMs = 0.0;
    };
    Stats getStats() const;
    
    /**
     * Clear the response cache
     */
    void clearCache();
    
    /**
     * Update configuration
     */
    void setConfig(const HybridConfig& config);
    
    /**
     * Register custom AIML patterns at runtime
     */
    void addPattern(const std::string& pattern, const std::string& response);
    
    /**
     * Register a callback for when LLM is used
     */
    using LLMCallback = std::function<void(const std::string& input, const std::string& output)>;
    void setLLMCallback(LLMCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    float scoreAIMLResponse(const std::string& response, const std::string& input);
    std::string combineResponses(const std::string& aiml, const std::string& llm, float aimlScore);
    std::string applyPersonality(const std::string& response, const LLM::NPCContext& ctx);
    std::string getCachedResponse(const std::string& key);
    void cacheResponse(const std::string& key, const std::string& response);
};

/**
 * DialogueDirector - High-level dialogue management
 * 
 * Manages multiple NPCs, conversation flow, and quest-related dialogue.
 */
class DialogueDirector {
public:
    DialogueDirector();
    ~DialogueDirector();
    
    /**
     * Initialize the director
     */
    bool initialize(const HybridConfig& config);
    
    /**
     * Register an NPC with the dialogue system
     */
    void registerNPC(const std::string& npcId, const LLM::NPCContext& context);
    
    /**
     * Start a conversation with an NPC
     */
    std::string startConversation(const std::string& npcId, const std::string& playerId);
    
    /**
     * Continue a conversation
     */
    HybridDialogueResult continueConversation(
        const std::string& npcId,
        const std::string& playerId,
        const std::string& playerInput
    );
    
    /**
     * End a conversation
     */
    std::string endConversation(const std::string& npcId, const std::string& playerId);
    
    /**
     * Inject quest-related dialogue
     */
    void injectQuestDialogue(
        const std::string& npcId,
        const std::string& questId,
        const std::vector<std::pair<std::string, std::string>>& dialogueOptions
    );
    
    /**
     * Update NPC's knowledge
     */
    void updateNPCKnowledge(const std::string& npcId, const std::string& fact);
    
    /**
     * Get conversation history
     */
    std::vector<std::pair<std::string, std::string>> getConversationHistory(
        const std::string& npcId,
        const std::string& playerId
    );
    
    /**
     * Check if NPC is in conversation
     */
    bool isInConversation(const std::string& npcId) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * TopicExtractor - Extract topics and entities from dialogue
 */
class TopicExtractor {
public:
    struct ExtractionResult {
        std::vector<std::string> topics;
        std::vector<std::string> entities;
        std::vector<std::string> keywords;
        std::string intent;
        std::string sentiment;
    };
    
    static ExtractionResult extract(const std::string& text);
    static std::vector<std::string> extractKeywords(const std::string& text);
    static std::string detectIntent(const std::string& text);
    static std::string detectSentiment(const std::string& text);
};

/**
 * ResponseTemplates - Pre-defined response templates for common situations
 */
class ResponseTemplates {
public:
    // Greeting templates
    static std::string greeting(const std::string& npcName, const std::string& mood);
    
    // Farewell templates
    static std::string farewell(const std::string& npcName, const std::string& mood);
    
    // Quest-related templates
    static std::string questOffer(const std::string& questName, const std::string& description);
    static std::string questAccepted(const std::string& questName);
    static std::string questDeclined(const std::string& questName);
    static std::string questComplete(const std::string& questName, const std::string& reward);
    
    // Commerce templates
    static std::string shopGreeting(const std::string& shopType);
    static std::string buyConfirm(const std::string& item, int price);
    static std::string sellConfirm(const std::string& item, int price);
    static std::string notEnoughGold(int required, int have);
    
    // Information templates
    static std::string locationInfo(const std::string& location, const std::string& description);
    static std::string personInfo(const std::string& person, const std::string& description);
    static std::string rumor(const std::string& rumorText);
    
    // Emotional responses
    static std::string angry(const std::string& reason);
    static std::string happy(const std::string& reason);
    static std::string sad(const std::string& reason);
    static std::string afraid(const std::string& reason);
    static std::string surprised(const std::string& reason);
};

} // namespace Dialogue
} // namespace NPC
} // namespace Ultima
