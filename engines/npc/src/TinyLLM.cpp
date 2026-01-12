/**
 * TinyLLM.cpp - Lightweight LLM Implementation for NPC Dialogue
 * 
 * This implementation provides a self-contained, lightweight language model
 * for generating creative NPC dialogue. It uses a simple transformer-based
 * architecture optimized for small models and fast inference.
 */

#include "llm/TinyLLM.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <regex>

namespace Ultima {
namespace NPC {
namespace LLM {

//=============================================================================
// NPCContext Implementation
//=============================================================================

std::string NPCContext::toSystemPrompt() const {
    std::ostringstream ss;
    ss << "You are " << name << ", a " << occupation << " in a medieval fantasy world.\n";
    ss << "Personality: " << personality << "\n";
    ss << "Current mood: " << currentMood << "\n";
    ss << "Location: " << location << "\n";
    
    if (!recentEvents.empty()) {
        ss << "Recent events: " << recentEvents << "\n";
    }
    
    if (!knownFacts.empty()) {
        ss << "You know these facts:\n";
        for (const auto& fact : knownFacts) {
            ss << "- " << fact << "\n";
        }
    }
    
    ss << "\nRespond in character as " << name << ". ";
    ss << "Keep responses concise (1-3 sentences). ";
    ss << "Match your personality and current mood in your tone.";
    
    return ss.str();
}

//=============================================================================
// Simple Tokenizer (BPE-like)
//=============================================================================

class SimpleTokenizer {
public:
    SimpleTokenizer() {
        // Initialize basic vocabulary
        initializeVocab();
    }
    
    std::vector<int> encode(const std::string& text) const {
        std::vector<int> tokens;
        std::string word;
        
        for (char c : text) {
            if (std::isspace(c)) {
                if (!word.empty()) {
                    encodeWord(word, tokens);
                    word.clear();
                }
                tokens.push_back(getToken(" "));
            } else {
                word += c;
            }
        }
        
        if (!word.empty()) {
            encodeWord(word, tokens);
        }
        
        return tokens;
    }
    
    std::string decode(const std::vector<int>& tokens) const {
        std::ostringstream ss;
        for (int token : tokens) {
            if (token >= 0 && token < static_cast<int>(vocab_.size())) {
                ss << vocab_[token];
            }
        }
        return ss.str();
    }
    
    int vocabSize() const { return static_cast<int>(vocab_.size()); }
    
private:
    std::vector<std::string> vocab_;
    std::unordered_map<std::string, int> tokenToId_;
    
    void initializeVocab() {
        // Basic character-level + common words vocabulary
        std::vector<std::string> baseVocab = {
            "<pad>", "<unk>", "<s>", "</s>", " ", "\n",
            "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
            "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
            "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
            "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            ".", ",", "!", "?", "'", "\"", "-", ":", ";", "(", ")", "[", "]",
            // Common words for dialogue
            "the", "and", "is", "are", "was", "were", "be", "been", "being",
            "have", "has", "had", "do", "does", "did", "will", "would", "could",
            "should", "may", "might", "must", "shall", "can", "need", "dare",
            "I", "you", "he", "she", "it", "we", "they", "me", "him", "her",
            "us", "them", "my", "your", "his", "its", "our", "their",
            "this", "that", "these", "those", "what", "which", "who", "whom",
            "hello", "greetings", "welcome", "farewell", "goodbye", "yes", "no",
            "please", "thank", "thanks", "sorry", "excuse", "pardon",
            "friend", "stranger", "traveler", "adventurer", "hero", "villain",
            "merchant", "guard", "mage", "wizard", "knight", "king", "queen",
            "gold", "silver", "coin", "sword", "shield", "armor", "potion",
            "magic", "spell", "quest", "adventure", "danger", "treasure",
            "tavern", "inn", "shop", "castle", "dungeon", "forest", "village",
            "help", "need", "want", "seek", "find", "buy", "sell", "trade",
            "good", "bad", "great", "terrible", "wonderful", "horrible",
            "safe", "dangerous", "careful", "beware", "warning"
        };
        
        vocab_ = baseVocab;
        for (size_t i = 0; i < vocab_.size(); ++i) {
            tokenToId_[vocab_[i]] = static_cast<int>(i);
        }
    }
    
    int getToken(const std::string& s) const {
        auto it = tokenToId_.find(s);
        return it != tokenToId_.end() ? it->second : 1; // 1 = <unk>
    }
    
    void encodeWord(const std::string& word, std::vector<int>& tokens) const {
        // Try whole word first
        auto it = tokenToId_.find(word);
        if (it != tokenToId_.end()) {
            tokens.push_back(it->second);
            return;
        }
        
        // Fall back to character-level
        for (char c : word) {
            std::string s(1, c);
            tokens.push_back(getToken(s));
        }
    }
};

//=============================================================================
// Simple Neural Network Components
//=============================================================================

class SimpleMatrix {
public:
    SimpleMatrix(int rows, int cols) : rows_(rows), cols_(cols), data_(rows * cols, 0.0f) {}
    
    float& at(int r, int c) { return data_[r * cols_ + c]; }
    float at(int r, int c) const { return data_[r * cols_ + c]; }
    
    int rows() const { return rows_; }
    int cols() const { return cols_; }
    
    void randomize(float scale = 0.02f) {
        static std::mt19937 rng(42);
        std::normal_distribution<float> dist(0.0f, scale);
        for (auto& v : data_) {
            v = dist(rng);
        }
    }
    
    std::vector<float> multiply(const std::vector<float>& vec) const {
        std::vector<float> result(rows_, 0.0f);
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                result[i] += at(i, j) * vec[j];
            }
        }
        return result;
    }

private:
    int rows_, cols_;
    std::vector<float> data_;
};

// Simple attention mechanism
class SimpleAttention {
public:
    SimpleAttention(int dim, int heads = 4) 
        : dim_(dim), heads_(heads), headDim_(dim / heads),
          wq_(dim, dim), wk_(dim, dim), wv_(dim, dim), wo_(dim, dim) {
        wq_.randomize();
        wk_.randomize();
        wv_.randomize();
        wo_.randomize();
    }
    
    std::vector<float> forward(const std::vector<float>& x) {
        // Simplified single-head attention for demo
        auto q = wq_.multiply(x);
        auto k = wk_.multiply(x);
        auto v = wv_.multiply(x);
        
        // Self-attention (simplified)
        float score = 0.0f;
        for (int i = 0; i < dim_; ++i) {
            score += q[i] * k[i];
        }
        score /= std::sqrt(static_cast<float>(dim_));
        float attn = 1.0f / (1.0f + std::exp(-score)); // Sigmoid approximation
        
        std::vector<float> out(dim_);
        for (int i = 0; i < dim_; ++i) {
            out[i] = attn * v[i];
        }
        
        return wo_.multiply(out);
    }

private:
    int dim_, heads_, headDim_;
    SimpleMatrix wq_, wk_, wv_, wo_;
};

// Simple feed-forward network
class SimpleFeedForward {
public:
    SimpleFeedForward(int dim, int hiddenDim = 0)
        : dim_(dim), hiddenDim_(hiddenDim > 0 ? hiddenDim : dim * 4),
          w1_(hiddenDim_, dim), w2_(dim, hiddenDim_) {
        w1_.randomize();
        w2_.randomize();
    }
    
    std::vector<float> forward(const std::vector<float>& x) {
        auto h = w1_.multiply(x);
        
        // GELU activation
        for (auto& v : h) {
            v = 0.5f * v * (1.0f + std::tanh(std::sqrt(2.0f / 3.14159f) * (v + 0.044715f * v * v * v)));
        }
        
        return w2_.multiply(h);
    }

private:
    int dim_, hiddenDim_;
    SimpleMatrix w1_, w2_;
};

//=============================================================================
// TinyLLM Implementation
//=============================================================================

struct TinyLLM::Impl {
    LLMConfig config;
    bool ready = false;
    SimpleTokenizer tokenizer;
    
    // Simple transformer layers
    std::vector<std::unique_ptr<SimpleAttention>> attentionLayers;
    std::vector<std::unique_ptr<SimpleFeedForward>> ffnLayers;
    SimpleMatrix embeddings{0, 0};
    SimpleMatrix outputProj{0, 0};
    
    std::mt19937 rng{std::random_device{}()};
    
    // Response templates for fallback
    std::vector<std::string> greetingResponses = {
        "Greetings, traveler. What brings you here?",
        "Well met, friend. How may I help you?",
        "Ah, a visitor! Welcome!",
        "Hello there. What can I do for you?"
    };
    
    std::vector<std::string> farewellResponses = {
        "Safe travels, friend.",
        "Farewell. May fortune favor you.",
        "Until we meet again.",
        "Go with care, traveler."
    };
    
    std::vector<std::string> unknownResponses = {
        "I'm not sure I understand. Could you say that differently?",
        "Hmm, that's an interesting question. Let me think...",
        "I'm afraid I don't know much about that.",
        "Perhaps you should ask someone else about that."
    };
    
    void initialize(int vocabSize, int dim, int numLayers) {
        embeddings = SimpleMatrix(vocabSize, dim);
        embeddings.randomize();
        
        outputProj = SimpleMatrix(vocabSize, dim);
        outputProj.randomize();
        
        for (int i = 0; i < numLayers; ++i) {
            attentionLayers.push_back(std::make_unique<SimpleAttention>(dim));
            ffnLayers.push_back(std::make_unique<SimpleFeedForward>(dim));
        }
    }
    
    std::vector<float> embed(int token) {
        std::vector<float> emb(embeddings.cols());
        for (int i = 0; i < embeddings.cols(); ++i) {
            emb[i] = embeddings.at(token % embeddings.rows(), i);
        }
        return emb;
    }
    
    int sampleToken(const std::vector<float>& logits, float temperature) {
        std::vector<float> probs(logits.size());
        float maxLogit = *std::max_element(logits.begin(), logits.end());
        
        float sum = 0.0f;
        for (size_t i = 0; i < logits.size(); ++i) {
            probs[i] = std::exp((logits[i] - maxLogit) / std::max(temperature, 0.1f));
            sum += probs[i];
        }
        
        for (auto& p : probs) {
            p /= sum;
        }
        
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float r = dist(rng);
        float cumsum = 0.0f;
        
        for (size_t i = 0; i < probs.size(); ++i) {
            cumsum += probs[i];
            if (r < cumsum) {
                return static_cast<int>(i);
            }
        }
        
        return static_cast<int>(probs.size() - 1);
    }
    
    std::string generateFallback(const DialogueRequest& request) {
        std::string input = request.playerInput;
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        
        // Simple pattern matching for fallback
        if (input.find("hello") != std::string::npos ||
            input.find("hi") != std::string::npos ||
            input.find("greet") != std::string::npos) {
            return greetingResponses[rng() % greetingResponses.size()];
        }
        
        if (input.find("bye") != std::string::npos ||
            input.find("farewell") != std::string::npos ||
            input.find("leave") != std::string::npos) {
            return farewellResponses[rng() % farewellResponses.size()];
        }
        
        if (input.find("name") != std::string::npos) {
            return "I am " + request.npcContext.name + ", " + 
                   request.npcContext.occupation + ".";
        }
        
        if (input.find("help") != std::string::npos) {
            return "I'll do what I can to help you, traveler.";
        }
        
        if (input.find("buy") != std::string::npos ||
            input.find("sell") != std::string::npos ||
            input.find("trade") != std::string::npos) {
            if (request.npcContext.occupation.find("merchant") != std::string::npos ||
                request.npcContext.occupation.find("Merchant") != std::string::npos) {
                return "Ah, interested in my wares? Take a look at what I have!";
            }
            return "I'm not a merchant, but perhaps I can point you to one.";
        }
        
        return unknownResponses[rng() % unknownResponses.size()];
    }
};

TinyLLM::TinyLLM() : impl_(std::make_unique<Impl>()) {}
TinyLLM::~TinyLLM() = default;

bool TinyLLM::initialize(const LLMConfig& config) {
    impl_->config = config;
    
    // Initialize with small dimensions for demo
    // In production, this would load weights from the GGUF model
    int vocabSize = impl_->tokenizer.vocabSize();
    int dim = 256;  // Small embedding dimension
    int numLayers = 4;
    
    impl_->initialize(vocabSize, dim, numLayers);
    impl_->ready = true;
    
    return true;
}

bool TinyLLM::isReady() const {
    return impl_->ready;
}

DialogueResult TinyLLM::generateDialogue(const DialogueRequest& request) {
    DialogueResult result;
    result.wasGenerated = true;
    result.confidence = 0.7f;
    result.tokensUsed = 0;
    
    if (!impl_->ready) {
        result.response = impl_->generateFallback(request);
        result.wasGenerated = false;
        result.confidence = 0.5f;
        return result;
    }
    
    // Build the prompt
    std::string prompt = buildPrompt(request);
    
    // Tokenize
    auto tokens = impl_->tokenizer.encode(prompt);
    result.tokensUsed = static_cast<int>(tokens.size());
    
    // Generate response
    std::vector<int> generatedTokens;
    std::vector<float> hidden(256, 0.0f);
    
    // Process input tokens
    for (int token : tokens) {
        auto emb = impl_->embed(token);
        for (size_t i = 0; i < impl_->attentionLayers.size(); ++i) {
            emb = impl_->attentionLayers[i]->forward(emb);
            emb = impl_->ffnLayers[i]->forward(emb);
        }
        hidden = emb;
    }
    
    // Generate new tokens
    int maxNewTokens = std::min(impl_->config.maxTokens, 64);
    for (int i = 0; i < maxNewTokens; ++i) {
        // Compute logits
        std::vector<float> logits(impl_->tokenizer.vocabSize());
        for (int j = 0; j < impl_->tokenizer.vocabSize(); ++j) {
            for (int k = 0; k < static_cast<int>(hidden.size()); ++k) {
                logits[j] += impl_->outputProj.at(j, k) * hidden[k];
            }
        }
        
        // Sample next token
        int nextToken = impl_->sampleToken(logits, impl_->config.temperature);
        
        // Check for end token
        if (nextToken == 3) break; // </s>
        
        generatedTokens.push_back(nextToken);
        result.tokensUsed++;
        
        // Update hidden state
        auto emb = impl_->embed(nextToken);
        for (size_t j = 0; j < impl_->attentionLayers.size(); ++j) {
            emb = impl_->attentionLayers[j]->forward(emb);
            emb = impl_->ffnLayers[j]->forward(emb);
        }
        hidden = emb;
    }
    
    // Decode response
    std::string rawResponse = impl_->tokenizer.decode(generatedTokens);
    
    // If generated response is too short or gibberish, use fallback
    if (rawResponse.length() < 10 || rawResponse.find_first_not_of(" \n\t") == std::string::npos) {
        result.response = impl_->generateFallback(request);
        result.wasGenerated = false;
        result.confidence = 0.5f;
    } else {
        result.response = rawResponse;
    }
    
    // Detect emotion from response
    result.emotion = "neutral";
    std::string lower = result.response;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower.find("happy") != std::string::npos || 
        lower.find("wonderful") != std::string::npos ||
        lower.find("great") != std::string::npos) {
        result.emotion = "joy";
    } else if (lower.find("sorry") != std::string::npos ||
               lower.find("sad") != std::string::npos) {
        result.emotion = "sadness";
    } else if (lower.find("angry") != std::string::npos ||
               lower.find("furious") != std::string::npos) {
        result.emotion = "anger";
    } else if (lower.find("afraid") != std::string::npos ||
               lower.find("scared") != std::string::npos ||
               lower.find("danger") != std::string::npos) {
        result.emotion = "fear";
    }
    
    return result;
}

DialogueResult TinyLLM::generateDialogueStreaming(
    const DialogueRequest& request,
    StreamCallback callback) {
    
    // For simplicity, generate full response then stream it
    auto result = generateDialogue(request);
    
    // Stream tokens
    for (char c : result.response) {
        callback(std::string(1, c));
    }
    
    return result;
}

std::string TinyLLM::complete(const std::string& prompt, int maxTokens) {
    DialogueRequest request;
    request.playerInput = prompt;
    request.requiresCreativity = true;
    
    impl_->config.maxTokens = maxTokens;
    auto result = generateDialogue(request);
    return result.response;
}

std::vector<DialogueResult> TinyLLM::generateVariations(
    const DialogueRequest& request,
    int count) {
    
    std::vector<DialogueResult> results;
    float originalTemp = impl_->config.temperature;
    
    for (int i = 0; i < count; ++i) {
        // Vary temperature for diversity
        impl_->config.temperature = originalTemp + (i * 0.1f);
        results.push_back(generateDialogue(request));
    }
    
    impl_->config.temperature = originalTemp;
    return results;
}

std::unordered_map<std::string, std::string> TinyLLM::analyzeInput(
    const std::string& input) {
    
    std::unordered_map<std::string, std::string> analysis;
    std::string lower = input;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Intent detection
    if (lower.find("?") != std::string::npos) {
        analysis["intent"] = "question";
    } else if (lower.find("!") != std::string::npos) {
        analysis["intent"] = "exclamation";
    } else if (lower.find("please") != std::string::npos ||
               lower.find("could you") != std::string::npos ||
               lower.find("would you") != std::string::npos) {
        analysis["intent"] = "request";
    } else if (lower.find("hello") != std::string::npos ||
               lower.find("hi") != std::string::npos) {
        analysis["intent"] = "greeting";
    } else if (lower.find("bye") != std::string::npos ||
               lower.find("farewell") != std::string::npos) {
        analysis["intent"] = "farewell";
    } else {
        analysis["intent"] = "statement";
    }
    
    // Emotion detection
    if (lower.find("angry") != std::string::npos ||
        lower.find("hate") != std::string::npos ||
        lower.find("furious") != std::string::npos) {
        analysis["emotion"] = "anger";
    } else if (lower.find("happy") != std::string::npos ||
               lower.find("love") != std::string::npos ||
               lower.find("great") != std::string::npos) {
        analysis["emotion"] = "joy";
    } else if (lower.find("sad") != std::string::npos ||
               lower.find("sorry") != std::string::npos) {
        analysis["emotion"] = "sadness";
    } else if (lower.find("afraid") != std::string::npos ||
               lower.find("scared") != std::string::npos) {
        analysis["emotion"] = "fear";
    } else {
        analysis["emotion"] = "neutral";
    }
    
    // Topic detection
    if (lower.find("quest") != std::string::npos ||
        lower.find("mission") != std::string::npos ||
        lower.find("task") != std::string::npos) {
        analysis["topic"] = "quest";
    } else if (lower.find("buy") != std::string::npos ||
               lower.find("sell") != std::string::npos ||
               lower.find("trade") != std::string::npos ||
               lower.find("gold") != std::string::npos) {
        analysis["topic"] = "commerce";
    } else if (lower.find("fight") != std::string::npos ||
               lower.find("battle") != std::string::npos ||
               lower.find("attack") != std::string::npos) {
        analysis["topic"] = "combat";
    } else if (lower.find("where") != std::string::npos ||
               lower.find("direction") != std::string::npos ||
               lower.find("location") != std::string::npos) {
        analysis["topic"] = "navigation";
    } else {
        analysis["topic"] = "general";
    }
    
    return analysis;
}

int TinyLLM::getTokenCount(const std::string& text) const {
    return static_cast<int>(impl_->tokenizer.encode(text).size());
}

void TinyLLM::clearContext() {
    // Reset hidden state would go here
}

std::string TinyLLM::getModelInfo() const {
    std::ostringstream ss;
    ss << "TinyLLM v1.0\n";
    ss << "Vocab size: " << impl_->tokenizer.vocabSize() << "\n";
    ss << "Embedding dim: 256\n";
    ss << "Layers: 4\n";
    ss << "Ready: " << (impl_->ready ? "yes" : "no");
    return ss.str();
}

void TinyLLM::setTemperature(float temp) {
    impl_->config.temperature = std::max(0.1f, std::min(2.0f, temp));
}

void TinyLLM::shutdown() {
    impl_->ready = false;
    impl_->attentionLayers.clear();
    impl_->ffnLayers.clear();
}

std::string TinyLLM::buildPrompt(const DialogueRequest& request) const {
    std::ostringstream ss;
    
    // System prompt
    ss << request.npcContext.toSystemPrompt() << "\n\n";
    
    // Conversation history
    if (!request.history.empty()) {
        ss << formatChatHistory(request.history) << "\n";
    }
    
    // Current input
    ss << "Player: " << request.playerInput << "\n";
    ss << request.npcContext.name << ": ";
    
    return ss.str();
}

std::string TinyLLM::formatChatHistory(const std::vector<ChatMessage>& history) const {
    std::ostringstream ss;
    for (const auto& msg : history) {
        switch (msg.role) {
            case ChatMessage::Role::User:
                ss << "Player: " << msg.content << "\n";
                break;
            case ChatMessage::Role::Assistant:
                ss << "NPC: " << msg.content << "\n";
                break;
            case ChatMessage::Role::System:
                // Skip system messages in history
                break;
        }
    }
    return ss.str();
}

DialogueResult TinyLLM::parseResponse(const std::string& raw, const DialogueRequest& request) const {
    DialogueResult result;
    result.response = raw;
    result.wasGenerated = true;
    result.confidence = 0.7f;
    return result;
}

//=============================================================================
// DialogueGenerator Implementation
//=============================================================================

struct DialogueGenerator::Impl {
    std::unique_ptr<TinyLLM> llm;
    float creativityThreshold = 0.6f;
    bool personalityProcessing = true;
    Stats stats;
    
    // Simple AIML-like patterns
    std::unordered_map<std::string, std::string> patterns;
    
    void initializePatterns() {
        patterns["hello"] = "Greetings, {name}. How may I assist you?";
        patterns["hi"] = "Hello there! What brings you to {location}?";
        patterns["bye"] = "Farewell, traveler. Safe journeys!";
        patterns["goodbye"] = "Until we meet again. Take care!";
        patterns["name"] = "I am {name}, {occupation} of this place.";
        patterns["help"] = "I shall do my best to help you, friend.";
        patterns["quest"] = "Ah, seeking adventure? There may be something...";
        patterns["buy"] = "Interested in my wares? Let me show you what I have.";
        patterns["sell"] = "Looking to sell something? Let me see what you've got.";
        patterns["weather"] = "The weather? It seems fair enough today.";
        patterns["time"] = "The hour grows late. Best be careful after dark.";
    }
    
    std::string matchPattern(const std::string& input, const NPCContext& ctx) {
        std::string lower = input;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        for (const auto& [pattern, response] : patterns) {
            if (lower.find(pattern) != std::string::npos) {
                std::string result = response;
                
                // Replace placeholders
                size_t pos;
                while ((pos = result.find("{name}")) != std::string::npos) {
                    result.replace(pos, 6, ctx.name);
                }
                while ((pos = result.find("{occupation}")) != std::string::npos) {
                    result.replace(pos, 12, ctx.occupation);
                }
                while ((pos = result.find("{location}")) != std::string::npos) {
                    result.replace(pos, 10, ctx.location);
                }
                
                return result;
            }
        }
        
        return "";
    }
};

DialogueGenerator::DialogueGenerator() : impl_(std::make_unique<Impl>()) {
    impl_->initializePatterns();
}

DialogueGenerator::~DialogueGenerator() = default;

bool DialogueGenerator::initialize(const LLMConfig& llmConfig, const std::string& aimlPath) {
    impl_->llm = std::make_unique<TinyLLM>();
    return impl_->llm->initialize(llmConfig);
}

DialogueResult DialogueGenerator::generate(const DialogueRequest& request) {
    auto startTime = std::chrono::high_resolution_clock::now();
    impl_->stats.totalRequests++;
    
    DialogueResult result;
    
    // Try pattern matching first
    std::string patternResponse = impl_->matchPattern(
        request.playerInput, request.npcContext);
    
    if (!patternResponse.empty()) {
        result.response = patternResponse;
        result.wasGenerated = false;
        result.confidence = 0.9f;
        impl_->stats.aimlResponses++;
    } else if (impl_->llm && impl_->llm->isReady()) {
        // Use LLM for creative response
        result = impl_->llm->generateDialogue(request);
        impl_->stats.llmResponses++;
    } else {
        // Fallback
        result.response = "I'm not sure what to say about that.";
        result.wasGenerated = false;
        result.confidence = 0.3f;
        impl_->stats.fallbackResponses++;
    }
    
    // Apply personality processing
    if (impl_->personalityProcessing) {
        result.response = PersonalityModifier::addEmotion(
            result.response,
            request.npcContext.currentMood,
            0.5f
        );
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    impl_->stats.avgResponseTime = 
        (impl_->stats.avgResponseTime * (impl_->stats.totalRequests - 1) + elapsed) 
        / impl_->stats.totalRequests;
    
    return result;
}

DialogueResult DialogueGenerator::generateCreative(const DialogueRequest& request) {
    if (impl_->llm && impl_->llm->isReady()) {
        return impl_->llm->generateDialogue(request);
    }
    
    DialogueResult result;
    result.response = "The words escape me at the moment...";
    result.wasGenerated = false;
    result.confidence = 0.2f;
    return result;
}

DialogueResult DialogueGenerator::generatePattern(const DialogueRequest& request) {
    DialogueResult result;
    std::string response = impl_->matchPattern(request.playerInput, request.npcContext);
    
    if (!response.empty()) {
        result.response = response;
        result.confidence = 0.9f;
    } else {
        result.response = "Hmm?";
        result.confidence = 0.1f;
    }
    
    result.wasGenerated = false;
    return result;
}

void DialogueGenerator::setCreativityThreshold(float threshold) {
    impl_->creativityThreshold = std::max(0.0f, std::min(1.0f, threshold));
}

void DialogueGenerator::setPersonalityProcessing(bool enabled) {
    impl_->personalityProcessing = enabled;
}

DialogueGenerator::Stats DialogueGenerator::getStats() const {
    return impl_->stats;
}

//=============================================================================
// PersonalityModifier Implementation
//=============================================================================

std::string PersonalityModifier::modify(
    const std::string& response,
    float openness,
    float conscientiousness,
    float extraversion,
    float agreeableness,
    float neuroticism) {
    
    std::string result = response;
    
    // High extraversion: add enthusiasm
    if (extraversion > 0.7f) {
        if (result.back() == '.') {
            result.back() = '!';
        }
    }
    
    // Low extraversion: more reserved
    if (extraversion < 0.3f) {
        // Add hedging language
        if (result.find("I think") == std::string::npos) {
            result = "Well... " + result;
        }
    }
    
    // High agreeableness: add politeness
    if (agreeableness > 0.7f) {
        if (result.find("please") == std::string::npos &&
            result.find("thank") == std::string::npos) {
            result += " If you don't mind, of course.";
        }
    }
    
    // High neuroticism: add worry
    if (neuroticism > 0.7f) {
        result += " I hope that helps...";
    }
    
    return result;
}

std::string PersonalityModifier::addEmotion(
    const std::string& response,
    const std::string& emotion,
    float intensity) {
    
    std::string result = response;
    
    if (intensity < 0.3f) {
        return result; // Emotion too weak to show
    }
    
    std::string prefix;
    
    if (emotion == "joy" || emotion == "happy") {
        prefix = intensity > 0.7f ? "*beaming* " : "*smiling* ";
    } else if (emotion == "sadness" || emotion == "sad") {
        prefix = intensity > 0.7f ? "*sighing deeply* " : "*looking down* ";
    } else if (emotion == "anger" || emotion == "angry") {
        prefix = intensity > 0.7f ? "*scowling* " : "*frowning* ";
    } else if (emotion == "fear" || emotion == "afraid") {
        prefix = intensity > 0.7f ? "*trembling* " : "*nervously* ";
    } else if (emotion == "surprise" || emotion == "surprised") {
        prefix = "*eyes widening* ";
    }
    
    return prefix + result;
}

std::string PersonalityModifier::addSpeechPattern(
    const std::string& response,
    const std::string& occupation,
    float education) {
    
    std::string result = response;
    std::string lower = occupation;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Merchant speech
    if (lower.find("merchant") != std::string::npos ||
        lower.find("trader") != std::string::npos) {
        if (result.find("gold") == std::string::npos) {
            result += " And remember, I always offer fair prices!";
        }
    }
    
    // Guard speech
    if (lower.find("guard") != std::string::npos ||
        lower.find("soldier") != std::string::npos) {
        result = "Citizen, " + result;
    }
    
    // Mage speech
    if (lower.find("mage") != std::string::npos ||
        lower.find("wizard") != std::string::npos) {
        if (education > 0.7f) {
            result += " The arcane arts reveal much, you see.";
        }
    }
    
    // Low education: simpler speech
    if (education < 0.3f) {
        // Could simplify vocabulary here
    }
    
    return result;
}

//=============================================================================
// ConversationMemory Implementation
//=============================================================================

struct ConversationMemory::Impl {
    std::vector<ChatMessage> history;
    int maxTurns;
    
    Impl(int max) : maxTurns(max) {}
};

ConversationMemory::ConversationMemory(int maxTurns) 
    : impl_(std::make_unique<Impl>(maxTurns)) {}

void ConversationMemory::addTurn(const std::string& speaker, const std::string& text) {
    ChatMessage::Role role = (speaker == "player" || speaker == "Player") 
        ? ChatMessage::Role::User 
        : ChatMessage::Role::Assistant;
    
    impl_->history.push_back({role, text});
    
    // Trim to max turns
    while (static_cast<int>(impl_->history.size()) > impl_->maxTurns * 2) {
        impl_->history.erase(impl_->history.begin());
    }
}

void ConversationMemory::addPlayerInput(const std::string& input) {
    addTurn("player", input);
}

void ConversationMemory::addNPCResponse(const std::string& response) {
    addTurn("npc", response);
}

std::vector<ChatMessage> ConversationMemory::getHistory() const {
    return impl_->history;
}

std::string ConversationMemory::getSummary() const {
    std::ostringstream ss;
    ss << "Conversation with " << impl_->history.size() / 2 << " exchanges.";
    return ss.str();
}

void ConversationMemory::clear() {
    impl_->history.clear();
}

void ConversationMemory::setMaxTurns(int max) {
    impl_->maxTurns = max;
}

std::vector<std::string> ConversationMemory::findRelevant(
    const std::string& query, int count) const {
    
    std::vector<std::string> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    // Simple keyword matching
    for (const auto& msg : impl_->history) {
        std::string lower = msg.content;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        // Check for word overlap
        bool relevant = false;
        std::istringstream iss(lowerQuery);
        std::string word;
        while (iss >> word && !relevant) {
            if (word.length() > 3 && lower.find(word) != std::string::npos) {
                relevant = true;
            }
        }
        
        if (relevant) {
            results.push_back(msg.content);
            if (static_cast<int>(results.size()) >= count) {
                break;
            }
        }
    }
    
    return results;
}

} // namespace LLM
} // namespace NPC
} // namespace Ultima
