/**
 * DialogueHooks.h - Hooks into Exult's conversation and usecode system
 * 
 * Provides integration points for the NPC AI dialogue system to intercept
 * and enhance Exult's native conversation handling.
 */

#ifndef EXULT_DIALOGUE_HOOKS_H
#define EXULT_DIALOGUE_HOOKS_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

// Forward declarations
class Actor;
class Conversation;
class Usecode_value;

namespace Ultima {
namespace Exult {

/**
 * DialogueEvent - Events that occur during dialogue
 */
enum class DialogueEvent {
    CONVERSATION_START,
    CONVERSATION_END,
    NPC_SPEAKS,
    PLAYER_CHOICE_SHOWN,
    PLAYER_CHOICE_SELECTED,
    FACE_SHOWN,
    FACE_REMOVED,
    TOPIC_CHANGED
};

/**
 * DialogueEventData - Data associated with dialogue events
 */
struct DialogueEventData {
    DialogueEvent event;
    Actor* npc = nullptr;
    std::string text;
    int choiceIndex = -1;
    std::vector<std::string> choices;
    std::string topic;
    int faceShape = 0;
    int faceFrame = 0;
};

/**
 * DialogueHookResult - Result from a hook callback
 */
struct DialogueHookResult {
    bool handled = false;           // If true, skip default processing
    std::string modifiedText;       // Modified text (if applicable)
    std::vector<std::string> modifiedChoices;  // Modified choices (if applicable)
    bool suppressDisplay = false;   // If true, don't display anything
};

/**
 * DialogueHookCallback - Callback type for dialogue hooks
 */
using DialogueHookCallback = std::function<DialogueHookResult(const DialogueEventData&)>;

/**
 * DialogueHooks - Manager for dialogue system hooks
 */
class DialogueHooks {
public:
    static DialogueHooks& getInstance();
    
    // Hook registration
    
    /**
     * Register a callback for a specific dialogue event
     * Returns a handle that can be used to unregister
     */
    int registerHook(DialogueEvent event, DialogueHookCallback callback);
    
    /**
     * Unregister a previously registered hook
     */
    void unregisterHook(int handle);
    
    /**
     * Clear all hooks for a specific event
     */
    void clearHooks(DialogueEvent event);
    
    /**
     * Clear all hooks
     */
    void clearAllHooks();
    
    // Hook invocation (called by Exult integration code)
    
    /**
     * Called when a conversation starts
     */
    DialogueHookResult onConversationStart(Actor* npc);
    
    /**
     * Called when a conversation ends
     */
    DialogueHookResult onConversationEnd(Actor* npc);
    
    /**
     * Called when an NPC speaks
     * Can modify the text before display
     */
    DialogueHookResult onNPCSpeaks(Actor* npc, const std::string& text);
    
    /**
     * Called when player choices are about to be shown
     * Can modify or add choices
     */
    DialogueHookResult onPlayerChoicesShown(
        Actor* npc, 
        const std::vector<std::string>& choices
    );
    
    /**
     * Called when player selects a choice
     */
    DialogueHookResult onPlayerChoiceSelected(
        Actor* npc,
        int choiceIndex,
        const std::string& choiceText
    );
    
    /**
     * Called when an NPC face is shown
     */
    DialogueHookResult onFaceShown(Actor* npc, int shape, int frame);
    
    /**
     * Called when an NPC face is removed
     */
    DialogueHookResult onFaceRemoved(Actor* npc, int shape);
    
private:
    DialogueHooks() = default;
    ~DialogueHooks() = default;
    DialogueHooks(const DialogueHooks&) = delete;
    DialogueHooks& operator=(const DialogueHooks&) = delete;
    
    DialogueHookResult invokeHooks(const DialogueEventData& data);
    
    struct HookEntry {
        int handle;
        DialogueEvent event;
        DialogueHookCallback callback;
    };
    
    std::vector<HookEntry> hooks_;
    int nextHandle_ = 1;
};

/**
 * UsecodeHooks - Hooks into usecode intrinsics
 */
class UsecodeHooks {
public:
    static UsecodeHooks& getInstance();
    
    // Usecode intrinsic hooks
    
    /**
     * Hook for item_say intrinsic
     * Called when usecode makes an item/NPC say something
     */
    using ItemSayHook = std::function<std::string(Actor*, const std::string&)>;
    void setItemSayHook(ItemSayHook hook);
    std::string invokeItemSayHook(Actor* npc, const std::string& text);
    
    /**
     * Hook for add_answer intrinsic
     * Called when usecode adds a dialogue answer
     */
    using AddAnswerHook = std::function<bool(Actor*, const std::string&)>;
    void setAddAnswerHook(AddAnswerHook hook);
    bool invokeAddAnswerHook(Actor* npc, const std::string& answer);
    
    /**
     * Hook for remove_answer intrinsic
     * Called when usecode removes a dialogue answer
     */
    using RemoveAnswerHook = std::function<bool(Actor*, const std::string&)>;
    void setRemoveAnswerHook(RemoveAnswerHook hook);
    bool invokeRemoveAnswerHook(Actor* npc, const std::string& answer);
    
    /**
     * Hook for init_conversation intrinsic
     */
    using InitConversationHook = std::function<void(Actor*)>;
    void setInitConversationHook(InitConversationHook hook);
    void invokeInitConversationHook(Actor* npc);
    
    /**
     * Hook for end_conversation intrinsic
     */
    using EndConversationHook = std::function<void(Actor*)>;
    void setEndConversationHook(EndConversationHook hook);
    void invokeEndConversationHook(Actor* npc);
    
private:
    UsecodeHooks() = default;
    ~UsecodeHooks() = default;
    UsecodeHooks(const UsecodeHooks&) = delete;
    UsecodeHooks& operator=(const UsecodeHooks&) = delete;
    
    ItemSayHook itemSayHook_;
    AddAnswerHook addAnswerHook_;
    RemoveAnswerHook removeAnswerHook_;
    InitConversationHook initConversationHook_;
    EndConversationHook endConversationHook_;
};

/**
 * AIDialogueIntegration - High-level integration of AI dialogue with Exult
 * 
 * Sets up all necessary hooks to enable AI-enhanced dialogue
 */
class AIDialogueIntegration {
public:
    static AIDialogueIntegration& getInstance();
    
    /**
     * Initialize AI dialogue integration
     * Sets up all hooks and connects to the NPC AI system
     */
    bool initialize();
    
    /**
     * Shutdown AI dialogue integration
     */
    void shutdown();
    
    /**
     * Check if integration is active
     */
    bool isActive() const { return active_; }
    
    /**
     * Enable/disable AI dialogue enhancement
     */
    void setEnabled(bool enabled);
    bool isEnabled() const { return enabled_; }
    
    /**
     * Set the confidence threshold for using AI responses
     * Below this threshold, original dialogue is used
     */
    void setConfidenceThreshold(float threshold);
    float getConfidenceThreshold() const { return confidenceThreshold_; }
    
    /**
     * Enable/disable dynamic choice generation
     */
    void setDynamicChoicesEnabled(bool enabled);
    bool isDynamicChoicesEnabled() const { return dynamicChoicesEnabled_; }
    
private:
    AIDialogueIntegration() = default;
    ~AIDialogueIntegration() = default;
    AIDialogueIntegration(const AIDialogueIntegration&) = delete;
    AIDialogueIntegration& operator=(const AIDialogueIntegration&) = delete;
    
    // Hook handlers
    DialogueHookResult handleConversationStart(const DialogueEventData& data);
    DialogueHookResult handleConversationEnd(const DialogueEventData& data);
    DialogueHookResult handleNPCSpeaks(const DialogueEventData& data);
    DialogueHookResult handlePlayerChoices(const DialogueEventData& data);
    DialogueHookResult handlePlayerChoice(const DialogueEventData& data);
    
    std::string handleItemSay(Actor* npc, const std::string& text);
    void handleInitConversation(Actor* npc);
    void handleEndConversation(Actor* npc);
    
    bool active_ = false;
    bool enabled_ = true;
    float confidenceThreshold_ = 0.5f;
    bool dynamicChoicesEnabled_ = true;
    
    std::vector<int> hookHandles_;
};

/**
 * Convenience macros for Exult source code integration
 * 
 * These can be inserted into Exult's conversation.cc and ucinternal.cc
 * to enable AI dialogue hooks
 */

// Insert at the start of Conversation::show_npc_message()
#define NPCAI_HOOK_NPC_MESSAGE(npc, msg) \
    do { \
        auto& hooks = Ultima::Exult::DialogueHooks::getInstance(); \
        auto result = hooks.onNPCSpeaks(npc, msg); \
        if (result.handled) { \
            if (result.suppressDisplay) return; \
            msg = result.modifiedText; \
        } \
    } while(0)

// Insert at the start of Conversation::show_avatar_choices()
#define NPCAI_HOOK_PLAYER_CHOICES(npc, choices) \
    do { \
        auto& hooks = Ultima::Exult::DialogueHooks::getInstance(); \
        auto result = hooks.onPlayerChoicesShown(npc, choices); \
        if (result.handled && !result.modifiedChoices.empty()) { \
            choices = result.modifiedChoices; \
        } \
    } while(0)

// Insert in the item_say intrinsic handler
#define NPCAI_HOOK_ITEM_SAY(npc, text) \
    Ultima::Exult::UsecodeHooks::getInstance().invokeItemSayHook(npc, text)

// Insert in init_conversation intrinsic handler
#define NPCAI_HOOK_INIT_CONVERSATION(npc) \
    Ultima::Exult::UsecodeHooks::getInstance().invokeInitConversationHook(npc)

// Insert in end_conversation intrinsic handler
#define NPCAI_HOOK_END_CONVERSATION(npc) \
    Ultima::Exult::UsecodeHooks::getInstance().invokeEndConversationHook(npc)

} // namespace Exult
} // namespace Ultima

#endif // EXULT_DIALOGUE_HOOKS_H
