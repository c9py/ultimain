/**
 * ScheduleIntegration.h - Integration of NPC AI with Exult's schedule system
 * 
 * Provides AI-enhanced scheduling and behavior for NPCs, allowing cognitive
 * decision-making to influence NPC activities and routines.
 */

#ifndef EXULT_SCHEDULE_INTEGRATION_H
#define EXULT_SCHEDULE_INTEGRATION_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

// Forward declarations
class Actor;
class Schedule;
class Game_object;

namespace Ultima {
namespace Exult {

/**
 * ScheduleType - Mirrors Exult's Schedule::Schedule_types
 */
enum class ScheduleType {
    COMBAT = 0,
    HORIZ_PACE = 1,
    VERT_PACE = 2,
    TALK = 3,
    DANCE = 4,
    EAT = 5,
    FARM = 6,
    TEND_SHOP = 7,
    MINER = 8,
    HOUND = 9,
    STAND = 10,
    LOITER = 11,
    WANDER = 12,
    BLACKSMITH = 13,
    SLEEP = 14,
    WAIT = 15,
    SIT = 16,
    GRAZE = 17,
    BAKE = 18,
    SEW = 19,
    SHY = 20,
    LAB = 21,
    THIEF = 22,
    WAITER = 23,
    SPECIAL = 24,
    KID_GAMES = 25,
    EAT_AT_INN = 26,
    DUEL = 27,
    PREACH = 28,
    PATROL = 29,
    DESK_WORK = 30,
    FOLLOW_AVATAR = 31,
    WALK_TO_SCHEDULE = 32,
    STREET_MAINTENANCE = 33,
    ARREST_AVATAR = 34
};

/**
 * AIScheduleContext - Context for AI schedule decisions
 */
struct AIScheduleContext {
    Actor* npc = nullptr;
    ScheduleType currentSchedule;
    int gameHour;
    int gameMinute;
    int gameDay;
    std::string location;
    
    // Nearby entities
    std::vector<Actor*> nearbyNPCs;
    std::vector<Game_object*> nearbyObjects;
    
    // NPC state
    float hunger = 0.0f;
    float fatigue = 0.0f;
    float health = 1.0f;
    bool inCombat = false;
    bool isFollowing = false;
    
    // Environmental factors
    bool isIndoors = false;
    bool isNight = false;
    bool isRaining = false;
};

/**
 * AIScheduleDecision - AI's decision about what schedule to use
 */
struct AIScheduleDecision {
    bool shouldChange = false;
    ScheduleType newSchedule;
    std::string targetId;           // For schedules that need a target
    int targetX = 0, targetY = 0;   // For location-based schedules
    float confidence = 0.0f;
    std::string reasoning;
    
    // Additional actions
    bool shouldSpeak = false;
    std::string speechText;
    
    bool shouldInteract = false;
    Game_object* interactionTarget = nullptr;
};

/**
 * ScheduleHooks - Hooks into Exult's schedule system
 */
class ScheduleHooks {
public:
    static ScheduleHooks& getInstance();
    
    /**
     * Hook called when Schedule::now_what() is invoked
     * Allows AI to suggest alternative behavior
     */
    using NowWhatHook = std::function<AIScheduleDecision(const AIScheduleContext&)>;
    void setNowWhatHook(NowWhatHook hook);
    AIScheduleDecision invokeNowWhatHook(const AIScheduleContext& context);
    
    /**
     * Hook called when a schedule is about to change
     */
    using ScheduleChangeHook = std::function<bool(Actor*, ScheduleType, ScheduleType)>;
    void setScheduleChangeHook(ScheduleChangeHook hook);
    bool invokeScheduleChangeHook(Actor* npc, ScheduleType from, ScheduleType to);
    
    /**
     * Hook called when NPC goes dormant
     */
    using DormantHook = std::function<void(Actor*)>;
    void setDormantHook(DormantHook hook);
    void invokeDormantHook(Actor* npc);
    
    /**
     * Hook called when NPC becomes active
     */
    using ActiveHook = std::function<void(Actor*)>;
    void setActiveHook(ActiveHook hook);
    void invokeActiveHook(Actor* npc);
    
private:
    ScheduleHooks() = default;
    NowWhatHook nowWhatHook_;
    ScheduleChangeHook scheduleChangeHook_;
    DormantHook dormantHook_;
    ActiveHook activeHook_;
};

/**
 * AIScheduleManager - Manages AI-enhanced scheduling for NPCs
 */
class AIScheduleManager {
public:
    static AIScheduleManager& getInstance();
    
    /**
     * Initialize the schedule manager
     */
    bool initialize();
    
    /**
     * Shutdown the schedule manager
     */
    void shutdown();
    
    /**
     * Check if manager is active
     */
    bool isActive() const { return active_; }
    
    /**
     * Enable/disable AI schedule enhancement
     */
    void setEnabled(bool enabled);
    bool isEnabled() const { return enabled_; }
    
    /**
     * Process a now_what() call for an NPC
     * Returns true if AI handled the decision
     */
    bool processNowWhat(Actor* npc, const AIScheduleContext& context);
    
    /**
     * Get the AI's schedule decision for an NPC
     */
    AIScheduleDecision getScheduleDecision(Actor* npc, const AIScheduleContext& context);
    
    /**
     * Register a custom schedule behavior
     */
    using CustomScheduleBehavior = std::function<void(Actor*, const AIScheduleContext&)>;
    void registerCustomBehavior(ScheduleType schedule, CustomScheduleBehavior behavior);
    
    /**
     * Set schedule priorities for an NPC
     * Higher priority schedules are preferred when multiple are valid
     */
    void setSchedulePriorities(
        Actor* npc,
        const std::map<ScheduleType, float>& priorities
    );
    
    /**
     * Add a schedule constraint for an NPC
     * Constraints limit when certain schedules can be used
     */
    struct ScheduleConstraint {
        ScheduleType schedule;
        int startHour;
        int endHour;
        bool requiresIndoors;
        bool requiresOutdoors;
        float minHealth;
        float maxFatigue;
    };
    void addScheduleConstraint(Actor* npc, const ScheduleConstraint& constraint);
    
    /**
     * Update all managed NPCs
     */
    void update(double deltaTime);
    
private:
    AIScheduleManager() = default;
    
    bool active_ = false;
    bool enabled_ = true;
    
    // Custom behaviors per schedule type
    std::map<ScheduleType, CustomScheduleBehavior> customBehaviors_;
    
    // Per-NPC data
    struct NPCScheduleData {
        std::map<ScheduleType, float> priorities;
        std::vector<ScheduleConstraint> constraints;
        ScheduleType lastSchedule;
        double timeInSchedule = 0.0;
    };
    std::map<Actor*, NPCScheduleData> npcData_;
    
    // Internal methods
    bool isScheduleValid(
        Actor* npc,
        ScheduleType schedule,
        const AIScheduleContext& context
    );
    
    float evaluateSchedule(
        Actor* npc,
        ScheduleType schedule,
        const AIScheduleContext& context
    );
    
    ScheduleType selectBestSchedule(
        Actor* npc,
        const AIScheduleContext& context
    );
};

/**
 * BehaviorIntegration - High-level behavior integration
 */
class BehaviorIntegration {
public:
    static BehaviorIntegration& getInstance();
    
    /**
     * Initialize behavior integration
     */
    bool initialize();
    
    /**
     * Shutdown behavior integration
     */
    void shutdown();
    
    /**
     * Process an NPC's behavior tick
     */
    void processBehavior(Actor* npc, double deltaTime);
    
    /**
     * Handle an NPC event
     */
    void handleEvent(
        Actor* npc,
        const std::string& eventType,
        const std::map<std::string, std::string>& eventData
    );
    
    /**
     * Get spontaneous speech for an NPC
     * Returns empty string if NPC has nothing to say
     */
    std::string getSpontaneousSpeech(Actor* npc);
    
    /**
     * Check if NPC should initiate interaction with another NPC
     */
    bool shouldInitiateInteraction(Actor* npc, Actor* target);
    
    /**
     * Get interaction type for NPC-to-NPC interaction
     */
    std::string getInteractionType(Actor* npc, Actor* target);
    
private:
    BehaviorIntegration() = default;
    
    bool active_ = false;
    
    // Spontaneous speech cooldowns
    std::map<Actor*, double> speechCooldowns_;
    
    // Interaction cooldowns
    std::map<std::pair<Actor*, Actor*>, double> interactionCooldowns_;
};

/**
 * Convenience macros for Exult schedule.cc integration
 */

// Insert at the start of Schedule::now_what()
#define NPCAI_HOOK_NOW_WHAT(npc, context) \
    do { \
        auto& manager = Ultima::Exult::AIScheduleManager::getInstance(); \
        if (manager.isActive() && manager.isEnabled()) { \
            if (manager.processNowWhat(npc, context)) { \
                return; \
            } \
        } \
    } while(0)

// Insert when schedule changes
#define NPCAI_HOOK_SCHEDULE_CHANGE(npc, from, to) \
    Ultima::Exult::ScheduleHooks::getInstance().invokeScheduleChangeHook(npc, \
        static_cast<Ultima::Exult::ScheduleType>(from), \
        static_cast<Ultima::Exult::ScheduleType>(to))

// Insert in Schedule::im_dormant()
#define NPCAI_HOOK_DORMANT(npc) \
    Ultima::Exult::ScheduleHooks::getInstance().invokeDormantHook(npc)

} // namespace Exult
} // namespace Ultima

#endif // EXULT_SCHEDULE_INTEGRATION_H
