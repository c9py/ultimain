/**
 * ScheduleIntegration.cpp - Implementation of schedule system integration
 */

#include "ScheduleIntegration.h"
#include "ExultNPCBridge.h"
#include "../../npc/include/NPCSystem.h"

#include <algorithm>
#include <iostream>
#include <cmath>

namespace Ultima {
namespace Exult {

// ScheduleHooks implementation

ScheduleHooks& ScheduleHooks::getInstance() {
    static ScheduleHooks instance;
    return instance;
}

void ScheduleHooks::setNowWhatHook(NowWhatHook hook) {
    nowWhatHook_ = std::move(hook);
}

AIScheduleDecision ScheduleHooks::invokeNowWhatHook(const AIScheduleContext& context) {
    if (nowWhatHook_) {
        return nowWhatHook_(context);
    }
    return AIScheduleDecision();
}

void ScheduleHooks::setScheduleChangeHook(ScheduleChangeHook hook) {
    scheduleChangeHook_ = std::move(hook);
}

bool ScheduleHooks::invokeScheduleChangeHook(Actor* npc, ScheduleType from, ScheduleType to) {
    if (scheduleChangeHook_) {
        return scheduleChangeHook_(npc, from, to);
    }
    return true;  // Allow by default
}

void ScheduleHooks::setDormantHook(DormantHook hook) {
    dormantHook_ = std::move(hook);
}

void ScheduleHooks::invokeDormantHook(Actor* npc) {
    if (dormantHook_) {
        dormantHook_(npc);
    }
}

void ScheduleHooks::setActiveHook(ActiveHook hook) {
    activeHook_ = std::move(hook);
}

void ScheduleHooks::invokeActiveHook(Actor* npc) {
    if (activeHook_) {
        activeHook_(npc);
    }
}

// AIScheduleManager implementation

AIScheduleManager& AIScheduleManager::getInstance() {
    static AIScheduleManager instance;
    return instance;
}

bool AIScheduleManager::initialize() {
    if (active_) {
        return true;
    }
    
    // Set up hooks
    auto& hooks = ScheduleHooks::getInstance();
    
    hooks.setNowWhatHook([this](const AIScheduleContext& context) {
        return getScheduleDecision(context.npc, context);
    });
    
    hooks.setScheduleChangeHook([this](Actor* npc, ScheduleType from, ScheduleType to) {
        // Record the schedule change
        auto& bridge = ExultNPCBridge::getInstance();
        if (bridge.isRegistered(npc)) {
            std::string content = "Changed schedule from " + 
                std::to_string(static_cast<int>(from)) + " to " +
                std::to_string(static_cast<int>(to));
            bridge.recordMemory(npc, "schedule", content, 0.3f);
        }
        
        // Update our tracking
        auto it = npcData_.find(npc);
        if (it != npcData_.end()) {
            it->second.lastSchedule = to;
            it->second.timeInSchedule = 0.0;
        }
        
        return true;  // Allow the change
    });
    
    hooks.setDormantHook([](Actor* npc) {
        auto& bridge = ExultNPCBridge::getInstance();
        if (bridge.isRegistered(npc)) {
            bridge.notifyEvent(npc, "dormant", {});
        }
    });
    
    hooks.setActiveHook([](Actor* npc) {
        auto& bridge = ExultNPCBridge::getInstance();
        if (bridge.isRegistered(npc)) {
            bridge.notifyEvent(npc, "active", {});
        }
    });
    
    active_ = true;
    std::cout << "[AIScheduleManager] Initialized successfully" << std::endl;
    return true;
}

void AIScheduleManager::shutdown() {
    if (!active_) {
        return;
    }
    
    // Clear hooks
    auto& hooks = ScheduleHooks::getInstance();
    hooks.setNowWhatHook(nullptr);
    hooks.setScheduleChangeHook(nullptr);
    hooks.setDormantHook(nullptr);
    hooks.setActiveHook(nullptr);
    
    // Clear data
    customBehaviors_.clear();
    npcData_.clear();
    
    active_ = false;
    std::cout << "[AIScheduleManager] Shutdown complete" << std::endl;
}

void AIScheduleManager::setEnabled(bool enabled) {
    enabled_ = enabled;
}

bool AIScheduleManager::processNowWhat(Actor* npc, const AIScheduleContext& context) {
    if (!active_ || !enabled_ || !npc) {
        return false;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    if (!bridge.isRegistered(npc)) {
        return false;
    }
    
    auto decision = getScheduleDecision(npc, context);
    
    if (!decision.shouldChange && decision.confidence < 0.5f) {
        return false;  // Let Exult handle it normally
    }
    
    // Handle spontaneous speech
    if (decision.shouldSpeak && !decision.speechText.empty()) {
        // Would call Exult's item_say here
        std::cout << "[AIScheduleManager] NPC says: " << decision.speechText << std::endl;
    }
    
    // Handle custom behavior
    auto behaviorIt = customBehaviors_.find(decision.newSchedule);
    if (behaviorIt != customBehaviors_.end()) {
        behaviorIt->second(npc, context);
        return true;
    }
    
    return decision.shouldChange;
}

AIScheduleDecision AIScheduleManager::getScheduleDecision(
    Actor* npc,
    const AIScheduleContext& context
) {
    AIScheduleDecision decision;
    
    if (!npc) {
        return decision;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    auto* entity = bridge.getCognitiveNPC(npc);
    
    if (!entity) {
        return decision;
    }
    
    // Build options for the NPC to consider
    std::vector<std::string> options;
    
    // Always include current schedule as an option
    options.push_back("continue:" + std::to_string(static_cast<int>(context.currentSchedule)));
    
    // Add valid alternative schedules
    std::vector<ScheduleType> possibleSchedules = {
        ScheduleType::STAND,
        ScheduleType::LOITER,
        ScheduleType::WANDER,
        ScheduleType::SIT,
        ScheduleType::TALK
    };
    
    // Add profession-specific schedules
    const auto& persona = entity->getPersona();
    if (persona.role.title == "merchant" || persona.role.title == "shopkeeper") {
        possibleSchedules.push_back(ScheduleType::TEND_SHOP);
    } else if (persona.role.title == "guard" || persona.role.title == "soldier") {
        possibleSchedules.push_back(ScheduleType::PATROL);
    } else if (persona.role.title == "farmer") {
        possibleSchedules.push_back(ScheduleType::FARM);
    } else if (persona.role.title == "blacksmith") {
        possibleSchedules.push_back(ScheduleType::BLACKSMITH);
    } else if (persona.role.title == "baker") {
        possibleSchedules.push_back(ScheduleType::BAKE);
    }
    
    // Time-based schedules
    if (context.isNight || context.gameHour >= 22 || context.gameHour < 6) {
        possibleSchedules.push_back(ScheduleType::SLEEP);
    }
    if (context.gameHour >= 6 && context.gameHour <= 9 ||
        context.gameHour >= 12 && context.gameHour <= 14 ||
        context.gameHour >= 18 && context.gameHour <= 20) {
        possibleSchedules.push_back(ScheduleType::EAT);
    }
    
    // Filter by constraints and add to options
    for (auto schedule : possibleSchedules) {
        if (isScheduleValid(npc, schedule, context)) {
            options.push_back("schedule:" + std::to_string(static_cast<int>(schedule)));
        }
    }
    
    // Add interaction options if there are nearby NPCs
    for (auto* other : context.nearbyNPCs) {
        if (other != npc) {
            options.push_back("interact:npc");
        }
    }
    
    // Let the cognitive NPC make a decision
    auto npcDecision = entity->makeDecision(options);
    
    // Parse the decision
    if (npcDecision.action.find("continue:") == 0) {
        decision.shouldChange = false;
        decision.confidence = static_cast<float>(npcDecision.confidence);
    } else if (npcDecision.action.find("schedule:") == 0) {
        int scheduleNum = std::stoi(npcDecision.action.substr(9));
        decision.shouldChange = true;
        decision.newSchedule = static_cast<ScheduleType>(scheduleNum);
        decision.confidence = static_cast<float>(npcDecision.confidence);
        decision.reasoning = npcDecision.reasoning;
    } else if (npcDecision.action.find("interact:") == 0) {
        decision.shouldChange = true;
        decision.newSchedule = ScheduleType::TALK;
        decision.confidence = static_cast<float>(npcDecision.confidence);
    }
    
    // Check for spontaneous speech
    auto& behavior = BehaviorIntegration::getInstance();
    std::string speech = behavior.getSpontaneousSpeech(npc);
    if (!speech.empty()) {
        decision.shouldSpeak = true;
        decision.speechText = speech;
    }
    
    return decision;
}

void AIScheduleManager::registerCustomBehavior(
    ScheduleType schedule,
    CustomScheduleBehavior behavior
) {
    customBehaviors_[schedule] = std::move(behavior);
}

void AIScheduleManager::setSchedulePriorities(
    Actor* npc,
    const std::map<ScheduleType, float>& priorities
) {
    npcData_[npc].priorities = priorities;
}

void AIScheduleManager::addScheduleConstraint(
    Actor* npc,
    const ScheduleConstraint& constraint
) {
    npcData_[npc].constraints.push_back(constraint);
}

void AIScheduleManager::update(double deltaTime) {
    if (!active_ || !enabled_) {
        return;
    }
    
    // Update time tracking for all NPCs
    for (auto& [npc, data] : npcData_) {
        data.timeInSchedule += deltaTime;
    }
}

bool AIScheduleManager::isScheduleValid(
    Actor* npc,
    ScheduleType schedule,
    const AIScheduleContext& context
) {
    auto it = npcData_.find(npc);
    if (it == npcData_.end()) {
        return true;  // No constraints, allow everything
    }
    
    for (const auto& constraint : it->second.constraints) {
        if (constraint.schedule != schedule) {
            continue;
        }
        
        // Check time constraint
        if (constraint.startHour <= constraint.endHour) {
            if (context.gameHour < constraint.startHour || 
                context.gameHour > constraint.endHour) {
                return false;
            }
        } else {
            // Wraps around midnight
            if (context.gameHour < constraint.startHour && 
                context.gameHour > constraint.endHour) {
                return false;
            }
        }
        
        // Check indoor/outdoor constraint
        if (constraint.requiresIndoors && !context.isIndoors) {
            return false;
        }
        if (constraint.requiresOutdoors && context.isIndoors) {
            return false;
        }
        
        // Check health constraint
        if (context.health < constraint.minHealth) {
            return false;
        }
        
        // Check fatigue constraint
        if (context.fatigue > constraint.maxFatigue) {
            return false;
        }
    }
    
    return true;
}

float AIScheduleManager::evaluateSchedule(
    Actor* npc,
    ScheduleType schedule,
    const AIScheduleContext& context
) {
    float score = 0.5f;  // Base score
    
    auto it = npcData_.find(npc);
    if (it != npcData_.end()) {
        auto priorityIt = it->second.priorities.find(schedule);
        if (priorityIt != it->second.priorities.end()) {
            score = priorityIt->second;
        }
    }
    
    // Adjust based on context
    switch (schedule) {
        case ScheduleType::SLEEP:
            if (context.fatigue > 0.7f) score += 0.3f;
            if (context.isNight) score += 0.2f;
            break;
            
        case ScheduleType::EAT:
            if (context.hunger > 0.7f) score += 0.3f;
            break;
            
        case ScheduleType::COMBAT:
            if (context.inCombat) score += 0.5f;
            break;
            
        case ScheduleType::FOLLOW_AVATAR:
            if (context.isFollowing) score += 0.4f;
            break;
            
        default:
            break;
    }
    
    return std::min(1.0f, std::max(0.0f, score));
}

ScheduleType AIScheduleManager::selectBestSchedule(
    Actor* npc,
    const AIScheduleContext& context
) {
    ScheduleType best = context.currentSchedule;
    float bestScore = evaluateSchedule(npc, best, context);
    
    std::vector<ScheduleType> candidates = {
        ScheduleType::STAND,
        ScheduleType::LOITER,
        ScheduleType::WANDER,
        ScheduleType::SIT,
        ScheduleType::SLEEP,
        ScheduleType::EAT
    };
    
    for (auto schedule : candidates) {
        if (!isScheduleValid(npc, schedule, context)) {
            continue;
        }
        
        float score = evaluateSchedule(npc, schedule, context);
        if (score > bestScore) {
            bestScore = score;
            best = schedule;
        }
    }
    
    return best;
}

// BehaviorIntegration implementation

BehaviorIntegration& BehaviorIntegration::getInstance() {
    static BehaviorIntegration instance;
    return instance;
}

bool BehaviorIntegration::initialize() {
    if (active_) {
        return true;
    }
    
    active_ = true;
    std::cout << "[BehaviorIntegration] Initialized successfully" << std::endl;
    return true;
}

void BehaviorIntegration::shutdown() {
    if (!active_) {
        return;
    }
    
    speechCooldowns_.clear();
    interactionCooldowns_.clear();
    
    active_ = false;
    std::cout << "[BehaviorIntegration] Shutdown complete" << std::endl;
}

void BehaviorIntegration::processBehavior(Actor* npc, double deltaTime) {
    if (!active_ || !npc) {
        return;
    }
    
    // Update cooldowns
    auto speechIt = speechCooldowns_.find(npc);
    if (speechIt != speechCooldowns_.end()) {
        speechIt->second -= deltaTime;
        if (speechIt->second <= 0) {
            speechCooldowns_.erase(speechIt);
        }
    }
    
    // Update NPC through the bridge
    auto& bridge = ExultNPCBridge::getInstance();
    bridge.update(npc, deltaTime);
}

void BehaviorIntegration::handleEvent(
    Actor* npc,
    const std::string& eventType,
    const std::map<std::string, std::string>& eventData
) {
    if (!active_ || !npc) {
        return;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    bridge.notifyEvent(npc, eventType, eventData);
}

std::string BehaviorIntegration::getSpontaneousSpeech(Actor* npc) {
    if (!active_ || !npc) {
        return "";
    }
    
    // Check cooldown
    auto it = speechCooldowns_.find(npc);
    if (it != speechCooldowns_.end() && it->second > 0) {
        return "";
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    auto* entity = bridge.getCognitiveNPC(npc);
    
    if (!entity) {
        return "";
    }
    
    // Check if NPC wants to say something based on their state
    const auto& persona = entity->getPersona();
    const auto& emotional = entity->getEmotionalState();
    
    // High extraversion NPCs speak more often
    float speechChance = persona.traits.extraversion * 0.1f;
    
    // Emotional states can trigger speech
    if (emotional.overallValence > 0.5 && emotional.arousal > 0.7f) {
        speechChance += 0.2f;
    } else if (emotional.emotions.count(Ultima::NPC::Persona::EmotionType::Anger) && emotional.emotions.at(Ultima::NPC::Persona::EmotionType::Anger) > 0.5 && emotional.arousal > 0.6f) {
        speechChance += 0.3f;
    } else if (emotional.overallValence < -0.3 && emotional.arousal > 0.5f) {
        speechChance += 0.1f;
    }
    
    // Random check
    float roll = static_cast<float>(rand()) / RAND_MAX;
    if (roll > speechChance) {
        return "";
    }
    
    // Generate speech based on mood and personality
    std::vector<std::string> possiblePhrases;
    
    if (emotional.overallValence > 0.5) {
        possiblePhrases = {
            "What a fine day!",
            "'Tis good to be alive!",
            "The virtues guide us well.",
            "May the Avatar bless thee!"
        };
    } else if (emotional.emotions.count(Ultima::NPC::Persona::EmotionType::Anger) && emotional.emotions.at(Ultima::NPC::Persona::EmotionType::Anger) > 0.5) {
        possiblePhrases = {
            "Bah! Leave me be!",
            "I have no patience for this!",
            "The Guardian's influence grows...",
            "These are troubled times."
        };
    } else if (emotional.overallValence < -0.3) {
        possiblePhrases = {
            "*sigh*",
            "I miss the old days...",
            "Times are hard, friend.",
            "The world grows darker."
        };
    } else if (emotional.emotions.count(Ultima::NPC::Persona::EmotionType::Fear) && emotional.emotions.at(Ultima::NPC::Persona::EmotionType::Fear) > 0.5) {
        possiblePhrases = {
            "Did you hear that?",
            "I sense danger nearby...",
            "We must be careful.",
            "Something is not right."
        };
    } else {
        possiblePhrases = {
            "Greetings, traveler.",
            "The day passes slowly.",
            "Britannia endures.",
            "May thy path be safe."
        };
    }
    
    if (possiblePhrases.empty()) {
        return "";
    }
    
    // Select a random phrase
    int index = rand() % possiblePhrases.size();
    
    // Set cooldown (30-60 seconds)
    speechCooldowns_[npc] = 30.0 + (rand() % 30);
    
    return possiblePhrases[index];
}

bool BehaviorIntegration::shouldInitiateInteraction(Actor* npc, Actor* target) {
    if (!active_ || !npc || !target) {
        return false;
    }
    
    // Check cooldown
    auto key = std::make_pair(npc, target);
    auto it = interactionCooldowns_.find(key);
    if (it != interactionCooldowns_.end() && it->second > 0) {
        return false;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    
    // Both must be registered
    if (!bridge.isRegistered(npc) || !bridge.isRegistered(target)) {
        return false;
    }
    
    // Check relationship
    float relationship = bridge.getRelationship(npc, target);
    
    // Higher relationship = more likely to interact
    float interactChance = 0.1f + (relationship * 0.3f);
    
    float roll = static_cast<float>(rand()) / RAND_MAX;
    if (roll > interactChance) {
        return false;
    }
    
    // Set cooldown (60-120 seconds)
    interactionCooldowns_[key] = 60.0 + (rand() % 60);
    
    return true;
}

std::string BehaviorIntegration::getInteractionType(Actor* npc, Actor* target) {
    if (!active_ || !npc || !target) {
        return "greeting";
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    float relationship = bridge.getRelationship(npc, target);
    
    if (relationship > 0.8f) {
        return "friendly_chat";
    } else if (relationship > 0.5f) {
        return "greeting";
    } else if (relationship > 0.2f) {
        return "neutral";
    } else {
        return "hostile_glare";
    }
}

} // namespace Exult
} // namespace Ultima
