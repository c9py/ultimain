/**
 * DialogueHooks.cpp - Implementation of dialogue system hooks
 */

#include "DialogueHooks.h"
#include "ExultNPCBridge.h"

#include <algorithm>
#include <iostream>

namespace Ultima {
namespace Exult {

// DialogueHooks implementation

DialogueHooks& DialogueHooks::getInstance() {
    static DialogueHooks instance;
    return instance;
}

int DialogueHooks::registerHook(DialogueEvent event, DialogueHookCallback callback) {
    HookEntry entry;
    entry.handle = nextHandle_++;
    entry.event = event;
    entry.callback = std::move(callback);
    hooks_.push_back(std::move(entry));
    return entry.handle;
}

void DialogueHooks::unregisterHook(int handle) {
    hooks_.erase(
        std::remove_if(hooks_.begin(), hooks_.end(),
            [handle](const HookEntry& e) { return e.handle == handle; }),
        hooks_.end()
    );
}

void DialogueHooks::clearHooks(DialogueEvent event) {
    hooks_.erase(
        std::remove_if(hooks_.begin(), hooks_.end(),
            [event](const HookEntry& e) { return e.event == event; }),
        hooks_.end()
    );
}

void DialogueHooks::clearAllHooks() {
    hooks_.clear();
}

DialogueHookResult DialogueHooks::invokeHooks(const DialogueEventData& data) {
    DialogueHookResult finalResult;
    
    for (const auto& entry : hooks_) {
        if (entry.event == data.event) {
            try {
                auto result = entry.callback(data);
                
                // Merge results - later hooks can override earlier ones
                if (result.handled) {
                    finalResult.handled = true;
                    if (!result.modifiedText.empty()) {
                        finalResult.modifiedText = result.modifiedText;
                    }
                    if (!result.modifiedChoices.empty()) {
                        finalResult.modifiedChoices = result.modifiedChoices;
                    }
                    if (result.suppressDisplay) {
                        finalResult.suppressDisplay = true;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "[DialogueHooks] Hook error: " << e.what() << std::endl;
            }
        }
    }
    
    return finalResult;
}

DialogueHookResult DialogueHooks::onConversationStart(Actor* npc) {
    DialogueEventData data;
    data.event = DialogueEvent::CONVERSATION_START;
    data.npc = npc;
    return invokeHooks(data);
}

DialogueHookResult DialogueHooks::onConversationEnd(Actor* npc) {
    DialogueEventData data;
    data.event = DialogueEvent::CONVERSATION_END;
    data.npc = npc;
    return invokeHooks(data);
}

DialogueHookResult DialogueHooks::onNPCSpeaks(Actor* npc, const std::string& text) {
    DialogueEventData data;
    data.event = DialogueEvent::NPC_SPEAKS;
    data.npc = npc;
    data.text = text;
    return invokeHooks(data);
}

DialogueHookResult DialogueHooks::onPlayerChoicesShown(
    Actor* npc,
    const std::vector<std::string>& choices
) {
    DialogueEventData data;
    data.event = DialogueEvent::PLAYER_CHOICE_SHOWN;
    data.npc = npc;
    data.choices = choices;
    return invokeHooks(data);
}

DialogueHookResult DialogueHooks::onPlayerChoiceSelected(
    Actor* npc,
    int choiceIndex,
    const std::string& choiceText
) {
    DialogueEventData data;
    data.event = DialogueEvent::PLAYER_CHOICE_SELECTED;
    data.npc = npc;
    data.choiceIndex = choiceIndex;
    data.text = choiceText;
    return invokeHooks(data);
}

DialogueHookResult DialogueHooks::onFaceShown(Actor* npc, int shape, int frame) {
    DialogueEventData data;
    data.event = DialogueEvent::FACE_SHOWN;
    data.npc = npc;
    data.faceShape = shape;
    data.faceFrame = frame;
    return invokeHooks(data);
}

DialogueHookResult DialogueHooks::onFaceRemoved(Actor* npc, int shape) {
    DialogueEventData data;
    data.event = DialogueEvent::FACE_REMOVED;
    data.npc = npc;
    data.faceShape = shape;
    return invokeHooks(data);
}

// UsecodeHooks implementation

UsecodeHooks& UsecodeHooks::getInstance() {
    static UsecodeHooks instance;
    return instance;
}

void UsecodeHooks::setItemSayHook(ItemSayHook hook) {
    itemSayHook_ = std::move(hook);
}

std::string UsecodeHooks::invokeItemSayHook(Actor* npc, const std::string& text) {
    if (itemSayHook_) {
        return itemSayHook_(npc, text);
    }
    return text;
}

void UsecodeHooks::setAddAnswerHook(AddAnswerHook hook) {
    addAnswerHook_ = std::move(hook);
}

bool UsecodeHooks::invokeAddAnswerHook(Actor* npc, const std::string& answer) {
    if (addAnswerHook_) {
        return addAnswerHook_(npc, answer);
    }
    return true;  // Allow by default
}

void UsecodeHooks::setRemoveAnswerHook(RemoveAnswerHook hook) {
    removeAnswerHook_ = std::move(hook);
}

bool UsecodeHooks::invokeRemoveAnswerHook(Actor* npc, const std::string& answer) {
    if (removeAnswerHook_) {
        return removeAnswerHook_(npc, answer);
    }
    return true;  // Allow by default
}

void UsecodeHooks::setInitConversationHook(InitConversationHook hook) {
    initConversationHook_ = std::move(hook);
}

void UsecodeHooks::invokeInitConversationHook(Actor* npc) {
    if (initConversationHook_) {
        initConversationHook_(npc);
    }
}

void UsecodeHooks::setEndConversationHook(EndConversationHook hook) {
    endConversationHook_ = std::move(hook);
}

void UsecodeHooks::invokeEndConversationHook(Actor* npc) {
    if (endConversationHook_) {
        endConversationHook_(npc);
    }
}

// AIDialogueIntegration implementation

AIDialogueIntegration& AIDialogueIntegration::getInstance() {
    static AIDialogueIntegration instance;
    return instance;
}

bool AIDialogueIntegration::initialize() {
    if (active_) {
        return true;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    if (!bridge.isInitialized()) {
        if (!bridge.initialize()) {
            std::cerr << "[AIDialogueIntegration] Failed to initialize NPC bridge" << std::endl;
            return false;
        }
    }
    
    auto& dialogueHooks = DialogueHooks::getInstance();
    auto& usecodeHooks = UsecodeHooks::getInstance();
    
    // Register dialogue hooks
    hookHandles_.push_back(
        dialogueHooks.registerHook(
            DialogueEvent::CONVERSATION_START,
            [this](const DialogueEventData& data) {
                return handleConversationStart(data);
            }
        )
    );
    
    hookHandles_.push_back(
        dialogueHooks.registerHook(
            DialogueEvent::CONVERSATION_END,
            [this](const DialogueEventData& data) {
                return handleConversationEnd(data);
            }
        )
    );
    
    hookHandles_.push_back(
        dialogueHooks.registerHook(
            DialogueEvent::NPC_SPEAKS,
            [this](const DialogueEventData& data) {
                return handleNPCSpeaks(data);
            }
        )
    );
    
    hookHandles_.push_back(
        dialogueHooks.registerHook(
            DialogueEvent::PLAYER_CHOICE_SHOWN,
            [this](const DialogueEventData& data) {
                return handlePlayerChoices(data);
            }
        )
    );
    
    hookHandles_.push_back(
        dialogueHooks.registerHook(
            DialogueEvent::PLAYER_CHOICE_SELECTED,
            [this](const DialogueEventData& data) {
                return handlePlayerChoice(data);
            }
        )
    );
    
    // Register usecode hooks
    usecodeHooks.setItemSayHook(
        [this](Actor* npc, const std::string& text) {
            return handleItemSay(npc, text);
        }
    );
    
    usecodeHooks.setInitConversationHook(
        [this](Actor* npc) {
            handleInitConversation(npc);
        }
    );
    
    usecodeHooks.setEndConversationHook(
        [this](Actor* npc) {
            handleEndConversation(npc);
        }
    );
    
    active_ = true;
    std::cout << "[AIDialogueIntegration] Initialized successfully" << std::endl;
    return true;
}

void AIDialogueIntegration::shutdown() {
    if (!active_) {
        return;
    }
    
    auto& dialogueHooks = DialogueHooks::getInstance();
    
    // Unregister all hooks
    for (int handle : hookHandles_) {
        dialogueHooks.unregisterHook(handle);
    }
    hookHandles_.clear();
    
    // Clear usecode hooks
    auto& usecodeHooks = UsecodeHooks::getInstance();
    usecodeHooks.setItemSayHook(nullptr);
    usecodeHooks.setInitConversationHook(nullptr);
    usecodeHooks.setEndConversationHook(nullptr);
    
    active_ = false;
    std::cout << "[AIDialogueIntegration] Shutdown complete" << std::endl;
}

void AIDialogueIntegration::setEnabled(bool enabled) {
    enabled_ = enabled;
}

void AIDialogueIntegration::setConfidenceThreshold(float threshold) {
    confidenceThreshold_ = std::max(0.0f, std::min(1.0f, threshold));
}

void AIDialogueIntegration::setDynamicChoicesEnabled(bool enabled) {
    dynamicChoicesEnabled_ = enabled;
}

DialogueHookResult AIDialogueIntegration::handleConversationStart(
    const DialogueEventData& data
) {
    DialogueHookResult result;
    
    if (!enabled_ || !data.npc) {
        return result;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    if (bridge.isRegistered(data.npc)) {
        DialogueContext context;
        context.npcId = std::to_string(reinterpret_cast<uintptr_t>(data.npc));
        context.playerId = "avatar";
        context.mood = "neutral";
        context.relationshipLevel = 0.5f;
        
        bridge.startConversation(data.npc, context);
        result.handled = true;
    }
    
    return result;
}

DialogueHookResult AIDialogueIntegration::handleConversationEnd(
    const DialogueEventData& data
) {
    DialogueHookResult result;
    
    if (!enabled_ || !data.npc) {
        return result;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    if (bridge.isRegistered(data.npc)) {
        bridge.endConversation(data.npc);
        result.handled = true;
    }
    
    return result;
}

DialogueHookResult AIDialogueIntegration::handleNPCSpeaks(
    const DialogueEventData& data
) {
    DialogueHookResult result;
    
    if (!enabled_ || !data.npc || data.text.empty()) {
        return result;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    if (bridge.isRegistered(data.npc)) {
        // Let the AI potentially modify the text
        std::string modified = bridge.hookItemSay(data.npc, data.text);
        if (modified != data.text) {
            result.handled = true;
            result.modifiedText = modified;
        }
    }
    
    return result;
}

DialogueHookResult AIDialogueIntegration::handlePlayerChoices(
    const DialogueEventData& data
) {
    DialogueHookResult result;
    
    if (!enabled_ || !dynamicChoicesEnabled_ || !data.npc) {
        return result;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    if (bridge.isRegistered(data.npc)) {
        DialogueContext context;
        context.npcId = std::to_string(reinterpret_cast<uintptr_t>(data.npc));
        context.playerId = "avatar";
        
        // Get AI-generated choices
        auto aiChoices = bridge.getDialogueChoices(data.npc, context);
        
        if (!aiChoices.empty()) {
            // Merge AI choices with original choices
            result.modifiedChoices = aiChoices;
            
            // Add original choices that aren't duplicates
            for (const auto& original : data.choices) {
                bool isDuplicate = false;
                for (const auto& ai : aiChoices) {
                    if (ai == original) {
                        isDuplicate = true;
                        break;
                    }
                }
                if (!isDuplicate) {
                    result.modifiedChoices.push_back(original);
                }
            }
            
            result.handled = true;
        }
    }
    
    return result;
}

DialogueHookResult AIDialogueIntegration::handlePlayerChoice(
    const DialogueEventData& data
) {
    DialogueHookResult result;
    
    if (!enabled_ || !data.npc) {
        return result;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    if (bridge.isRegistered(data.npc)) {
        // Record the player's choice
        bridge.recordMemory(
            data.npc,
            "dialogue_choice",
            "Player chose: " + data.text,
            0.5f
        );
        result.handled = true;
    }
    
    return result;
}

std::string AIDialogueIntegration::handleItemSay(Actor* npc, const std::string& text) {
    if (!enabled_ || !npc || text.empty()) {
        return text;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    return bridge.hookItemSay(npc, text);
}

void AIDialogueIntegration::handleInitConversation(Actor* npc) {
    if (!enabled_ || !npc) {
        return;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    bridge.hookInitConversation(npc);
}

void AIDialogueIntegration::handleEndConversation(Actor* npc) {
    if (!enabled_ || !npc) {
        return;
    }
    
    auto& bridge = ExultNPCBridge::getInstance();
    bridge.hookEndConversation(npc);
}

} // namespace Exult
} // namespace Ultima
