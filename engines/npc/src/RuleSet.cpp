/**
 * RuleSet.cpp - Base Rule Set for NPC Behavior Implementation
 */

#include "reasoning/RuleSet.h"
#include <random>
#include <algorithm>
#include <fstream>

namespace Ultima {
namespace NPC {
namespace Reasoning {

static std::mt19937 rng(std::random_device{}());

//=============================================================================
// Condition Implementation
//=============================================================================

bool Condition::evaluate(const std::string& npcId, RuleContext& context) const {
    bool result = false;

    switch (type) {
        case ConditionType::HasItem:
            result = context.getState(npcId, "has_" + target) > 0;
            break;

        case ConditionType::HasGold:
            result = context.getState(npcId, "gold") >= threshold;
            break;

        case ConditionType::HealthBelow:
            result = context.getState(npcId, "health") < threshold;
            break;

        case ConditionType::HealthAbove:
            result = context.getState(npcId, "health") > threshold;
            break;

        case ConditionType::StaminaBelow:
            result = context.getState(npcId, "stamina") < threshold;
            break;

        case ConditionType::AtLocation:
            result = context.getState(npcId, "location_" + target) > 0;
            break;

        case ConditionType::RelationshipAbove:
            result = context.getState(npcId, "rel_" + target) > threshold;
            break;

        case ConditionType::RelationshipBelow:
            result = context.getState(npcId, "rel_" + target) < threshold;
            break;

        case ConditionType::EmotionAbove:
            result = context.getState(npcId, "emotion_" + target) > threshold;
            break;

        case ConditionType::TraitAbove:
            result = context.getState(npcId, "trait_" + target) > threshold;
            break;

        case ConditionType::TraitBelow:
            result = context.getState(npcId, "trait_" + target) < threshold;
            break;

        case ConditionType::KnowsFact:
            result = context.checkFact(npcId, "knows", target);
            break;

        case ConditionType::TimeOfDay:
            // Check if current time is within range
            {
                uint32_t hour = (context.getCurrentTime() / 60) % 24;
                // threshold encodes start hour, target parses to end hour
                result = hour >= static_cast<uint32_t>(threshold);
            }
            break;

        case ConditionType::InCombat:
            result = context.getState(npcId, "in_combat") > 0;
            break;

        case ConditionType::CustomCondition:
            if (customEvaluator) {
                result = customEvaluator(npcId);
            }
            break;

        default:
            result = false;
    }

    return negated ? !result : result;
}

//=============================================================================
// Action Implementation
//=============================================================================

void Action::execute(const std::string& npcId, RuleContext& context) const {
    switch (type) {
        case ActionType::MoveTo:
            context.setState(npcId, "target_location", 1.0);
            break;

        case ActionType::Attack:
            context.setState(npcId, "attack_target", 1.0);
            break;

        case ActionType::Flee:
            context.setState(npcId, "flee_from", 1.0);
            break;

        case ActionType::Greet:
        case ActionType::Talk:
            context.setState(npcId, "dialogue_target", 1.0);
            break;

        case ActionType::Trade:
            context.setState(npcId, "trade_target", 1.0);
            break;

        case ActionType::Express:
            context.setState(npcId, "emotion_" + target, value);
            break;

        case ActionType::SetGoal:
            context.setState(npcId, "goal_" + target, value);
            break;

        case ActionType::Rest:
            context.setState(npcId, "resting", 1.0);
            break;

        case ActionType::Work:
            context.setState(npcId, "working", 1.0);
            break;

        case ActionType::CustomAction:
            if (customExecutor) {
                customExecutor(npcId);
            }
            break;

        default:
            break;
    }
}

//=============================================================================
// BehaviorRule Implementation
//=============================================================================

bool BehaviorRule::canFire(const std::string& npcId, RuleContext& context) const {
    // Check cooldown
    if (context.isOnCooldown(npcId, id)) {
        return false;
    }

    // Check all conditions
    for (const auto& condition : conditions) {
        if (!condition.evaluate(npcId, context)) {
            return false;
        }
    }

    // Check personality requirements
    for (const auto& [trait, bounds] : personalityBounds) {
        double traitValue = context.getState(npcId, "trait_" + trait);
        if (traitValue < bounds.first || traitValue > bounds.second) {
            return false;
        }
    }

    return true;
}

//=============================================================================
// RuleContext Implementation
//=============================================================================

RuleContext::RuleContext() = default;
RuleContext::~RuleContext() = default;

void RuleContext::registerStateGetter(const std::string& stateType, StateGetter getter) {
    stateGetters_[stateType] = getter;
}

void RuleContext::registerStateSetter(const std::string& stateType, StateSetter setter) {
    stateSetters_[stateType] = setter;
}

void RuleContext::registerFactChecker(FactChecker checker) {
    factChecker_ = checker;
}

double RuleContext::getState(const std::string& npcId, const std::string& key) const {
    // Try specific getter first
    for (const auto& [prefix, getter] : stateGetters_) {
        if (key.find(prefix) == 0) {
            return getter(npcId, key);
        }
    }
    return 0.0;
}

void RuleContext::setState(const std::string& npcId, const std::string& key, double value) {
    for (const auto& [prefix, setter] : stateSetters_) {
        if (key.find(prefix) == 0) {
            setter(npcId, key, value);
            return;
        }
    }
}

bool RuleContext::checkFact(const std::string& subject,
                           const std::string& predicate,
                           const std::string& object) const {
    if (factChecker_) {
        return factChecker_(subject, predicate, object);
    }
    return false;
}

bool RuleContext::isOnCooldown(const std::string& npcId, const std::string& ruleId) const {
    auto npcIt = cooldowns_.find(npcId);
    if (npcIt == cooldowns_.end()) return false;

    auto ruleIt = npcIt->second.find(ruleId);
    if (ruleIt == npcIt->second.end()) return false;

    return ruleIt->second > 0;
}

void RuleContext::setCooldown(const std::string& npcId,
                             const std::string& ruleId,
                             int duration) {
    cooldowns_[npcId][ruleId] = duration;
}

void RuleContext::updateCooldowns(int deltaTicks) {
    for (auto& [npcId, ruleCooldowns] : cooldowns_) {
        for (auto& [ruleId, remaining] : ruleCooldowns) {
            remaining = std::max(0, remaining - deltaTicks);
        }
    }
}

//=============================================================================
// RuleSet Implementation
//=============================================================================

RuleSet::RuleSet() = default;
RuleSet::~RuleSet() = default;

void RuleSet::addRule(const BehaviorRule& rule) {
    rules_[rule.id] = rule;
    categoryIndex_.insert({rule.category, rule.id});
}

void RuleSet::removeRule(const std::string& ruleId) {
    auto it = rules_.find(ruleId);
    if (it != rules_.end()) {
        // Remove from category index
        auto range = categoryIndex_.equal_range(it->second.category);
        for (auto catIt = range.first; catIt != range.second; ) {
            if (catIt->second == ruleId) {
                catIt = categoryIndex_.erase(catIt);
            } else {
                ++catIt;
            }
        }
        rules_.erase(it);
    }
}

const BehaviorRule* RuleSet::getRule(const std::string& ruleId) const {
    auto it = rules_.find(ruleId);
    return it != rules_.end() ? &it->second : nullptr;
}

std::vector<const BehaviorRule*> RuleSet::getRulesByCategory(const std::string& category) const {
    std::vector<const BehaviorRule*> results;
    auto range = categoryIndex_.equal_range(category);
    for (auto it = range.first; it != range.second; ++it) {
        results.push_back(&rules_.at(it->second));
    }
    return results;
}

std::vector<const BehaviorRule*> RuleSet::findApplicableRules(
    const std::string& npcId,
    RuleContext& context) const
{
    std::vector<const BehaviorRule*> applicable;

    for (const auto& [id, rule] : rules_) {
        if (rule.canFire(npcId, context)) {
            applicable.push_back(&rule);
        }
    }

    // Sort by priority
    std::sort(applicable.begin(), applicable.end(),
              [](const BehaviorRule* a, const BehaviorRule* b) {
                  return a->priority > b->priority;
              });

    return applicable;
}

const BehaviorRule* RuleSet::selectBestRule(const std::string& npcId,
                                            RuleContext& context) const {
    auto applicable = findApplicableRules(npcId, context);
    if (applicable.empty()) return nullptr;

    // Find highest priority
    int maxPriority = applicable[0]->priority;
    std::vector<const BehaviorRule*> topPriority;

    for (const auto* rule : applicable) {
        if (rule->priority == maxPriority) {
            topPriority.push_back(rule);
        }
    }

    // Weighted random selection among ties
    if (topPriority.size() == 1) {
        return topPriority[0];
    }

    double totalWeight = 0.0;
    for (const auto* rule : topPriority) {
        totalWeight += rule->weight;
    }

    std::uniform_real_distribution<double> dist(0.0, totalWeight);
    double r = dist(rng);

    double cumulative = 0.0;
    for (const auto* rule : topPriority) {
        cumulative += rule->weight;
        if (r <= cumulative) {
            return rule;
        }
    }

    return topPriority[0];
}

void RuleSet::executeRule(const BehaviorRule& rule,
                         const std::string& npcId,
                         RuleContext& context) {
    for (const auto& action : rule.actions) {
        action.execute(npcId, context);
    }

    if (rule.cooldown > 0) {
        context.setCooldown(npcId, rule.id, rule.cooldown);
    }
}

bool RuleSet::loadFromFile(const std::string& filename) {
    // Load rules from JSON file
    return true;
}

bool RuleSet::saveToFile(const std::string& filename) const {
    // Save rules to JSON file
    return true;
}

std::vector<std::string> RuleSet::getRuleIds() const {
    std::vector<std::string> ids;
    for (const auto& [id, _] : rules_) {
        ids.push_back(id);
    }
    return ids;
}

void RuleSet::clear() {
    rules_.clear();
    categoryIndex_.clear();
}

//=============================================================================
// DefaultRules Implementation
//=============================================================================

std::vector<BehaviorRule> DefaultRules::getSurvivalRules() {
    std::vector<BehaviorRule> rules;

    // Flee when health low
    rules.push_back(
        RuleBuilder("survival_flee_low_health")
            .name("Flee when health critical")
            .category("survival")
            .priority(100)
            .whenHealthBelow(0.2)
            .when(ConditionType::InCombat, "true")
            .then(ActionType::Flee)
            .build()
    );

    // Seek healing
    rules.push_back(
        RuleBuilder("survival_seek_healing")
            .name("Seek healing")
            .category("survival")
            .priority(90)
            .whenHealthBelow(0.5)
            .then(ActionType::MoveTo, "healer")
            .build()
    );

    // Rest when tired
    rules.push_back(
        RuleBuilder("survival_rest")
            .name("Rest when tired")
            .category("survival")
            .priority(50)
            .when(ConditionType::StaminaBelow, "", 0.3)
            .then(ActionType::Rest)
            .build()
    );

    return rules;
}

std::vector<BehaviorRule> DefaultRules::getSocialRules() {
    std::vector<BehaviorRule> rules;

    // Greet friends
    rules.push_back(
        RuleBuilder("social_greet_friend")
            .name("Greet friend")
            .category("social")
            .priority(30)
            .when(ConditionType::NearEntity, "friend")
            .whenRelationAbove("friend", 0.5)
            .then(ActionType::Greet)
            .cooldown(100)
            .build()
    );

    // Avoid enemies
    rules.push_back(
        RuleBuilder("social_avoid_enemy")
            .name("Avoid enemy")
            .category("social")
            .priority(60)
            .when(ConditionType::NearEntity, "enemy")
            .whenRelationAbove("enemy", -0.5)
            .then(ActionType::MoveTo, "away")
            .build()
    );

    return rules;
}

std::vector<BehaviorRule> DefaultRules::getCombatRules() {
    std::vector<BehaviorRule> rules;

    // Attack enemy in range
    rules.push_back(
        RuleBuilder("combat_attack")
            .name("Attack enemy")
            .category("combat")
            .priority(80)
            .when(ConditionType::InCombat, "true")
            .when(ConditionType::NearEntity, "enemy")
            .then(ActionType::Attack)
            .build()
    );

    // Defend when overwhelmed
    rules.push_back(
        RuleBuilder("combat_defend")
            .name("Defensive stance")
            .category("combat")
            .priority(70)
            .when(ConditionType::InCombat, "true")
            .whenHealthBelow(0.4)
            .then(ActionType::Defend)
            .requiresTrait("aggression", 0.0, 0.6)
            .build()
    );

    return rules;
}

std::vector<BehaviorRule> DefaultRules::getWorkRules() {
    std::vector<BehaviorRule> rules;

    // Go to work
    rules.push_back(
        RuleBuilder("work_go_to_work")
            .name("Go to work")
            .category("work")
            .priority(40)
            .whenTimeOfDay(6, 18)
            .then(ActionType::Work)
            .build()
    );

    // Go home at night
    rules.push_back(
        RuleBuilder("work_go_home")
            .name("Go home")
            .category("work")
            .priority(40)
            .whenTimeOfDay(18, 6)
            .thenMoveTo("home")
            .build()
    );

    return rules;
}

std::vector<BehaviorRule> DefaultRules::getEmotionalRules() {
    std::vector<BehaviorRule> rules;

    // Express happiness
    rules.push_back(
        RuleBuilder("emotion_express_happy")
            .name("Express happiness")
            .category("emotional")
            .priority(20)
            .whenEmotionAbove("happiness", 0.7)
            .thenExpress("happy", 0.5)
            .cooldown(50)
            .build()
    );

    // Calm down when angry
    rules.push_back(
        RuleBuilder("emotion_calm_anger")
            .name("Calm down")
            .category("emotional")
            .priority(25)
            .whenEmotionAbove("anger", 0.6)
            .requiresTrait("neuroticism", 0.0, 0.5)
            .then(ActionType::Calm)
            .build()
    );

    return rules;
}

std::vector<BehaviorRule> DefaultRules::getAllDefaultRules() {
    std::vector<BehaviorRule> all;

    auto survival = getSurvivalRules();
    auto social = getSocialRules();
    auto combat = getCombatRules();
    auto work = getWorkRules();
    auto emotional = getEmotionalRules();

    all.insert(all.end(), survival.begin(), survival.end());
    all.insert(all.end(), social.begin(), social.end());
    all.insert(all.end(), combat.begin(), combat.end());
    all.insert(all.end(), work.begin(), work.end());
    all.insert(all.end(), emotional.begin(), emotional.end());

    return all;
}

//=============================================================================
// RuleBuilder Implementation
//=============================================================================

RuleBuilder::RuleBuilder(const std::string& id) {
    rule_.id = id;
}

RuleBuilder& RuleBuilder::name(const std::string& n) {
    rule_.name = n;
    return *this;
}

RuleBuilder& RuleBuilder::category(const std::string& cat) {
    rule_.category = cat;
    return *this;
}

RuleBuilder& RuleBuilder::description(const std::string& desc) {
    rule_.description = desc;
    return *this;
}

RuleBuilder& RuleBuilder::priority(int p) {
    rule_.priority = p;
    return *this;
}

RuleBuilder& RuleBuilder::weight(double w) {
    rule_.weight = w;
    return *this;
}

RuleBuilder& RuleBuilder::cooldown(int ticks) {
    rule_.cooldown = ticks;
    return *this;
}

RuleBuilder& RuleBuilder::when(ConditionType type, const std::string& target,
                               double threshold, const std::string& comparison) {
    Condition c;
    c.type = type;
    c.target = target;
    c.threshold = threshold;
    c.comparison = comparison;
    rule_.conditions.push_back(c);
    return *this;
}

RuleBuilder& RuleBuilder::whenNot(ConditionType type, const std::string& target,
                                  double threshold) {
    Condition c;
    c.type = type;
    c.target = target;
    c.threshold = threshold;
    c.negated = true;
    rule_.conditions.push_back(c);
    return *this;
}

RuleBuilder& RuleBuilder::whenHasItem(const std::string& item) {
    return when(ConditionType::HasItem, item);
}

RuleBuilder& RuleBuilder::whenHealthBelow(double threshold) {
    return when(ConditionType::HealthBelow, "", threshold);
}

RuleBuilder& RuleBuilder::whenNear(const std::string& entity, double distance) {
    return when(ConditionType::NearEntity, entity, distance);
}

RuleBuilder& RuleBuilder::whenRelationAbove(const std::string& entity, double threshold) {
    return when(ConditionType::RelationshipAbove, entity, threshold);
}

RuleBuilder& RuleBuilder::whenEmotionAbove(const std::string& emotion, double threshold) {
    return when(ConditionType::EmotionAbove, emotion, threshold);
}

RuleBuilder& RuleBuilder::whenTraitAbove(const std::string& trait, double threshold) {
    return when(ConditionType::TraitAbove, trait, threshold);
}

RuleBuilder& RuleBuilder::whenTimeOfDay(int startHour, int endHour) {
    return when(ConditionType::TimeOfDay, std::to_string(endHour), startHour);
}

RuleBuilder& RuleBuilder::then(ActionType type, const std::string& target,
                               double value) {
    Action a;
    a.type = type;
    a.target = target;
    a.value = value;
    rule_.actions.push_back(a);
    return *this;
}

RuleBuilder& RuleBuilder::thenMoveTo(const std::string& location) {
    return then(ActionType::MoveTo, location);
}

RuleBuilder& RuleBuilder::thenAttack(const std::string& target) {
    return then(ActionType::Attack, target);
}

RuleBuilder& RuleBuilder::thenFlee(const std::string& threat) {
    return then(ActionType::Flee, threat);
}

RuleBuilder& RuleBuilder::thenTalk(const std::string& entity) {
    return then(ActionType::Talk, entity);
}

RuleBuilder& RuleBuilder::thenExpress(const std::string& emotion, double intensity) {
    return then(ActionType::Express, emotion, intensity);
}

RuleBuilder& RuleBuilder::thenSetGoal(const std::string& goal, double priority) {
    return then(ActionType::SetGoal, goal, priority);
}

RuleBuilder& RuleBuilder::requiresTrait(const std::string& trait, double min, double max) {
    rule_.personalityBounds[trait] = {min, max};
    return *this;
}

RuleBuilder& RuleBuilder::requiresConsciousness(bool req) {
    rule_.requiresConsciousness = req;
    return *this;
}

RuleBuilder& RuleBuilder::interruptible(bool inter) {
    rule_.interruptible = inter;
    return *this;
}

RuleBuilder& RuleBuilder::repeatable(bool rep) {
    rule_.repeatable = rep;
    return *this;
}

BehaviorRule RuleBuilder::build() {
    return rule_;
}

//=============================================================================
// RuleEngine Implementation
//=============================================================================

RuleEngine::RuleEngine() = default;
RuleEngine::~RuleEngine() = default;

void RuleEngine::initialize(std::shared_ptr<RuleSet> ruleSet) {
    ruleSet_ = ruleSet;
}

void RuleEngine::setContext(std::shared_ptr<RuleContext> context) {
    context_ = context;
}

std::vector<Action> RuleEngine::process(const std::string& npcId) {
    std::vector<Action> actions;

    if (!ruleSet_ || !context_) return actions;

    const BehaviorRule* rule = ruleSet_->selectBestRule(npcId, *context_);
    if (rule) {
        actions = rule->actions;
        ruleSet_->executeRule(*rule, npcId, *context_);
        lastRules_[npcId] = rule;
        executionCounts_[rule->id]++;
    }

    return actions;
}

std::vector<Action> RuleEngine::processCategory(const std::string& npcId,
                                                const std::string& category) {
    std::vector<Action> actions;

    if (!ruleSet_ || !context_) return actions;

    auto categoryRules = ruleSet_->getRulesByCategory(category);

    for (const auto* rule : categoryRules) {
        if (rule->canFire(npcId, *context_)) {
            actions.insert(actions.end(), rule->actions.begin(), rule->actions.end());
            ruleSet_->executeRule(*rule, npcId, *context_);
            lastRules_[npcId] = rule;
            executionCounts_[rule->id]++;
            break;
        }
    }

    return actions;
}

void RuleEngine::update(int deltaTicks) {
    if (context_) {
        context_->updateCooldowns(deltaTicks);
    }
}

const BehaviorRule* RuleEngine::getLastRule(const std::string& npcId) const {
    auto it = lastRules_.find(npcId);
    return it != lastRules_.end() ? it->second : nullptr;
}

uint32_t RuleEngine::getRuleExecutionCount(const std::string& ruleId) const {
    auto it = executionCounts_.find(ruleId);
    return it != executionCounts_.end() ? it->second : 0;
}

std::vector<std::pair<std::string, uint32_t>> RuleEngine::getTopRules(int count) const {
    std::vector<std::pair<std::string, uint32_t>> results(
        executionCounts_.begin(), executionCounts_.end());

    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    if (results.size() > static_cast<size_t>(count)) {
        results.resize(count);
    }

    return results;
}

} // namespace Reasoning
} // namespace NPC
} // namespace Ultima
