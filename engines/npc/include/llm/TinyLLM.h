/**
 * TinyLLM.h - Lightweight LLM Integration for NPC Dialogue
 * 
 * Provides a simple interface for generating creative NPC dialogue
 * using small language models (GGUF format via llama.cpp).
 * 
 * Design Philosophy:
 * - AIML handles deterministic, pattern-matched responses
 * - TinyLLM handles novel situations requiring creativity
 * - Hybrid system combines both for natural dialogue
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace Ultima {
namespace NPC {
namespace LLM {

/**
 * Configuration for the TinyLLM engine
 */
struct LLMConfig {
    std::string modelPath;          // Path to GGUF model file
    int contextSize = 2048;         // Context window size
    int maxTokens = 256;            // Max tokens to generate
    float temperature = 0.7f;       // Sampling temperature (0.0-2.0)
    float topP = 0.9f;              // Nucleus sampling threshold
    int topK = 40;                  // Top-K sampling
    float repeatPenalty = 1.1f;     // Repetition penalty
    int threads = 4;                // Number of CPU threads
    bool useGPU = false;            // Use GPU acceleration if available
};

/**
 * Represents a message in a conversation
 */
struct ChatMessage {
    enum class Role { System, User, Assistant };
    Role role;
    std::string content;
    
    static ChatMessage system(const std::string& content) {
        return {Role::System, content};
    }
    static ChatMessage user(const std::string& content) {
        return {Role::User, content};
    }
    static ChatMessage assistant(const std::string& content) {
        return {Role::Assistant, content};
    }
};

/**
 * NPC personality context for dialogue generation
 */
struct NPCContext {
    std::string name;
    std::string occupation;
    std::string personality;        // Big Five summary
    std::string currentMood;        // Current emotional state
    std::string location;           // Where the NPC is
    std::string recentEvents;       // Recent happenings
    std::vector<std::string> knownFacts;  // Facts the NPC knows
    std::vector<std::string> secrets;     // Things NPC won't reveal easily
    
    // Generate a system prompt from this context
    std::string toSystemPrompt() const;
};

/**
 * Dialogue generation request
 */
struct DialogueRequest {
    std::string playerInput;        // What the player said
    NPCContext npcContext;          // NPC's context
    std::vector<ChatMessage> history; // Recent conversation history
    std::string situationalContext; // Current game situation
    bool requiresCreativity = true; // Whether to use LLM or fallback
};

/**
 * Dialogue generation result
 */
struct DialogueResult {
    std::string response;           // The generated response
    float confidence;               // Confidence in the response (0-1)
    std::string emotion;            // Detected/generated emotion
    std::vector<std::string> suggestedActions; // Actions NPC might take
    bool wasGenerated;              // True if LLM generated, false if fallback
    int tokensUsed;                 // Number of tokens consumed
};

/**
 * Callback for streaming responses
 */
using StreamCallback = std::function<void(const std::string& token)>;

/**
 * TinyLLM - Lightweight Language Model for NPC Dialogue
 * 
 * This class provides a simple interface for generating creative
 * NPC dialogue using small language models. It's designed to work
 * alongside the AIML engine for hybrid dialogue generation.
 */
class TinyLLM {
public:
    TinyLLM();
    ~TinyLLM();
    
    // Prevent copying
    TinyLLM(const TinyLLM&) = delete;
    TinyLLM& operator=(const TinyLLM&) = delete;
    
    /**
     * Initialize the LLM with configuration
     * @param config LLM configuration
     * @return true if initialization successful
     */
    bool initialize(const LLMConfig& config);
    
    /**
     * Check if the LLM is ready
     */
    bool isReady() const;
    
    /**
     * Generate dialogue for an NPC
     * @param request The dialogue request
     * @return Generated dialogue result
     */
    DialogueResult generateDialogue(const DialogueRequest& request);
    
    /**
     * Generate dialogue with streaming output
     * @param request The dialogue request
     * @param callback Called for each generated token
     * @return Final dialogue result
     */
    DialogueResult generateDialogueStreaming(
        const DialogueRequest& request,
        StreamCallback callback
    );
    
    /**
     * Generate a simple completion (no chat format)
     * @param prompt The prompt to complete
     * @param maxTokens Maximum tokens to generate
     * @return Generated text
     */
    std::string complete(const std::string& prompt, int maxTokens = 128);
    
    /**
     * Generate multiple response variations
     * @param request The dialogue request
     * @param count Number of variations to generate
     * @return Vector of dialogue results
     */
    std::vector<DialogueResult> generateVariations(
        const DialogueRequest& request,
        int count = 3
    );
    
    /**
     * Analyze player input for intent and emotion
     * @param input Player's input text
     * @return Analysis results as key-value pairs
     */
    std::unordered_map<std::string, std::string> analyzeInput(
        const std::string& input
    );
    
    /**
     * Get token count for a string
     * @param text Text to tokenize
     * @return Number of tokens
     */
    int getTokenCount(const std::string& text) const;
    
    /**
     * Clear the conversation context
     */
    void clearContext();
    
    /**
     * Get model information
     */
    std::string getModelInfo() const;
    
    /**
     * Set the temperature for generation
     */
    void setTemperature(float temp);
    
    /**
     * Shutdown and release resources
     */
    void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    std::string buildPrompt(const DialogueRequest& request) const;
    std::string formatChatHistory(const std::vector<ChatMessage>& history) const;
    DialogueResult parseResponse(const std::string& raw, const DialogueRequest& request) const;
};

/**
 * DialogueGenerator - High-level dialogue generation combining AIML and LLM
 * 
 * This class provides a unified interface for NPC dialogue that
 * intelligently routes requests between AIML patterns and LLM generation.
 */
class DialogueGenerator {
public:
    DialogueGenerator();
    ~DialogueGenerator();
    
    /**
     * Initialize with LLM config and AIML patterns
     * @param llmConfig Configuration for the LLM
     * @param aimlPath Path to AIML pattern files (optional)
     */
    bool initialize(const LLMConfig& llmConfig, const std::string& aimlPath = "");
    
    /**
     * Generate dialogue using hybrid approach
     * 
     * Strategy:
     * 1. Try AIML pattern matching first
     * 2. If no match or low confidence, use LLM
     * 3. Post-process for personality consistency
     * 
     * @param request Dialogue request
     * @return Generated dialogue
     */
    DialogueResult generate(const DialogueRequest& request);
    
    /**
     * Force LLM generation (bypass AIML)
     */
    DialogueResult generateCreative(const DialogueRequest& request);
    
    /**
     * Force AIML pattern matching (bypass LLM)
     */
    DialogueResult generatePattern(const DialogueRequest& request);
    
    /**
     * Set the creativity threshold
     * Requests with AIML confidence below this use LLM
     * @param threshold Value between 0 and 1
     */
    void setCreativityThreshold(float threshold);
    
    /**
     * Enable/disable personality post-processing
     */
    void setPersonalityProcessing(bool enabled);
    
    /**
     * Get statistics about generation
     */
    struct Stats {
        int totalRequests = 0;
        int aimlResponses = 0;
        int llmResponses = 0;
        int fallbackResponses = 0;
        double avgResponseTime = 0.0;
    };
    Stats getStats() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * Personality-aware response modifier
 * 
 * Takes a base response and modifies it to match
 * the NPC's personality traits.
 */
class PersonalityModifier {
public:
    /**
     * Modify response based on Big Five traits
     * @param response Original response
     * @param openness 0-1 (conventional to creative)
     * @param conscientiousness 0-1 (careless to organized)
     * @param extraversion 0-1 (reserved to outgoing)
     * @param agreeableness 0-1 (challenging to cooperative)
     * @param neuroticism 0-1 (calm to anxious)
     * @return Modified response
     */
    static std::string modify(
        const std::string& response,
        float openness,
        float conscientiousness,
        float extraversion,
        float agreeableness,
        float neuroticism
    );
    
    /**
     * Add emotional coloring to response
     * @param response Original response
     * @param emotion Current emotion (joy, sadness, anger, fear, etc.)
     * @param intensity Emotion intensity 0-1
     * @return Modified response
     */
    static std::string addEmotion(
        const std::string& response,
        const std::string& emotion,
        float intensity
    );
    
    /**
     * Add speech patterns based on occupation/background
     * @param response Original response
     * @param occupation NPC's occupation
     * @param education Education level (0-1)
     * @return Modified response
     */
    static std::string addSpeechPattern(
        const std::string& response,
        const std::string& occupation,
        float education
    );
};

/**
 * Conversation memory for maintaining context
 */
class ConversationMemory {
public:
    ConversationMemory(int maxTurns = 10);
    
    void addTurn(const std::string& speaker, const std::string& text);
    void addPlayerInput(const std::string& input);
    void addNPCResponse(const std::string& response);
    
    std::vector<ChatMessage> getHistory() const;
    std::string getSummary() const;
    
    void clear();
    void setMaxTurns(int max);
    
    // Semantic search in history
    std::vector<std::string> findRelevant(const std::string& query, int count = 3) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace LLM
} // namespace NPC
} // namespace Ultima
