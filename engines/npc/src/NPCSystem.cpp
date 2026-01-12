/**
 * NPCSystem.cpp - Unified NPC AI System Implementation
 */

#include "NPCSystem.h"
#include <algorithm>
#include <random>

namespace Ultima {
namespace NPC {

//=============================================================================
// NPCEntity Implementation
//=============================================================================

NPCEntity::NPCEntity(const std::string& id) 
    : id_(id)
    , persona_(std::make_unique<Persona::NPCPersona>(id))
    , memory_(std::make_unique<Memory::MemorySystem>())
    , learning_(std::make_unique<Neural::NPCLearningNetwork>(Neural::NPCLearningNetwork::LearningContext{}))
    , economicAgent_(std::make_unique<Economy::EconomicAgent>(id))
    , dialogueContext_(std::make_unique<AIML::SessionContext>())
{
    persona_->id = id;
}

NPCEntity::~NPCEntity() = default;

void NPCEntity::update(double deltaTime) {
    // Update emotional state decay
    auto& emotions = persona_->emotionalState;
    emotions.arousal *= (1.0 - 0.1 * deltaTime);  // Arousal decays over time
    
    // Mood tends toward neutral
    emotions.overallValence *= (1.0 - 0.05 * deltaTime);
}

std::string NPCEntity::respondToDialogue(const std::string& input) {
    // Simple response based on personality
    static std::mt19937 rng(std::random_device{}());
    
    std::vector<std::string> greetings = {
        "Hello there, traveler!",
        "Greetings, friend.",
        "Well met!",
        "Good day to you."
    };
    
    std::vector<std::string> sellResponses = {
        "I have many fine wares for sale.",
        "Take a look at my goods!",
        "I deal in quality merchandise.",
        "What catches your eye?"
    };
    
    std::vector<std::string> unknownResponses = {
        "I'm not sure I understand.",
        "Could you say that again?",
        "Hmm, interesting question.",
        "Let me think about that."
    };
    
    // Simple keyword matching
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    
    std::vector<std::string>* responses = &unknownResponses;
    
    if (lowerInput.find("hello") != std::string::npos || 
        lowerInput.find("hi") != std::string::npos ||
        lowerInput.find("greet") != std::string::npos) {
        responses = &greetings;
    } else if (lowerInput.find("sell") != std::string::npos ||
               lowerInput.find("buy") != std::string::npos ||
               lowerInput.find("wares") != std::string::npos) {
        responses = &sellResponses;
    }
    
    // Modify response based on extraversion
    std::uniform_int_distribution<> dist(0, responses->size() - 1);
    std::string response = (*responses)[dist(rng)];
    
    // Add personality flavor
    if (persona_->traits.extraversion > 0.7) {
        response = "Oh! " + response + " It's wonderful to see you!";
    } else if (persona_->traits.extraversion < 0.3) {
        response = "*quietly* " + response;
    }
    
    return response;
}

NPCEntity::Decision NPCEntity::makeDecision(const std::vector<std::string>& options) {
    Decision decision;
    
    if (options.empty()) {
        decision.action = "wait";
        decision.confidence = 1.0;
        decision.reasoning = "No options available";
        return decision;
    }
    
    // Score each option based on personality
    std::vector<double> scores(options.size(), 0.5);
    
    for (size_t i = 0; i < options.size(); ++i) {
        std::string opt = options[i];
        std::transform(opt.begin(), opt.end(), opt.begin(), ::tolower);
        
        // Conscientiousness favors duty-related actions
        if (opt.find("patrol") != std::string::npos || 
            opt.find("report") != std::string::npos ||
            opt.find("continue") != std::string::npos) {
            scores[i] += persona_->traits.conscientiousness * 0.3;
        }
        
        // Openness favors investigation
        if (opt.find("investigate") != std::string::npos ||
            opt.find("explore") != std::string::npos) {
            scores[i] += persona_->traits.openness * 0.3;
        }
        
        // Low conscientiousness favors breaks
        if (opt.find("break") != std::string::npos ||
            opt.find("rest") != std::string::npos) {
            scores[i] += (1.0 - persona_->traits.conscientiousness) * 0.3;
        }
        
        // Neuroticism affects caution
        if (opt.find("careful") != std::string::npos ||
            opt.find("safe") != std::string::npos) {
            scores[i] += persona_->traits.neuroticism * 0.2;
        }
    }
    
    // Find best option
    size_t bestIdx = 0;
    double bestScore = scores[0];
    for (size_t i = 1; i < scores.size(); ++i) {
        if (scores[i] > bestScore) {
            bestScore = scores[i];
            bestIdx = i;
        }
    }
    
    decision.action = options[bestIdx];
    decision.confidence = bestScore;
    decision.reasoning = "Based on personality traits and current situation";
    
    return decision;
}

std::string NPCEntity::serialize() const {
    return "{}";  // Placeholder
}

bool NPCEntity::deserialize(const std::string& data) {
    (void)data;
    return true;  // Placeholder
}

//=============================================================================
// NPCManager Implementation
//=============================================================================

NPCManager::NPCManager()
    : dialogueManager_(std::make_unique<AIML::NPCDialogueManager>())
    , relationshipSystem_(std::make_unique<Social::RelationshipSystem>())
    , economicSystem_(std::make_unique<Economy::EconomicSystem>())
{
}

NPCManager::~NPCManager() = default;

bool NPCManager::initialize(const std::string& dataDirectory) {
    (void)dataDirectory;
    relationshipSystem_->initialize();
    return true;
}

NPCEntity* NPCManager::createNPC(const std::string& id, const std::string& templateId) {
    (void)templateId;
    
    auto npc = std::make_unique<NPCEntity>(id);
    NPCEntity* ptr = npc.get();
    npcs_[id] = std::move(npc);
    
    // Register with relationship system
    relationshipSystem_->registerEntity(id);
    
    return ptr;
}

NPCEntity* NPCManager::getNPC(const std::string& id) {
    auto it = npcs_.find(id);
    if (it != npcs_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void NPCManager::removeNPC(const std::string& id) {
    npcs_.erase(id);
}

std::vector<std::string> NPCManager::getAllNPCIds() const {
    std::vector<std::string> ids;
    ids.reserve(npcs_.size());
    for (const auto& pair : npcs_) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<NPCEntity*> NPCManager::getNPCsAtLocation(const std::string& location) {
    std::vector<NPCEntity*> result;
    for (auto& pair : npcs_) {
        if (pair.second->getCurrentLocation() == location) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

void NPCManager::update(double deltaTime) {
    for (auto& pair : npcs_) {
        pair.second->update(deltaTime);
    }
}

void NPCManager::processInteraction(const std::string& npcA, const std::string& npcB,
                                    Social::InteractionType type, double intensity) {
    relationshipSystem_->processInteraction(npcA, npcB, type, intensity);
}

} // namespace NPC
} // namespace Ultima
