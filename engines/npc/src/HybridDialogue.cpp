/**
 * HybridDialogue.cpp - AIML + LLM Hybrid Dialogue Implementation
 */

#include "aiml/HybridDialogue.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>
#include <unordered_set>
#include <regex>

namespace Ultima {
namespace NPC {
namespace Dialogue {

//=============================================================================
// HybridDialogueEngine Implementation
//=============================================================================

struct HybridDialogueEngine::Impl {
    HybridConfig config;
    std::unique_ptr<LLM::DialogueGenerator> dialogueGen;
    Stats stats;
    LLMCallback llmCallback;
    
    // Simple response cache
    std::unordered_map<std::string, std::string> cache;
    
    // AIML-like patterns
    std::unordered_map<std::string, std::vector<std::string>> patterns;
    
    std::mt19937 rng{std::random_device{}()};
    
    void initializePatterns() {
        // Greetings
        patterns["hello"] = {
            "Greetings, traveler!",
            "Well met, friend!",
            "Hello there! What brings you here?"
        };
        patterns["hi"] = patterns["hello"];
        patterns["greetings"] = patterns["hello"];
        
        // Farewells
        patterns["bye"] = {
            "Farewell, traveler. Safe journeys!",
            "Until we meet again!",
            "Go with care, friend."
        };
        patterns["goodbye"] = patterns["bye"];
        patterns["farewell"] = patterns["bye"];
        
        // Questions about self
        patterns["who are you"] = {
            "I am {name}, {occupation} of this place.",
            "They call me {name}. I work as {occupation}."
        };
        patterns["your name"] = patterns["who are you"];
        
        // Help requests
        patterns["help"] = {
            "I'll do what I can to assist you.",
            "How may I be of service?",
            "Tell me what you need, friend."
        };
        patterns["need help"] = patterns["help"];
        
        // Commerce
        patterns["buy"] = {
            "Interested in my wares? Take a look!",
            "I have many fine goods for sale.",
            "What would you like to purchase?"
        };
        patterns["sell"] = {
            "Looking to sell something? Let me see.",
            "I might be interested in what you have.",
            "Show me what you've got."
        };
        
        // Quests
        patterns["quest"] = {
            "Seeking adventure? I may have something for you...",
            "There is a task that needs doing, if you're willing.",
            "I've heard of trouble that needs a brave soul to solve."
        };
        patterns["job"] = patterns["quest"];
        patterns["work"] = patterns["quest"];
        
        // Location
        patterns["where"] = {
            "You're in {location}.",
            "This place? It's called {location}.",
            "We're in {location}, of course."
        };
        
        // Weather/time
        patterns["weather"] = {
            "The weather seems fair enough today.",
            "It's a fine day, isn't it?",
            "Could be better, could be worse."
        };
        patterns["time"] = {
            "The hour grows late.",
            "Time passes as it always does.",
            "Best keep track of the sun, traveler."
        };
        
        // Rumors
        patterns["rumor"] = {
            "I've heard whispers of strange happenings...",
            "There's talk in the tavern about trouble to the north.",
            "Some say dark forces are stirring."
        };
        patterns["news"] = patterns["rumor"];
        patterns["gossip"] = patterns["rumor"];
    }
    
    std::pair<std::string, float> matchPattern(const std::string& input, 
                                                const LLM::NPCContext& ctx) {
        std::string lower = input;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        float bestScore = 0.0f;
        std::string bestResponse;
        
        for (const auto& [pattern, responses] : patterns) {
            if (lower.find(pattern) != std::string::npos) {
                float score = static_cast<float>(pattern.length()) / lower.length();
                score = std::min(score * 1.5f, 1.0f); // Boost for matches
                
                if (score > bestScore && !responses.empty()) {
                    bestScore = score;
                    bestResponse = responses[rng() % responses.size()];
                }
            }
        }
        
        // Replace placeholders
        if (!bestResponse.empty()) {
            size_t pos;
            while ((pos = bestResponse.find("{name}")) != std::string::npos) {
                bestResponse.replace(pos, 6, ctx.name);
            }
            while ((pos = bestResponse.find("{occupation}")) != std::string::npos) {
                bestResponse.replace(pos, 12, ctx.occupation);
            }
            while ((pos = bestResponse.find("{location}")) != std::string::npos) {
                bestResponse.replace(pos, 10, ctx.location);
            }
        }
        
        return {bestResponse, bestScore};
    }
    
    std::string generateCacheKey(const std::string& input, const std::string& npcId) {
        return npcId + ":" + input;
    }
};

HybridDialogueEngine::HybridDialogueEngine() : impl_(std::make_unique<Impl>()) {
    impl_->initializePatterns();
}

HybridDialogueEngine::~HybridDialogueEngine() = default;

bool HybridDialogueEngine::initialize(const HybridConfig& config, const std::string& aimlPath) {
    impl_->config = config;
    
    // Initialize the dialogue generator
    impl_->dialogueGen = std::make_unique<LLM::DialogueGenerator>();
    return impl_->dialogueGen->initialize(config.llmConfig, aimlPath);
}

HybridDialogueResult HybridDialogueEngine::generateResponse(
    const std::string& input,
    const LLM::NPCContext& npcContext,
    DialogueContext& dialogueContext) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    impl_->stats.totalRequests++;
    
    HybridDialogueResult result;
    result.isConsistent = true;
    
    // Check cache first
    if (impl_->config.enableCaching) {
        std::string cacheKey = impl_->generateCacheKey(input, dialogueContext.npcId);
        std::string cached = getCachedResponse(cacheKey);
        if (!cached.empty()) {
            result.response = cached;
            result.source = ResponseSource::Cached;
            result.confidence = 0.8f;
            impl_->stats.cacheHits++;
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.processingTimeMs = static_cast<int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
            );
            return result;
        }
    }
    
    // Try AIML pattern matching
    auto [aimlResponse, aimlScore] = impl_->matchPattern(input, npcContext);
    result.aimlScore = aimlScore;
    
    // Decide whether to use AIML, LLM, or hybrid
    if (aimlScore >= impl_->config.aimlConfidenceThreshold) {
        // AIML is confident enough
        result.response = aimlResponse;
        result.source = ResponseSource::AIML;
        result.confidence = aimlScore;
        impl_->stats.aimlResponses++;
    } else if (impl_->dialogueGen) {
        // Use LLM for creative response
        LLM::DialogueRequest llmRequest;
        llmRequest.playerInput = input;
        llmRequest.npcContext = npcContext;
        llmRequest.requiresCreativity = true;
        
        // Add conversation history
        for (const auto& [player, npc] : dialogueContext.recentExchanges) {
            llmRequest.history.push_back(LLM::ChatMessage::user(player));
            llmRequest.history.push_back(LLM::ChatMessage::assistant(npc));
        }
        
        auto llmResult = impl_->dialogueGen->generate(llmRequest);
        result.llmScore = llmResult.confidence;
        
        if (aimlScore > 0.3f && llmResult.confidence > 0.5f) {
            // Hybrid: combine both responses
            result.response = combineResponses(aimlResponse, llmResult.response, aimlScore);
            result.source = ResponseSource::Hybrid;
            result.confidence = (aimlScore + llmResult.confidence) / 2.0f;
            impl_->stats.hybridResponses++;
        } else if (llmResult.confidence > aimlScore) {
            // LLM is better
            result.response = llmResult.response;
            result.source = ResponseSource::LLM;
            result.confidence = llmResult.confidence;
            result.emotion = llmResult.emotion;
            impl_->stats.llmResponses++;
            
            // Callback for LLM usage
            if (impl_->llmCallback) {
                impl_->llmCallback(input, llmResult.response);
            }
        } else {
            // Use AIML even with low confidence
            result.response = aimlResponse;
            result.source = ResponseSource::AIML;
            result.confidence = aimlScore;
            impl_->stats.aimlResponses++;
        }
    } else if (!aimlResponse.empty()) {
        // No LLM, use AIML
        result.response = aimlResponse;
        result.source = ResponseSource::AIML;
        result.confidence = aimlScore;
        impl_->stats.aimlResponses++;
    }
    
    // Fallback if no response
    if (result.response.empty()) {
        result.response = impl_->config.fallbackResponses[
            impl_->rng() % impl_->config.fallbackResponses.size()
        ];
        result.source = ResponseSource::Fallback;
        result.confidence = 0.2f;
        impl_->stats.fallbackResponses++;
    }
    
    // Apply personality modifier
    if (impl_->config.enablePersonalityModifier) {
        result.response = applyPersonality(result.response, npcContext);
    }
    
    // Validate consistency
    if (impl_->config.enableConsistencyCheck) {
        result.isConsistent = validateResponse(result.response, npcContext);
        if (!result.isConsistent) {
            impl_->stats.consistencyFailures++;
        }
    }
    
    // Extract topics and entities
    auto extraction = TopicExtractor::extract(input);
    result.suggestedTopics = extraction.topics;
    result.mentionedEntities = extraction.entities;
    
    // Cache the response
    if (impl_->config.enableCaching && result.confidence > 0.5f) {
        std::string cacheKey = impl_->generateCacheKey(input, dialogueContext.npcId);
        cacheResponse(cacheKey, result.response);
    }
    
    // Update dialogue context
    dialogueContext.addExchange(input, result.response);
    
    // Calculate processing time
    auto endTime = std::chrono::high_resolution_clock::now();
    result.processingTimeMs = static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
    );
    
    // Update average processing time
    impl_->stats.avgProcessingTimeMs = 
        (impl_->stats.avgProcessingTimeMs * (impl_->stats.totalRequests - 1) + result.processingTimeMs)
        / impl_->stats.totalRequests;
    
    return result;
}

HybridDialogueResult HybridDialogueEngine::generateFromAIML(
    const std::string& input,
    const LLM::NPCContext& npcContext) {
    
    HybridDialogueResult result;
    auto [response, score] = impl_->matchPattern(input, npcContext);
    
    result.response = response.empty() ? "Hmm?" : response;
    result.source = ResponseSource::AIML;
    result.confidence = score;
    result.aimlScore = score;
    result.isConsistent = true;
    
    return result;
}

HybridDialogueResult HybridDialogueEngine::generateFromLLM(
    const std::string& input,
    const LLM::NPCContext& npcContext,
    DialogueContext& dialogueContext) {
    
    HybridDialogueResult result;
    
    if (!impl_->dialogueGen) {
        result.response = "I'm at a loss for words...";
        result.source = ResponseSource::Fallback;
        result.confidence = 0.1f;
        return result;
    }
    
    LLM::DialogueRequest request;
    request.playerInput = input;
    request.npcContext = npcContext;
    request.requiresCreativity = true;
    
    auto llmResult = impl_->dialogueGen->generateCreative(request);
    
    result.response = llmResult.response;
    result.source = ResponseSource::LLM;
    result.confidence = llmResult.confidence;
    result.llmScore = llmResult.confidence;
    result.emotion = llmResult.emotion;
    result.isConsistent = true;
    
    return result;
}

bool HybridDialogueEngine::validateResponse(
    const std::string& response,
    const LLM::NPCContext& npcContext) {
    
    // Simple consistency checks
    std::string lower = response;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Check for contradictions with NPC context
    // Example: A merchant shouldn't say they don't sell things
    if (npcContext.occupation.find("merchant") != std::string::npos ||
        npcContext.occupation.find("Merchant") != std::string::npos) {
        if (lower.find("don't sell") != std::string::npos ||
            lower.find("not a merchant") != std::string::npos) {
            return false;
        }
    }
    
    // Check for name consistency
    if (lower.find("my name is") != std::string::npos) {
        std::string npcNameLower = npcContext.name;
        std::transform(npcNameLower.begin(), npcNameLower.end(), npcNameLower.begin(), ::tolower);
        if (lower.find(npcNameLower) == std::string::npos) {
            return false;
        }
    }
    
    return true;
}

HybridDialogueEngine::Stats HybridDialogueEngine::getStats() const {
    return impl_->stats;
}

void HybridDialogueEngine::clearCache() {
    impl_->cache.clear();
}

void HybridDialogueEngine::setConfig(const HybridConfig& config) {
    impl_->config = config;
}

void HybridDialogueEngine::addPattern(const std::string& pattern, const std::string& response) {
    std::string lower = pattern;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    impl_->patterns[lower].push_back(response);
}

void HybridDialogueEngine::setLLMCallback(LLMCallback callback) {
    impl_->llmCallback = callback;
}

float HybridDialogueEngine::scoreAIMLResponse(const std::string& response, const std::string& input) {
    if (response.empty()) return 0.0f;
    
    // Score based on response length and relevance
    float lengthScore = std::min(static_cast<float>(response.length()) / 100.0f, 1.0f);
    return lengthScore * 0.8f;
}

std::string HybridDialogueEngine::combineResponses(
    const std::string& aiml, 
    const std::string& llm, 
    float aimlScore) {
    
    // Simple combination: prefer AIML structure, add LLM detail
    if (aimlScore > 0.7f) {
        return aiml;
    }
    
    // If AIML is short, append LLM elaboration
    if (aiml.length() < 50 && !llm.empty()) {
        return aiml + " " + llm;
    }
    
    return aimlScore > 0.5f ? aiml : llm;
}

std::string HybridDialogueEngine::applyPersonality(
    const std::string& response, 
    const LLM::NPCContext& ctx) {
    
    return LLM::PersonalityModifier::addEmotion(response, ctx.currentMood, 0.5f);
}

std::string HybridDialogueEngine::getCachedResponse(const std::string& key) {
    auto it = impl_->cache.find(key);
    return it != impl_->cache.end() ? it->second : "";
}

void HybridDialogueEngine::cacheResponse(const std::string& key, const std::string& response) {
    if (impl_->cache.size() >= static_cast<size_t>(impl_->config.cacheMaxSize)) {
        // Simple eviction: clear half the cache
        auto it = impl_->cache.begin();
        for (size_t i = 0; i < impl_->cache.size() / 2 && it != impl_->cache.end(); ++i) {
            it = impl_->cache.erase(it);
        }
    }
    impl_->cache[key] = response;
}

//=============================================================================
// DialogueDirector Implementation
//=============================================================================

struct DialogueDirector::Impl {
    std::unique_ptr<HybridDialogueEngine> engine;
    std::unordered_map<std::string, LLM::NPCContext> npcs;
    std::unordered_map<std::string, DialogueContext> conversations;
    std::unordered_map<std::string, std::string> activeConversations; // npcId -> playerId
    
    std::string getConversationKey(const std::string& npcId, const std::string& playerId) {
        return npcId + ":" + playerId;
    }
};

DialogueDirector::DialogueDirector() : impl_(std::make_unique<Impl>()) {}
DialogueDirector::~DialogueDirector() = default;

bool DialogueDirector::initialize(const HybridConfig& config) {
    impl_->engine = std::make_unique<HybridDialogueEngine>();
    return impl_->engine->initialize(config);
}

void DialogueDirector::registerNPC(const std::string& npcId, const LLM::NPCContext& context) {
    impl_->npcs[npcId] = context;
}

std::string DialogueDirector::startConversation(const std::string& npcId, const std::string& playerId) {
    auto it = impl_->npcs.find(npcId);
    if (it == impl_->npcs.end()) {
        return "...";
    }
    
    // Check if already in conversation
    auto activeIt = impl_->activeConversations.find(npcId);
    if (activeIt != impl_->activeConversations.end() && activeIt->second != playerId) {
        return "I'm busy with someone else right now.";
    }
    
    impl_->activeConversations[npcId] = playerId;
    
    // Create or get conversation context
    std::string convKey = impl_->getConversationKey(npcId, playerId);
    if (impl_->conversations.find(convKey) == impl_->conversations.end()) {
        DialogueContext ctx;
        ctx.npcId = npcId;
        ctx.playerId = playerId;
        ctx.currentLocation = it->second.location;
        impl_->conversations[convKey] = ctx;
    }
    
    // Generate greeting
    return ResponseTemplates::greeting(it->second.name, it->second.currentMood);
}

HybridDialogueResult DialogueDirector::continueConversation(
    const std::string& npcId,
    const std::string& playerId,
    const std::string& playerInput) {
    
    HybridDialogueResult result;
    
    auto npcIt = impl_->npcs.find(npcId);
    if (npcIt == impl_->npcs.end()) {
        result.response = "...";
        result.source = ResponseSource::Fallback;
        return result;
    }
    
    std::string convKey = impl_->getConversationKey(npcId, playerId);
    auto convIt = impl_->conversations.find(convKey);
    if (convIt == impl_->conversations.end()) {
        // Start new conversation
        startConversation(npcId, playerId);
        convIt = impl_->conversations.find(convKey);
    }
    
    return impl_->engine->generateResponse(playerInput, npcIt->second, convIt->second);
}

std::string DialogueDirector::endConversation(const std::string& npcId, const std::string& playerId) {
    auto npcIt = impl_->npcs.find(npcId);
    if (npcIt == impl_->npcs.end()) {
        return "...";
    }
    
    impl_->activeConversations.erase(npcId);
    
    return ResponseTemplates::farewell(npcIt->second.name, npcIt->second.currentMood);
}

void DialogueDirector::injectQuestDialogue(
    const std::string& npcId,
    const std::string& questId,
    const std::vector<std::pair<std::string, std::string>>& dialogueOptions) {
    
    // Add quest-specific patterns to the engine
    for (const auto& [trigger, response] : dialogueOptions) {
        impl_->engine->addPattern(trigger, response);
    }
}

void DialogueDirector::updateNPCKnowledge(const std::string& npcId, const std::string& fact) {
    auto it = impl_->npcs.find(npcId);
    if (it != impl_->npcs.end()) {
        it->second.knownFacts.push_back(fact);
    }
}

std::vector<std::pair<std::string, std::string>> DialogueDirector::getConversationHistory(
    const std::string& npcId,
    const std::string& playerId) {
    
    std::string convKey = impl_->getConversationKey(npcId, playerId);
    auto it = impl_->conversations.find(convKey);
    if (it != impl_->conversations.end()) {
        return it->second.recentExchanges;
    }
    return {};
}

bool DialogueDirector::isInConversation(const std::string& npcId) const {
    return impl_->activeConversations.find(npcId) != impl_->activeConversations.end();
}

//=============================================================================
// TopicExtractor Implementation
//=============================================================================

TopicExtractor::ExtractionResult TopicExtractor::extract(const std::string& text) {
    ExtractionResult result;
    result.keywords = extractKeywords(text);
    result.intent = detectIntent(text);
    result.sentiment = detectSentiment(text);
    
    // Extract topics based on keywords
    std::unordered_set<std::string> topicKeywords = {
        "quest", "mission", "task", "job", "adventure",
        "buy", "sell", "trade", "gold", "shop",
        "fight", "battle", "monster", "danger",
        "magic", "spell", "potion",
        "tavern", "inn", "castle", "dungeon"
    };
    
    for (const auto& kw : result.keywords) {
        std::string lower = kw;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (topicKeywords.count(lower)) {
            result.topics.push_back(lower);
        }
    }
    
    // Extract entities (capitalized words)
    std::regex entityRegex("\\b[A-Z][a-z]+\\b");
    std::sregex_iterator it(text.begin(), text.end(), entityRegex);
    std::sregex_iterator end;
    while (it != end) {
        result.entities.push_back(it->str());
        ++it;
    }
    
    return result;
}

std::vector<std::string> TopicExtractor::extractKeywords(const std::string& text) {
    std::vector<std::string> keywords;
    std::unordered_set<std::string> stopwords = {
        "the", "a", "an", "is", "are", "was", "were", "be", "been",
        "have", "has", "had", "do", "does", "did", "will", "would",
        "could", "should", "may", "might", "must", "shall", "can",
        "i", "you", "he", "she", "it", "we", "they", "me", "him",
        "her", "us", "them", "my", "your", "his", "its", "our", "their",
        "this", "that", "these", "those", "what", "which", "who",
        "and", "or", "but", "if", "then", "so", "because", "as",
        "to", "of", "in", "for", "on", "with", "at", "by", "from"
    };
    
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        // Remove punctuation
        word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());
        
        std::string lower = word;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (word.length() > 2 && stopwords.find(lower) == stopwords.end()) {
            keywords.push_back(word);
        }
    }
    
    return keywords;
}

std::string TopicExtractor::detectIntent(const std::string& text) {
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower.find("?") != std::string::npos) {
        if (lower.find("where") != std::string::npos) return "location_query";
        if (lower.find("who") != std::string::npos) return "person_query";
        if (lower.find("what") != std::string::npos) return "info_query";
        if (lower.find("how") != std::string::npos) return "method_query";
        if (lower.find("why") != std::string::npos) return "reason_query";
        return "question";
    }
    
    if (lower.find("hello") != std::string::npos || 
        lower.find("hi") != std::string::npos) return "greeting";
    if (lower.find("bye") != std::string::npos || 
        lower.find("farewell") != std::string::npos) return "farewell";
    if (lower.find("buy") != std::string::npos) return "purchase";
    if (lower.find("sell") != std::string::npos) return "sale";
    if (lower.find("help") != std::string::npos) return "help_request";
    if (lower.find("quest") != std::string::npos || 
        lower.find("job") != std::string::npos) return "quest_inquiry";
    
    return "statement";
}

std::string TopicExtractor::detectSentiment(const std::string& text) {
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    int positive = 0, negative = 0;
    
    std::vector<std::string> positiveWords = {
        "good", "great", "wonderful", "excellent", "amazing",
        "happy", "glad", "pleased", "thank", "thanks",
        "love", "like", "enjoy", "appreciate", "help"
    };
    
    std::vector<std::string> negativeWords = {
        "bad", "terrible", "awful", "horrible", "hate",
        "angry", "sad", "upset", "annoyed", "frustrated",
        "stupid", "idiot", "fool", "damn", "hell"
    };
    
    for (const auto& word : positiveWords) {
        if (lower.find(word) != std::string::npos) positive++;
    }
    
    for (const auto& word : negativeWords) {
        if (lower.find(word) != std::string::npos) negative++;
    }
    
    if (positive > negative + 1) return "positive";
    if (negative > positive + 1) return "negative";
    return "neutral";
}

//=============================================================================
// ResponseTemplates Implementation
//=============================================================================

std::string ResponseTemplates::greeting(const std::string& npcName, const std::string& mood) {
    if (mood == "happy" || mood == "joy") {
        return "*smiling warmly* Greetings, traveler! I am " + npcName + ". What a fine day!";
    } else if (mood == "sad" || mood == "sadness") {
        return "*sighing* Ah, hello there. I'm " + npcName + ". Forgive my mood...";
    } else if (mood == "angry" || mood == "anger") {
        return "*scowling* What do you want? I'm " + npcName + ", and I'm busy.";
    } else if (mood == "afraid" || mood == "fear") {
        return "*nervously* Oh! You startled me. I'm " + npcName + "...";
    }
    return "Greetings, traveler. I am " + npcName + ". How may I help you?";
}

std::string ResponseTemplates::farewell(const std::string& npcName, const std::string& mood) {
    if (mood == "happy" || mood == "joy") {
        return "*waving cheerfully* Farewell, friend! May fortune smile upon you!";
    } else if (mood == "sad" || mood == "sadness") {
        return "*nodding slowly* Goodbye then. Take care of yourself...";
    }
    return "Farewell, traveler. Safe journeys.";
}

std::string ResponseTemplates::questOffer(const std::string& questName, const std::string& description) {
    return "I have a task that needs doing. " + description + " Will you help with '" + questName + "'?";
}

std::string ResponseTemplates::questAccepted(const std::string& questName) {
    return "Excellent! I knew I could count on you. Good luck with '" + questName + "'.";
}

std::string ResponseTemplates::questDeclined(const std::string& questName) {
    return "I understand. Perhaps another time, then.";
}

std::string ResponseTemplates::questComplete(const std::string& questName, const std::string& reward) {
    return "You've done it! '" + questName + "' is complete. Here is your reward: " + reward + ".";
}

std::string ResponseTemplates::shopGreeting(const std::string& shopType) {
    return "Welcome to my " + shopType + "! Take a look at my wares.";
}

std::string ResponseTemplates::buyConfirm(const std::string& item, int price) {
    return "The " + item + " will cost you " + std::to_string(price) + " gold. Deal?";
}

std::string ResponseTemplates::sellConfirm(const std::string& item, int price) {
    return "I'll give you " + std::to_string(price) + " gold for that " + item + ". Agreed?";
}

std::string ResponseTemplates::notEnoughGold(int required, int have) {
    return "I'm afraid you don't have enough gold. You need " + std::to_string(required) + 
           " but only have " + std::to_string(have) + ".";
}

std::string ResponseTemplates::locationInfo(const std::string& location, const std::string& description) {
    return location + "? " + description;
}

std::string ResponseTemplates::personInfo(const std::string& person, const std::string& description) {
    return "Ah, " + person + ". " + description;
}

std::string ResponseTemplates::rumor(const std::string& rumorText) {
    return "*leaning in* I've heard that " + rumorText;
}

std::string ResponseTemplates::angry(const std::string& reason) {
    return "*angrily* " + reason;
}

std::string ResponseTemplates::happy(const std::string& reason) {
    return "*beaming* " + reason;
}

std::string ResponseTemplates::sad(const std::string& reason) {
    return "*sadly* " + reason;
}

std::string ResponseTemplates::afraid(const std::string& reason) {
    return "*fearfully* " + reason;
}

std::string ResponseTemplates::surprised(const std::string& reason) {
    return "*eyes widening* " + reason;
}

} // namespace Dialogue
} // namespace NPC
} // namespace Ultima
