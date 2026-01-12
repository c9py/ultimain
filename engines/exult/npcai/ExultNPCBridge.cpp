/**
 * ExultNPCBridge.cpp - Implementation of the Exult-NPC AI bridge
 */

#include "ExultNPCBridge.h"
#include "../../npc/include/NPCSystem.h"
#include "../../npc/include/aiml/HybridDialogue.h"
#include "../../npc/include/llm/TinyLLM.h"
#include "../../npc/include/persona/Persona.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace Ultima {
namespace Exult {

// Singleton instance
ExultNPCBridge& ExultNPCBridge::getInstance() {
    static ExultNPCBridge instance;
    return instance;
}

ExultNPCBridge::ExultNPCBridge() {
}

ExultNPCBridge::~ExultNPCBridge() {
    shutdown();
}

bool ExultNPCBridge::initialize(const std::string& configPath) {
    if (initialized_) {
        return true;
    }
    
    try {
        // Initialize the dialogue engine
        dialogueEngine_ = std::make_unique<NPC::Dialogue::HybridDialogueEngine>();
        
        // Initialize with default config
        NPC::Dialogue::HybridConfig config;
        dialogueEngine_->initialize(config, "");
        
        // Set default paths
        if (aimlPatternsPath_.empty()) {
            aimlPatternsPath_ = "data/npcai/aiml/";
        }
        
        initialized_ = true;
        std::cout << "[ExultNPCBridge] Initialized successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ExultNPCBridge] Initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void ExultNPCBridge::shutdown() {
    if (!initialized_) {
        return;
    }
    
    cognitiveNPCs_.clear();
    actorToId_.clear();
    activeConversations_.clear();
    dialogueEngine_.reset();
    
    initialized_ = false;
    std::cout << "[ExultNPCBridge] Shutdown complete" << std::endl;
}

bool ExultNPCBridge::registerNPC(Actor* actor, const NPCProfile& profile) {
    if (!initialized_ || !actor) {
        return false;
    }
    
    if (isRegistered(actor)) {
        return true;
    }
    
    try {
        auto entity = std::make_unique<NPC::NPCEntity>(profile.id);
        
        auto& persona = entity->getPersona();
        persona.name = profile.name;
        persona.role.title = profile.profession;
        // backstory stored in description
        persona.description = profile.backstory;
        persona.traits.openness = profile.openness;
        persona.traits.conscientiousness = profile.conscientiousness;
        persona.traits.extraversion = profile.extraversion;
        persona.traits.agreeableness = profile.agreeableness;
        persona.traits.neuroticism = profile.neuroticism;
        
        int actorId = static_cast<int>(cognitiveNPCs_.size()) + 1;
        actorToId_[actor] = actorId;
        cognitiveNPCs_[actorId] = std::move(entity);
        
        std::cout << "[ExultNPCBridge] Registered NPC: " << profile.name 
                  << " (ID: " << actorId << ")" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[ExultNPCBridge] Failed to register NPC: " << e.what() << std::endl;
        return false;
    }
}

bool ExultNPCBridge::unregisterNPC(Actor* actor) {
    if (!actor) {
        return false;
    }
    
    auto it = actorToId_.find(actor);
    if (it == actorToId_.end()) {
        return false;
    }
    
    int actorId = it->second;
    cognitiveNPCs_.erase(actorId);
    actorToId_.erase(it);
    activeConversations_.erase(actorId);
    
    return true;
}

bool ExultNPCBridge::isRegistered(Actor* actor) const {
    return actor && actorToId_.find(actor) != actorToId_.end();
}

NPC::NPCEntity* ExultNPCBridge::getCognitiveNPC(Actor* actor) {
    if (!actor) return nullptr;
    
    auto it = actorToId_.find(actor);
    if (it == actorToId_.end()) return nullptr;
    
    auto npcIt = cognitiveNPCs_.find(it->second);
    if (npcIt == cognitiveNPCs_.end()) return nullptr;
    
    return npcIt->second.get();
}

const NPC::NPCEntity* ExultNPCBridge::getCognitiveNPC(Actor* actor) const {
    return const_cast<ExultNPCBridge*>(this)->getCognitiveNPC(actor);
}

int ExultNPCBridge::getActorId(Actor* actor) const {
    auto it = actorToId_.find(actor);
    return (it != actorToId_.end()) ? it->second : -1;
}

std::string ExultNPCBridge::processDialogue(
    Actor* npc,
    const std::string& playerInput,
    const DialogueContext& context
) {
    if (!initialized_ || !npc) {
        return "";
    }
    
    auto* entity = getCognitiveNPC(npc);
    if (!entity) {
        return "";
    }
    
    try {
        // Build NPC context for dialogue engine
        NPC::LLM::NPCContext npcContext;
        npcContext.name = entity->getPersona().name;
        npcContext.occupation = entity->getPersona().role.title;
        npcContext.location = context.location;
        npcContext.currentMood = context.mood;
        
        // Build personality string
        std::stringstream ss;
        const auto& p = entity->getPersona().traits;
        if (p.extraversion > 0.6f) ss << "outgoing, ";
        if (p.agreeableness > 0.6f) ss << "friendly, ";
        if (p.conscientiousness > 0.6f) ss << "diligent, ";
        npcContext.personality = ss.str();
        
        // Build dialogue context
        NPC::Dialogue::DialogueContext dialogueCtx;
        dialogueCtx.npcId = context.npcId;
        dialogueCtx.playerId = context.playerId;
        dialogueCtx.currentLocation = context.location;
        
        // Generate response
        auto result = dialogueEngine_->generateResponse(
            playerInput,
            npcContext,
            dialogueCtx
        );
        
        if (dialogueCallback_) {
            dialogueCallback_(result.response);
        }
        
        return result.response;
        
    } catch (const std::exception& e) {
        std::cerr << "[ExultNPCBridge] Dialogue error: " << e.what() << std::endl;
        return "";
    }
}

std::vector<std::string> ExultNPCBridge::getDialogueChoices(
    Actor* npc,
    const DialogueContext& context
) {
    std::vector<std::string> choices;
    
    if (!initialized_ || !npc) {
        return choices;
    }
    
    auto* entity = getCognitiveNPC(npc);
    if (!entity) {
        return choices;
    }
    
    choices.push_back("Tell me about yourself");
    choices.push_back("What news do you have?");
    
    const std::string& profession = entity->getPersona().role.title;
    if (profession == "merchant" || profession == "shopkeeper") {
        choices.push_back("What do you have for sale?");
    } else if (profession == "guard" || profession == "soldier") {
        choices.push_back("Is there any trouble nearby?");
    } else if (profession == "mage" || profession == "wizard") {
        choices.push_back("Can you teach me magic?");
    }
    
    if (context.relationshipLevel > 0.7f) {
        choices.push_back("I need a favor, friend");
    }
    
    choices.push_back("Farewell");
    
    return choices;
}

void ExultNPCBridge::startConversation(Actor* npc, const DialogueContext& context) {
    if (!initialized_ || !npc) return;
    
    int actorId = getActorId(npc);
    if (actorId < 0) return;
    
    activeConversations_[actorId] = context;
}

void ExultNPCBridge::endConversation(Actor* npc) {
    if (!initialized_ || !npc) return;
    
    int actorId = getActorId(npc);
    if (actorId < 0) return;
    
    activeConversations_.erase(actorId);
}

BehaviorSuggestion ExultNPCBridge::suggestBehavior(
    Actor* npc,
    int currentSchedule,
    const std::vector<std::string>& nearbyObjects,
    const std::vector<std::string>& nearbyNPCs
) {
    BehaviorSuggestion suggestion;
    suggestion.type = BehaviorSuggestion::Type::CONTINUE_CURRENT;
    suggestion.confidence = 0.5f;
    
    if (!initialized_ || !npc) {
        return suggestion;
    }
    
    auto* entity = getCognitiveNPC(npc);
    if (!entity) {
        return suggestion;
    }
    
    try {
        std::vector<std::string> options;
        options.push_back("continue_current");
        
        for (const auto& obj : nearbyObjects) {
            options.push_back("interact:" + obj);
        }
        
        for (const auto& other : nearbyNPCs) {
            options.push_back("approach:" + other);
        }
        
        auto decision = entity->makeDecision(options);
        
        if (decision.action.find("interact:") == 0) {
            suggestion.type = BehaviorSuggestion::Type::INTERACT_WITH_OBJECT;
            suggestion.targetId = decision.action.substr(9);
        } else if (decision.action.find("approach:") == 0) {
            suggestion.type = BehaviorSuggestion::Type::APPROACH_NPC;
            suggestion.targetId = decision.action.substr(9);
        }
        
        suggestion.confidence = static_cast<float>(decision.confidence);
        suggestion.reasoning = decision.reasoning;
        
        if (behaviorCallback_) {
            behaviorCallback_(suggestion);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ExultNPCBridge] Behavior suggestion error: " << e.what() << std::endl;
    }
    
    return suggestion;
}

void ExultNPCBridge::notifyEvent(
    Actor* npc,
    const std::string& eventType,
    const std::map<std::string, std::string>& eventData
) {
    if (!initialized_ || !npc) return;
    
    auto* entity = getCognitiveNPC(npc);
    if (!entity) return;
    
    entity->notifyEvent(eventType, eventData);
}

void ExultNPCBridge::update(Actor* npc, double deltaTime) {
    if (!initialized_ || !npc) return;
    
    auto* entity = getCognitiveNPC(npc);
    if (!entity) return;
    
    entity->update(deltaTime);
}

float ExultNPCBridge::getRelationship(Actor* npc1, Actor* npc2) const {
    if (!initialized_ || !npc1 || !npc2) return 0.5f;
    return 0.5f;  // Default neutral relationship
}

void ExultNPCBridge::modifyRelationship(Actor* npc1, Actor* npc2, float delta) {
    // TODO: Implement relationship modification
}

void ExultNPCBridge::recordMemory(
    Actor* npc,
    const std::string& memoryType,
    const std::string& content,
    float importance
) {
    if (!initialized_ || !npc) return;
    
    auto* entity = getCognitiveNPC(npc);
    if (!entity) return;
    
    entity->notifyEvent("memory_" + memoryType, {
        {"content", content},
        {"importance", std::to_string(importance)}
    });
}

std::vector<std::string> ExultNPCBridge::queryMemory(
    Actor* npc,
    const std::string& query,
    int maxResults
) {
    std::vector<std::string> results;
    
    if (!initialized_ || !npc) return results;
    
    auto* entity = getCognitiveNPC(npc);
    if (!entity) return results;
    
    results.push_back("Recent activity: " + entity->getCurrentActivity());
    
    return results;
}

// ConversationAIHook implementation

std::string ConversationAIHook::processNPCMessage(
    Actor* npc,
    const std::string& originalMessage
) {
    auto& bridge = ExultNPCBridge::getInstance();
    
    if (!bridge.isInitialized() || !bridge.isRegistered(npc)) {
        return originalMessage;
    }
    
    // For now, return original message - could enhance with personality
    return originalMessage;
}

std::vector<std::string> ConversationAIHook::generateChoices(
    Actor* npc,
    const std::vector<std::string>& originalChoices
) {
    auto& bridge = ExultNPCBridge::getInstance();
    
    if (!bridge.isInitialized() || !bridge.isRegistered(npc)) {
        return originalChoices;
    }
    
    DialogueContext context;
    context.npcId = std::to_string(bridge.getActorId(npc));
    
    return bridge.getDialogueChoices(npc, context);
}

void ConversationAIHook::handleChoice(
    Actor* npc,
    int choiceIndex,
    const std::string& choiceText
) {
    auto& bridge = ExultNPCBridge::getInstance();
    
    if (!bridge.isInitialized() || !bridge.isRegistered(npc)) {
        return;
    }
    
    // Record the choice in memory
    bridge.recordMemory(npc, "dialogue", "Player chose: " + choiceText, 0.5f);
}

} // namespace Exult
} // namespace Ultima
