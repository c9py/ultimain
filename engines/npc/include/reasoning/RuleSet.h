/**
 * RuleSet.h - Base Rule Set for NPC Behavior
 *
 * Defines the foundational rules that govern NPC behavior,
 * decision making, and reactions to world events.
 *
 * Part of Phase 2: Reasoning
 */

#ifndef ULTIMA_NPC_RULE_SET_H
#define ULTIMA_NPC_RULE_SET_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace Ultima {
namespace NPC {
namespace Reasoning {

// Forward declarations
class TensorLogicReasoner;

/**
 * Condition types for rule evaluation
 */
enum class ConditionType {
    // Entity state
    HasItem,            // NPC has specific item
    HasGold,            // NPC has minimum gold
    HealthBelow,        // Health below threshold
    HealthAbove,        // Health above threshold
    StaminaBelow,       // Stamina below threshold
    ManaBelow,          // Mana below threshold

    // Location
    AtLocation,         // NPC at specific location
    NearEntity,         // NPC near another entity
    InBuilding,         // NPC inside a building
    InCombat,           // NPC in combat

    // Time
    TimeOfDay,          // Specific time range
    DayOfWeek,          // Specific day
    Season,             // Specific season

    // Social
    RelationshipAbove,  // Relationship with entity above threshold
    RelationshipBelow,  // Relationship below threshold
    IsFriendOf,         // Is friend of entity
    IsEnemyOf,          // Is enemy of entity
    KnowsEntity,        // Has met entity
    OwesEntity,         // Has debt to entity

    // Emotional
    EmotionAbove,       // Emotion intensity above threshold
    MoodPositive,       // Positive mood
    MoodNegative,       // Negative mood
    StressAbove,        // Stress level above threshold

    // Personality
    TraitAbove,         // Personality trait above threshold
    TraitBelow,         // Personality trait below threshold

    // Knowledge
    KnowsFact,          // Knows specific fact
    BelievesFact,       // Believes specific fact

    // Goal
    HasGoal,            // Has specific goal
    GoalProgress,       // Goal progress above threshold

    // Custom
    CustomCondition     // User-defined condition
};

/**
 * Action types for rule consequences
 */
enum class ActionType {
    // Movement
    MoveTo,             // Move to location
    Flee,               // Run away from threat
    Approach,           // Approach entity
    Follow,             // Follow entity
    Patrol,             // Patrol path
    Wander,             // Wander randomly

    // Combat
    Attack,             // Attack target
    Defend,             // Defensive stance
    UseAbility,         // Use combat ability
    Retreat,            // Tactical retreat
    CallForHelp,        // Alert allies

    // Social
    Greet,              // Greet entity
    Talk,               // Initiate conversation
    Trade,              // Initiate trade
    GiveItem,           // Give item to entity
    RequestItem,        // Ask for item
    Insult,             // Hostile speech
    Compliment,         // Friendly speech

    // Work
    Work,               // Perform job
    Rest,               // Take a break
    Sleep,              // Go to sleep
    Eat,                // Find food
    Drink,              // Find drink

    // Emotional
    Express,            // Express emotion
    Suppress,           // Suppress emotion
    Calm,               // Calm down

    // Cognitive
    Remember,           // Create memory
    Forget,             // Remove memory
    Learn,              // Learn new fact
    UpdateBelief,       // Modify belief

    // Goal
    SetGoal,            // Add new goal
    AbandonGoal,        // Remove goal
    PrioritizeGoal,     // Change goal priority

    // Custom
    CustomAction        // User-defined action
};

/**
 * Condition - Part of a rule's premise
 */
struct Condition {
    ConditionType type;
    std::string subject;        // "self", entity ID, or variable
    std::string target;         // Target entity or value
    double threshold = 0.0;     // For numeric comparisons
    std::string comparison;     // "==", "!=", ">", "<", ">=", "<="
    bool negated = false;

    // For custom conditions
    std::function<bool(const std::string& npcId)> customEvaluator;

    /**
     * Evaluate condition
     */
    bool evaluate(const std::string& npcId,
                 class RuleContext& context) const;
};

/**
 * Action - Part of a rule's consequence
 */
struct Action {
    ActionType type;
    std::string target;         // Target entity or location
    std::string parameter;      // Additional parameter
    double value = 0.0;         // Numeric parameter
    int priority = 0;           // Execution priority

    // For custom actions
    std::function<void(const std::string& npcId)> customExecutor;

    /**
     * Execute action
     */
    void execute(const std::string& npcId,
                class RuleContext& context) const;
};

/**
 * Behavior Rule
 */
struct BehaviorRule {
    std::string id;
    std::string name;
    std::string category;       // "survival", "social", "work", "combat", etc.
    std::string description;

    std::vector<Condition> conditions;  // All must be true (conjunction)
    std::vector<Action> actions;        // Actions to execute

    int priority = 0;           // Higher priority rules checked first
    double weight = 1.0;        // For probabilistic selection
    int cooldown = 0;           // Game ticks before can fire again

    // Personality requirements
    std::map<std::string, std::pair<double, double>> personalityBounds;

    // Context flags
    bool requiresConsciousness = true;
    bool interruptible = true;
    bool repeatable = true;

    /**
     * Check if rule can fire
     */
    bool canFire(const std::string& npcId,
                RuleContext& context) const;
};

/**
 * Rule Context - Provides access to world state for rule evaluation
 */
class RuleContext {
public:
    using StateGetter = std::function<double(const std::string& npcId,
                                             const std::string& key)>;
    using StateSetter = std::function<void(const std::string& npcId,
                                           const std::string& key,
                                           double value)>;
    using FactChecker = std::function<bool(const std::string& subject,
                                           const std::string& predicate,
                                           const std::string& object)>;

    RuleContext();
    ~RuleContext();

    /**
     * Register state accessors
     */
    void registerStateGetter(const std::string& stateType, StateGetter getter);
    void registerStateSetter(const std::string& stateType, StateSetter setter);
    void registerFactChecker(FactChecker checker);

    /**
     * State access
     */
    double getState(const std::string& npcId, const std::string& key) const;
    void setState(const std::string& npcId, const std::string& key, double value);

    /**
     * Fact checking
     */
    bool checkFact(const std::string& subject,
                  const std::string& predicate,
                  const std::string& object) const;

    /**
     * Time management
     */
    uint32_t getCurrentTime() const { return currentTime_; }
    void setCurrentTime(uint32_t time) { currentTime_ = time; }

    /**
     * Cooldown tracking
     */
    bool isOnCooldown(const std::string& npcId, const std::string& ruleId) const;
    void setCooldown(const std::string& npcId, const std::string& ruleId, int duration);
    void updateCooldowns(int deltaTicks);

private:
    std::map<std::string, StateGetter> stateGetters_;
    std::map<std::string, StateSetter> stateSetters_;
    FactChecker factChecker_;
    uint32_t currentTime_ = 0;

    // Cooldown tracking: npcId -> ruleId -> remaining ticks
    std::map<std::string, std::map<std::string, int>> cooldowns_;
};

/**
 * Rule Set - Collection of behavior rules
 */
class RuleSet {
public:
    RuleSet();
    ~RuleSet();

    /**
     * Add rule
     */
    void addRule(const BehaviorRule& rule);

    /**
     * Remove rule
     */
    void removeRule(const std::string& ruleId);

    /**
     * Get rule by ID
     */
    const BehaviorRule* getRule(const std::string& ruleId) const;

    /**
     * Get rules by category
     */
    std::vector<const BehaviorRule*> getRulesByCategory(const std::string& category) const;

    /**
     * Find applicable rules for NPC
     */
    std::vector<const BehaviorRule*> findApplicableRules(
        const std::string& npcId,
        RuleContext& context) const;

    /**
     * Select best rule (highest priority, weighted random among ties)
     */
    const BehaviorRule* selectBestRule(
        const std::string& npcId,
        RuleContext& context) const;

    /**
     * Execute rule actions
     */
    void executeRule(const BehaviorRule& rule,
                    const std::string& npcId,
                    RuleContext& context);

    /**
     * Load rules from file
     */
    bool loadFromFile(const std::string& filename);

    /**
     * Save rules to file
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * Get all rule IDs
     */
    std::vector<std::string> getRuleIds() const;

    /**
     * Clear all rules
     */
    void clear();

private:
    std::map<std::string, BehaviorRule> rules_;
    std::multimap<std::string, std::string> categoryIndex_;  // category -> rule IDs
};

/**
 * Default Rule Definitions - Built-in behavior rules
 */
class DefaultRules {
public:
    /**
     * Get survival rules
     */
    static std::vector<BehaviorRule> getSurvivalRules();

    /**
     * Get social rules
     */
    static std::vector<BehaviorRule> getSocialRules();

    /**
     * Get combat rules
     */
    static std::vector<BehaviorRule> getCombatRules();

    /**
     * Get work/schedule rules
     */
    static std::vector<BehaviorRule> getWorkRules();

    /**
     * Get emotional response rules
     */
    static std::vector<BehaviorRule> getEmotionalRules();

    /**
     * Get all default rules
     */
    static std::vector<BehaviorRule> getAllDefaultRules();
};

/**
 * Rule Builder - Fluent interface for creating rules
 */
class RuleBuilder {
public:
    RuleBuilder(const std::string& id);

    RuleBuilder& name(const std::string& name);
    RuleBuilder& category(const std::string& category);
    RuleBuilder& description(const std::string& desc);
    RuleBuilder& priority(int p);
    RuleBuilder& weight(double w);
    RuleBuilder& cooldown(int ticks);

    // Conditions
    RuleBuilder& when(ConditionType type, const std::string& target = "",
                     double threshold = 0.0, const std::string& comparison = ">=");
    RuleBuilder& whenNot(ConditionType type, const std::string& target = "",
                        double threshold = 0.0);
    RuleBuilder& whenHasItem(const std::string& item);
    RuleBuilder& whenHealthBelow(double threshold);
    RuleBuilder& whenNear(const std::string& entity, double distance = 5.0);
    RuleBuilder& whenRelationAbove(const std::string& entity, double threshold);
    RuleBuilder& whenEmotionAbove(const std::string& emotion, double threshold);
    RuleBuilder& whenTraitAbove(const std::string& trait, double threshold);
    RuleBuilder& whenTimeOfDay(int startHour, int endHour);

    // Actions
    RuleBuilder& then(ActionType type, const std::string& target = "",
                     double value = 0.0);
    RuleBuilder& thenMoveTo(const std::string& location);
    RuleBuilder& thenAttack(const std::string& target);
    RuleBuilder& thenFlee(const std::string& threat);
    RuleBuilder& thenTalk(const std::string& entity);
    RuleBuilder& thenExpress(const std::string& emotion, double intensity);
    RuleBuilder& thenSetGoal(const std::string& goal, double priority);

    // Personality requirements
    RuleBuilder& requiresTrait(const std::string& trait, double min, double max);

    // Flags
    RuleBuilder& requiresConsciousness(bool req);
    RuleBuilder& interruptible(bool inter);
    RuleBuilder& repeatable(bool rep);

    /**
     * Build the rule
     */
    BehaviorRule build();

private:
    BehaviorRule rule_;
};

/**
 * Rule Engine - Processes rules for NPCs
 */
class RuleEngine {
public:
    RuleEngine();
    ~RuleEngine();

    /**
     * Initialize with rule set
     */
    void initialize(std::shared_ptr<RuleSet> ruleSet);

    /**
     * Set rule context
     */
    void setContext(std::shared_ptr<RuleContext> context);

    /**
     * Process NPC - find and execute applicable rules
     */
    std::vector<Action> process(const std::string& npcId);

    /**
     * Process with specific categories only
     */
    std::vector<Action> processCategory(const std::string& npcId,
                                       const std::string& category);

    /**
     * Update (call each tick)
     */
    void update(int deltaTicks);

    /**
     * Get last executed rule for NPC
     */
    const BehaviorRule* getLastRule(const std::string& npcId) const;

    /**
     * Statistics
     */
    uint32_t getRuleExecutionCount(const std::string& ruleId) const;
    std::vector<std::pair<std::string, uint32_t>> getTopRules(int count = 10) const;

private:
    std::shared_ptr<RuleSet> ruleSet_;
    std::shared_ptr<RuleContext> context_;

    std::map<std::string, const BehaviorRule*> lastRules_;
    std::map<std::string, uint32_t> executionCounts_;
};

} // namespace Reasoning
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_RULE_SET_H
