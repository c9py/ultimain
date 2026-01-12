/**
 * dialogue_demo.cpp - Demonstration of the Hybrid Dialogue System
 * 
 * This demo showcases the integration of AIML pattern matching
 * with the TinyLLM for creative NPC dialogue generation.
 */

#include <iostream>
#include <string>
#include "llm/TinyLLM.h"
#include "aiml/HybridDialogue.h"

using namespace Ultima::NPC;
using namespace Ultima::NPC::LLM;
using namespace Ultima::NPC::Dialogue;

void printSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

void printResult(const HybridDialogueResult& result) {
    std::cout << "Response: " << result.response << "\n";
    std::cout << "Source: ";
    switch (result.source) {
        case ResponseSource::AIML: std::cout << "AIML Pattern"; break;
        case ResponseSource::LLM: std::cout << "LLM Generated"; break;
        case ResponseSource::Hybrid: std::cout << "Hybrid (AIML + LLM)"; break;
        case ResponseSource::Fallback: std::cout << "Fallback"; break;
        case ResponseSource::Cached: std::cout << "Cached"; break;
    }
    std::cout << "\n";
    std::cout << "Confidence: " << (result.confidence * 100) << "%\n";
    if (!result.emotion.empty()) {
        std::cout << "Emotion: " << result.emotion << "\n";
    }
    std::cout << "Processing time: " << result.processingTimeMs << "ms\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║     Ultima NPC Hybrid Dialogue System Demo               ║\n";
    std::cout << "║     AIML + TinyLLM Integration                           ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n\n";

    // =========================================================================
    // Test 1: TinyLLM Direct Generation
    // =========================================================================
    std::cout << "TEST 1: TinyLLM Direct Dialogue Generation\n";
    printSeparator();
    
    TinyLLM llm;
    LLMConfig config;
    config.temperature = 0.7f;
    config.maxTokens = 64;
    
    if (llm.initialize(config)) {
        std::cout << "TinyLLM initialized successfully!\n";
        std::cout << llm.getModelInfo() << "\n\n";
        
        // Create NPC context
        NPCContext merchant;
        merchant.name = "Gareth";
        merchant.occupation = "Merchant";
        merchant.personality = "Friendly, talkative, shrewd businessman";
        merchant.currentMood = "happy";
        merchant.location = "Britain Market Square";
        merchant.knownFacts = {
            "The Avatar saved Britannia",
            "Lord British rules from his castle",
            "Bandits have been seen on the roads"
        };
        
        // Test various inputs
        std::vector<std::string> testInputs = {
            "Hello there!",
            "What do you have for sale?",
            "Tell me about the bandits",
            "Where can I find Lord British?",
            "Goodbye!"
        };
        
        for (const auto& input : testInputs) {
            std::cout << "Player: " << input << "\n";
            
            DialogueRequest request;
            request.playerInput = input;
            request.npcContext = merchant;
            
            auto result = llm.generateDialogue(request);
            std::cout << "Gareth: " << result.response << "\n";
            std::cout << "(Confidence: " << (result.confidence * 100) << "%, "
                      << "Emotion: " << result.emotion << ")\n\n";
        }
    } else {
        std::cout << "Failed to initialize TinyLLM\n";
    }

    // =========================================================================
    // Test 2: Hybrid Dialogue Engine
    // =========================================================================
    printSeparator();
    std::cout << "TEST 2: Hybrid Dialogue Engine (AIML + LLM)\n";
    printSeparator();
    
    HybridConfig hybridConfig;
    hybridConfig.aimlConfidenceThreshold = 0.6f;
    hybridConfig.enablePersonalityModifier = true;
    hybridConfig.enableCaching = true;
    hybridConfig.llmConfig = config;
    
    HybridDialogueEngine engine;
    if (engine.initialize(hybridConfig)) {
        std::cout << "Hybrid Dialogue Engine initialized!\n\n";
        
        // Create a guard NPC
        NPCContext guard;
        guard.name = "Captain Marcus";
        guard.occupation = "City Guard Captain";
        guard.personality = "Stern, dutiful, protective";
        guard.currentMood = "neutral";
        guard.location = "Britain City Gates";
        guard.knownFacts = {
            "The city is under martial law",
            "Strangers must register at the castle",
            "There was a theft at the jeweler's last night"
        };
        
        DialogueContext context;
        context.npcId = "guard_marcus";
        context.playerId = "player_1";
        context.currentLocation = "Britain City Gates";
        
        std::vector<std::string> guardInputs = {
            "Hello, guard",
            "What's happening in the city?",
            "I heard there was a theft?",
            "Can I enter the city?",
            "Thank you for your service"
        };
        
        for (const auto& input : guardInputs) {
            std::cout << "Player: " << input << "\n";
            auto result = engine.generateResponse(input, guard, context);
            std::cout << "Captain Marcus: ";
            printResult(result);
            std::cout << "\n";
        }
        
        // Print statistics
        auto stats = engine.getStats();
        std::cout << "\n--- Engine Statistics ---\n";
        std::cout << "Total requests: " << stats.totalRequests << "\n";
        std::cout << "AIML responses: " << stats.aimlResponses << "\n";
        std::cout << "LLM responses: " << stats.llmResponses << "\n";
        std::cout << "Hybrid responses: " << stats.hybridResponses << "\n";
        std::cout << "Fallback responses: " << stats.fallbackResponses << "\n";
        std::cout << "Cache hits: " << stats.cacheHits << "\n";
        std::cout << "Avg processing time: " << stats.avgProcessingTimeMs << "ms\n";
    }

    // =========================================================================
    // Test 3: Dialogue Director (Multi-NPC Management)
    // =========================================================================
    printSeparator();
    std::cout << "TEST 3: Dialogue Director (Multi-NPC Conversations)\n";
    printSeparator();
    
    DialogueDirector director;
    if (director.initialize(hybridConfig)) {
        std::cout << "Dialogue Director initialized!\n\n";
        
        // Register multiple NPCs
        NPCContext innkeeper;
        innkeeper.name = "Martha";
        innkeeper.occupation = "Innkeeper";
        innkeeper.personality = "Warm, motherly, gossipy";
        innkeeper.currentMood = "happy";
        innkeeper.location = "The Blue Boar Inn";
        
        NPCContext blacksmith;
        blacksmith.name = "Thorin";
        blacksmith.occupation = "Blacksmith";
        blacksmith.personality = "Gruff, hardworking, honest";
        blacksmith.currentMood = "neutral";
        blacksmith.location = "Thorin's Forge";
        
        director.registerNPC("innkeeper_martha", innkeeper);
        director.registerNPC("blacksmith_thorin", blacksmith);
        
        // Conversation with innkeeper
        std::cout << "--- Conversation at The Blue Boar Inn ---\n\n";
        std::string greeting = director.startConversation("innkeeper_martha", "player_1");
        std::cout << "Martha: " << greeting << "\n\n";
        
        auto result1 = director.continueConversation("innkeeper_martha", "player_1", 
            "Do you have any rooms available?");
        std::cout << "Player: Do you have any rooms available?\n";
        std::cout << "Martha: " << result1.response << "\n\n";
        
        auto result2 = director.continueConversation("innkeeper_martha", "player_1",
            "Have you heard any interesting rumors?");
        std::cout << "Player: Have you heard any interesting rumors?\n";
        std::cout << "Martha: " << result2.response << "\n\n";
        
        std::string farewell = director.endConversation("innkeeper_martha", "player_1");
        std::cout << "Martha: " << farewell << "\n";
        
        // Conversation with blacksmith
        std::cout << "\n--- Conversation at Thorin's Forge ---\n\n";
        greeting = director.startConversation("blacksmith_thorin", "player_1");
        std::cout << "Thorin: " << greeting << "\n\n";
        
        auto result3 = director.continueConversation("blacksmith_thorin", "player_1",
            "Can you repair my sword?");
        std::cout << "Player: Can you repair my sword?\n";
        std::cout << "Thorin: " << result3.response << "\n\n";
        
        farewell = director.endConversation("blacksmith_thorin", "player_1");
        std::cout << "Thorin: " << farewell << "\n";
    }

    // =========================================================================
    // Test 4: Input Analysis
    // =========================================================================
    printSeparator();
    std::cout << "TEST 4: Topic and Intent Extraction\n";
    printSeparator();
    
    std::vector<std::string> analysisInputs = {
        "Where can I find the ancient treasure?",
        "I want to buy a magic sword!",
        "Have you heard about the dragon in the mountains?",
        "I'm looking for work. Do you have any quests?",
        "Thank you so much for your help!"
    };
    
    for (const auto& input : analysisInputs) {
        std::cout << "Input: \"" << input << "\"\n";
        auto extraction = TopicExtractor::extract(input);
        std::cout << "  Intent: " << extraction.intent << "\n";
        std::cout << "  Sentiment: " << extraction.sentiment << "\n";
        std::cout << "  Topics: ";
        for (const auto& topic : extraction.topics) {
            std::cout << topic << " ";
        }
        std::cout << "\n  Keywords: ";
        for (const auto& kw : extraction.keywords) {
            std::cout << kw << " ";
        }
        std::cout << "\n\n";
    }

    // =========================================================================
    // Test 5: Personality Modification
    // =========================================================================
    printSeparator();
    std::cout << "TEST 5: Personality-Based Response Modification\n";
    printSeparator();
    
    std::string baseResponse = "I can help you with that.";
    
    std::cout << "Base response: \"" << baseResponse << "\"\n\n";
    
    // High extraversion
    std::string extraverted = PersonalityModifier::modify(
        baseResponse, 0.5f, 0.5f, 0.9f, 0.5f, 0.3f);
    std::cout << "High Extraversion: \"" << extraverted << "\"\n";
    
    // Low extraversion
    std::string introverted = PersonalityModifier::modify(
        baseResponse, 0.5f, 0.5f, 0.2f, 0.5f, 0.3f);
    std::cout << "Low Extraversion: \"" << introverted << "\"\n";
    
    // High agreeableness
    std::string agreeable = PersonalityModifier::modify(
        baseResponse, 0.5f, 0.5f, 0.5f, 0.9f, 0.3f);
    std::cout << "High Agreeableness: \"" << agreeable << "\"\n";
    
    // High neuroticism
    std::string neurotic = PersonalityModifier::modify(
        baseResponse, 0.5f, 0.5f, 0.5f, 0.5f, 0.9f);
    std::cout << "High Neuroticism: \"" << neurotic << "\"\n";
    
    // Emotional coloring
    std::cout << "\nEmotional Coloring:\n";
    std::cout << "Joy: \"" << PersonalityModifier::addEmotion(baseResponse, "joy", 0.8f) << "\"\n";
    std::cout << "Sadness: \"" << PersonalityModifier::addEmotion(baseResponse, "sadness", 0.8f) << "\"\n";
    std::cout << "Anger: \"" << PersonalityModifier::addEmotion(baseResponse, "anger", 0.8f) << "\"\n";
    std::cout << "Fear: \"" << PersonalityModifier::addEmotion(baseResponse, "fear", 0.8f) << "\"\n";

    printSeparator();
    std::cout << "Demo complete! The hybrid dialogue system combines:\n";
    std::cout << "  • AIML pattern matching for common interactions\n";
    std::cout << "  • TinyLLM for creative, contextual responses\n";
    std::cout << "  • Personality modifiers for character consistency\n";
    std::cout << "  • Topic extraction for conversation flow\n";
    std::cout << "  • Response caching for performance\n";
    printSeparator();

    return 0;
}
