/**
 * Persona.cpp - NPC Persona and Personality Implementation
 *
 * Comprehensive personality system for NPCs.
 */

#include "persona/Persona.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <sstream>
#include <fstream>

namespace Ultima {
namespace NPC {
namespace Persona {

static std::mt19937 rng(std::random_device{}());

//=============================================================================
// BigFiveTraits Implementation
//=============================================================================

void BigFiveTraits::calculateOverallScores() {
    // Openness from facets
    openness = (intellectualCuriosity + artisticInterest) / 2.0;

    // Conscientiousness from facets
    conscientiousness = (orderliness + dutifulness + achievementStriving + selfDiscipline) / 4.0;

    // Extraversion from facets
    extraversion = (warmth + gregariousness + assertiveness + activityLevel) / 4.0;

    // Agreeableness from facets
    agreeableness = (trust + altruism + compliance + modesty) / 4.0;

    // Neuroticism from facets
    neuroticism = (anxiety + hostility + depression + selfConsciousness +
                   impulsiveness + vulnerability) / 6.0;
}

void BigFiveTraits::applyVariation(double maxVariation) {
    std::uniform_real_distribution<double> dist(-maxVariation, maxVariation);

    auto clamp = [](double v) { return std::max(0.0, std::min(1.0, v)); };

    openness = clamp(openness + dist(rng));
    intellectualCuriosity = clamp(intellectualCuriosity + dist(rng));
    artisticInterest = clamp(artisticInterest + dist(rng));

    conscientiousness = clamp(conscientiousness + dist(rng));
    orderliness = clamp(orderliness + dist(rng));
    dutifulness = clamp(dutifulness + dist(rng));
    achievementStriving = clamp(achievementStriving + dist(rng));
    selfDiscipline = clamp(selfDiscipline + dist(rng));

    extraversion = clamp(extraversion + dist(rng));
    warmth = clamp(warmth + dist(rng));
    gregariousness = clamp(gregariousness + dist(rng));
    assertiveness = clamp(assertiveness + dist(rng));
    activityLevel = clamp(activityLevel + dist(rng));

    agreeableness = clamp(agreeableness + dist(rng));
    trust = clamp(trust + dist(rng));
    altruism = clamp(altruism + dist(rng));
    compliance = clamp(compliance + dist(rng));
    modesty = clamp(modesty + dist(rng));

    neuroticism = clamp(neuroticism + dist(rng));
    anxiety = clamp(anxiety + dist(rng));
    hostility = clamp(hostility + dist(rng));
    depression = clamp(depression + dist(rng));
    selfConsciousness = clamp(selfConsciousness + dist(rng));
    impulsiveness = clamp(impulsiveness + dist(rng));
    vulnerability = clamp(vulnerability + dist(rng));
}

BigFiveTraits BigFiveTraits::blend(const BigFiveTraits& other, double weight) const {
    BigFiveTraits result;
    double w1 = 1.0 - weight;
    double w2 = weight;

    result.openness = w1 * openness + w2 * other.openness;
    result.intellectualCuriosity = w1 * intellectualCuriosity + w2 * other.intellectualCuriosity;
    result.artisticInterest = w1 * artisticInterest + w2 * other.artisticInterest;

    result.conscientiousness = w1 * conscientiousness + w2 * other.conscientiousness;
    result.orderliness = w1 * orderliness + w2 * other.orderliness;
    result.dutifulness = w1 * dutifulness + w2 * other.dutifulness;
    result.achievementStriving = w1 * achievementStriving + w2 * other.achievementStriving;
    result.selfDiscipline = w1 * selfDiscipline + w2 * other.selfDiscipline;

    result.extraversion = w1 * extraversion + w2 * other.extraversion;
    result.warmth = w1 * warmth + w2 * other.warmth;
    result.gregariousness = w1 * gregariousness + w2 * other.gregariousness;
    result.assertiveness = w1 * assertiveness + w2 * other.assertiveness;
    result.activityLevel = w1 * activityLevel + w2 * other.activityLevel;

    result.agreeableness = w1 * agreeableness + w2 * other.agreeableness;
    result.trust = w1 * trust + w2 * other.trust;
    result.altruism = w1 * altruism + w2 * other.altruism;
    result.compliance = w1 * compliance + w2 * other.compliance;
    result.modesty = w1 * modesty + w2 * other.modesty;

    result.neuroticism = w1 * neuroticism + w2 * other.neuroticism;
    result.anxiety = w1 * anxiety + w2 * other.anxiety;
    result.hostility = w1 * hostility + w2 * other.hostility;
    result.depression = w1 * depression + w2 * other.depression;
    result.selfConsciousness = w1 * selfConsciousness + w2 * other.selfConsciousness;
    result.impulsiveness = w1 * impulsiveness + w2 * other.impulsiveness;
    result.vulnerability = w1 * vulnerability + w2 * other.vulnerability;

    return result;
}

//=============================================================================
// EmotionalState Implementation
//=============================================================================

EmotionType EmotionalState::getDominantEmotion() const {
    EmotionType dominant = EmotionType::Happiness;
    double maxIntensity = 0.0;

    for (const auto& [emotion, intensity] : emotions) {
        if (intensity > maxIntensity) {
            maxIntensity = intensity;
            dominant = emotion;
        }
    }

    return dominant;
}

void EmotionalState::applyEmotion(EmotionType emotion, double intensity, double duration) {
    // Blend with existing emotion
    double current = emotions.count(emotion) ? emotions[emotion] : 0.0;
    emotions[emotion] = std::min(1.0, current + intensity * duration);

    // Update valence/arousal/dominance based on emotion
    switch (emotion) {
        case EmotionType::Happiness:
            overallValence += intensity * 0.3;
            arousal += intensity * 0.2;
            break;
        case EmotionType::Sadness:
            overallValence -= intensity * 0.3;
            arousal -= intensity * 0.2;
            break;
        case EmotionType::Anger:
            overallValence -= intensity * 0.2;
            arousal += intensity * 0.4;
            dominance += intensity * 0.3;
            break;
        case EmotionType::Fear:
            overallValence -= intensity * 0.3;
            arousal += intensity * 0.5;
            dominance -= intensity * 0.4;
            break;
        case EmotionType::Surprise:
            arousal += intensity * 0.5;
            break;
        case EmotionType::Trust:
            overallValence += intensity * 0.2;
            break;
        default:
            break;
    }

    // Clamp values
    overallValence = std::max(-1.0, std::min(1.0, overallValence));
    arousal = std::max(0.0, std::min(1.0, arousal));
    dominance = std::max(0.0, std::min(1.0, dominance));
}

void EmotionalState::decay(double deltaTime, double decayRate) {
    double decay = std::pow(1.0 - decayRate, deltaTime);

    for (auto& [emotion, intensity] : emotions) {
        intensity *= decay;
        if (intensity < 0.01) intensity = 0.0;
    }

    // Decay valence/arousal toward neutral
    overallValence *= decay;
    arousal = 0.5 + (arousal - 0.5) * decay;
    dominance = 0.5 + (dominance - 0.5) * decay;
}

void EmotionalState::updateMood(double deltaTime) {
    // Mood slowly tracks emotional valence
    double moodInfluence = 0.01 * deltaTime;
    moodValence = moodValence * (1.0 - moodInfluence) + overallValence * moodInfluence;

    // Update stress based on negative emotions and arousal
    double negativeEmotionSum = 0.0;
    negativeEmotionSum += emotions.count(EmotionType::Anger) ? emotions[EmotionType::Anger] : 0.0;
    negativeEmotionSum += emotions.count(EmotionType::Fear) ? emotions[EmotionType::Fear] : 0.0;
    negativeEmotionSum += emotions.count(EmotionType::Sadness) ? emotions[EmotionType::Sadness] : 0.0;

    stressLevel = std::min(1.0, negativeEmotionSum * 0.5 + arousal * 0.3);
}

//=============================================================================
// Relationship Implementation
//=============================================================================

double Relationship::getOverallScore() const {
    // Weighted combination of relationship factors
    return (trust * 0.3 + affection * 0.25 + respect * 0.25 + familiarity * 0.2) - fear * 0.2;
}

void Relationship::processInteraction(double trustDelta, double affectionDelta, double respectDelta) {
    trust = std::max(-1.0, std::min(1.0, trust + trustDelta));
    affection = std::max(-1.0, std::min(1.0, affection + affectionDelta));
    respect = std::max(-1.0, std::min(1.0, respect + respectDelta));

    // Familiarity increases with any interaction
    familiarity = std::min(1.0, familiarity + 0.02);
    interactionCount++;
}

//=============================================================================
// NPCPersona Implementation
//=============================================================================

NPCPersona::NPCPersona() = default;

NPCPersona::NPCPersona(const std::string& personaId)
    : id(personaId)
{
}

NPCPersona::~NPCPersona() = default;

void NPCPersona::update(double deltaTime) {
    // Update emotional state
    emotionalState.decay(deltaTime);
    emotionalState.updateMood(deltaTime);

    // Decay memories
    decayMemories(deltaTime);

    // Update goals
    updateGoals(deltaTime);
}

void NPCPersona::decayMemories(double deltaTime) {
    for (auto& memory : memories) {
        // Clarity decays over time
        memory.clarity *= (1.0 - 0.001 * deltaTime);

        // Important/emotional memories decay slower
        double decayModifier = 1.0 - (memory.importance * 0.3 +
                                      std::abs(memory.emotionalImpact) * 0.3);
        memory.clarity *= (1.0 - 0.0005 * deltaTime * decayModifier);

        // Accuracy can degrade
        if (memory.clarity < 0.5) {
            memory.accuracy *= (1.0 - 0.0002 * deltaTime);
        }
    }

    // Remove very faded memories
    memories.erase(
        std::remove_if(memories.begin(), memories.end(),
                      [](const Memory& m) { return m.clarity < 0.1; }),
        memories.end()
    );
}

void NPCPersona::updateGoals(double deltaTime) {
    for (auto& goal : goals) {
        if (!goal.isActive) continue;

        // Check deadline
        if (goal.hasDeadline && goal.deadlineGameTime > 0) {
            // Would need game time to check this properly
        }

        // Motivations influence goal priority
        if (motivationStrengths.count(goal.motivation)) {
            goal.priority = motivationStrengths[goal.motivation];
        }
    }
}

void NPCPersona::processInteraction(const std::string& entityId,
                                    const std::string& interactionType,
                                    double outcome)
{
    auto& relationship = getRelationship(entityId);

    // Adjust relationship based on interaction outcome
    double trustChange = outcome * 0.1;
    double affectionChange = outcome * 0.05;
    double respectChange = outcome * 0.05;

    // Interaction type modifiers
    if (interactionType == "trade") {
        trustChange *= 1.5;
    } else if (interactionType == "combat") {
        trustChange *= -0.5;
        affectionChange *= -1.0;
    } else if (interactionType == "help") {
        affectionChange *= 2.0;
        respectChange *= 1.5;
    } else if (interactionType == "betrayal") {
        trustChange = -0.5;
        affectionChange *= -2.0;
    }

    relationship.processInteraction(trustChange, affectionChange, respectChange);

    // Create memory of interaction
    Memory mem;
    mem.type = Memory::Type::Interaction;
    mem.description = interactionType + " with " + entityId;
    mem.relatedEntityId = entityId;
    mem.emotionalImpact = outcome;
    mem.importance = std::abs(outcome);
    addMemory(mem);

    // Emotional response
    if (outcome > 0.5) {
        emotionalState.applyEmotion(EmotionType::Happiness, outcome * 0.3);
    } else if (outcome < -0.5) {
        emotionalState.applyEmotion(EmotionType::Anger, std::abs(outcome) * 0.2);
        emotionalState.applyEmotion(EmotionType::Sadness, std::abs(outcome) * 0.1);
    }
}

void NPCPersona::addMemory(const Memory& memory) {
    memories.push_back(memory);

    // Sort by importance
    std::sort(memories.begin(), memories.end(),
              [](const Memory& a, const Memory& b) {
                  return a.importance > b.importance;
              });

    // Limit memory count
    const size_t MAX_MEMORIES = 100;
    if (memories.size() > MAX_MEMORIES) {
        memories.resize(MAX_MEMORIES);
    }
}

std::vector<Memory> NPCPersona::recallMemories(const std::string& context, int maxCount) {
    std::vector<Memory> relevant;

    for (auto& memory : memories) {
        // Check relevance to context
        bool isRelevant = false;

        if (memory.description.find(context) != std::string::npos) {
            isRelevant = true;
        }
        if (memory.relatedEntityId == context) {
            isRelevant = true;
        }

        if (isRelevant) {
            memory.lastRecalled = 0; // Would use game time
            relevant.push_back(memory);
        }
    }

    // Sort by importance and clarity
    std::sort(relevant.begin(), relevant.end(),
              [](const Memory& a, const Memory& b) {
                  return (a.importance * a.clarity) > (b.importance * b.clarity);
              });

    if (relevant.size() > static_cast<size_t>(maxCount)) {
        relevant.resize(maxCount);
    }

    return relevant;
}

Relationship& NPCPersona::getRelationship(const std::string& entityId) {
    if (!relationships.count(entityId)) {
        Relationship rel;
        rel.targetId = entityId;
        relationships[entityId] = rel;
    }
    return relationships[entityId];
}

std::pair<double, int> NPCPersona::makeDecision(
    const std::vector<std::string>& options,
    const std::vector<std::map<std::string, double>>& optionTraits)
{
    if (options.empty() || optionTraits.empty()) {
        return {0.0, 0};
    }

    std::vector<double> scores;
    for (const auto& traits : optionTraits) {
        scores.push_back(calculateDecisionWeight(traits));
    }

    // Find best option
    auto maxIt = std::max_element(scores.begin(), scores.end());
    int bestIndex = static_cast<int>(std::distance(scores.begin(), maxIt));

    // Add some randomness based on impulsiveness
    if (behavior.impulsiveness > 0.5) {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        if (dist(rng) < behavior.impulsiveness - 0.5) {
            // Random choice instead
            std::uniform_int_distribution<int> idist(0, static_cast<int>(options.size() - 1));
            bestIndex = idist(rng);
        }
    }

    // Calculate confidence
    double confidence = 0.5;
    if (scores.size() > 1) {
        std::sort(scores.begin(), scores.end(), std::greater<double>());
        confidence = (scores[0] - scores[1]) / (scores[0] + 0.001);
    }

    return {confidence, bestIndex};
}

double NPCPersona::calculateDecisionWeight(const std::map<std::string, double>& optionTraits) const {
    double weight = 0.5;

    // Personality trait influence
    for (const auto& [trait, value] : optionTraits) {
        if (trait == "risk") {
            weight += (behavior.riskTolerance - 0.5) * value;
        } else if (trait == "social") {
            weight += (traits.extraversion - 0.5) * value;
        } else if (trait == "moral") {
            weight += (traits.agreeableness - 0.5) * value;
        } else if (trait == "cautious") {
            weight += (traits.conscientiousness - 0.5) * value;
        } else if (trait == "creative") {
            weight += (traits.openness - 0.5) * value;
        } else if (trait == "aggressive") {
            weight += (behavior.aggression - 0.5) * value;
        }
    }

    // Emotional state influence
    weight += (emotionalState.overallValence * 0.1);
    weight -= (emotionalState.stressLevel * 0.2);

    return std::max(0.0, std::min(1.0, weight));
}

std::map<std::string, double> NPCPersona::getDialogueModifiers() const {
    std::map<std::string, double> modifiers;

    modifiers["formality"] = communication.formality;
    modifiers["verbosity"] = communication.verbosity;
    modifiers["warmth"] = traits.warmth;
    modifiers["directness"] = communication.directness;
    modifiers["humor"] = communication.humor;
    modifiers["emotion"] = communication.emotionalExpression;

    // Adjust based on emotional state
    modifiers["warmth"] += emotionalState.overallValence * 0.2;
    modifiers["emotion"] += emotionalState.arousal * 0.2;

    // Clamp all values
    for (auto& [key, value] : modifiers) {
        value = std::max(0.0, std::min(1.0, value));
    }

    return modifiers;
}

std::string NPCPersona::serialize() const {
    std::stringstream ss;

    ss << "id:" << id << "\n";
    ss << "name:" << name << "\n";
    ss << "age:" << age << "\n";
    ss << "gender:" << gender << "\n";

    // Traits
    ss << "openness:" << traits.openness << "\n";
    ss << "conscientiousness:" << traits.conscientiousness << "\n";
    ss << "extraversion:" << traits.extraversion << "\n";
    ss << "agreeableness:" << traits.agreeableness << "\n";
    ss << "neuroticism:" << traits.neuroticism << "\n";

    return ss.str();
}

bool NPCPersona::deserialize(const std::string& data) {
    std::istringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        if (key == "id") id = value;
        else if (key == "name") name = value;
        else if (key == "age") age = std::stoul(value);
        else if (key == "gender") gender = value;
        else if (key == "openness") traits.openness = std::stod(value);
        else if (key == "conscientiousness") traits.conscientiousness = std::stod(value);
        else if (key == "extraversion") traits.extraversion = std::stod(value);
        else if (key == "agreeableness") traits.agreeableness = std::stod(value);
        else if (key == "neuroticism") traits.neuroticism = std::stod(value);
    }

    return true;
}

std::unique_ptr<NPCPersona> NPCPersona::clone() const {
    auto copy = std::make_unique<NPCPersona>();
    *copy = *this;
    return copy;
}

void NPCPersona::randomize(double variationAmount) {
    traits.applyVariation(variationAmount);

    std::uniform_real_distribution<double> dist(0.0, 1.0);

    behavior.riskTolerance = dist(rng);
    behavior.impulsiveness = dist(rng);
    behavior.cooperativeness = dist(rng);
    behavior.aggression = dist(rng);
    behavior.generosity = dist(rng);

    communication.formality = dist(rng);
    communication.verbosity = dist(rng);
    communication.emotionalExpression = dist(rng);
}

//=============================================================================
// PersonaTemplate Implementation
//=============================================================================

std::unique_ptr<NPCPersona> PersonaTemplate::generate(const std::string& personaId) const {
    auto persona = std::make_unique<NPCPersona>(personaId);

    // Base traits with variation
    persona->traits = baseTraits;
    persona->traits.applyVariation(traitVariation);

    // Communication and behavior
    persona->communication = baseComm;
    persona->behavior = baseBehavior;

    // Random name
    if (!possibleNames.empty()) {
        std::uniform_int_distribution<size_t> dist(0, possibleNames.size() - 1);
        persona->name = possibleNames[dist(rng)];
    }

    // Random age
    std::uniform_int_distribution<int> ageDist(minAge, maxAge);
    persona->age = ageDist(rng);

    // Role
    persona->role = defaultRole;

    return persona;
}

//=============================================================================
// PersonaFactory Implementation
//=============================================================================

PersonaFactory::PersonaFactory() {
    // Default name lists
    maleFirstNames_ = {
        "John", "William", "Thomas", "Richard", "Robert",
        "Edward", "Henry", "George", "Arthur", "James",
        "Gareth", "Aldric", "Edmund", "Roland", "Baldwin"
    };

    femaleFirstNames_ = {
        "Mary", "Elizabeth", "Anne", "Margaret", "Catherine",
        "Eleanor", "Joan", "Alice", "Matilda", "Isabella",
        "Isolde", "Gwendolyn", "Beatrice", "Rosamund", "Edith"
    };

    surnames_ = {
        "Smith", "Baker", "Miller", "Cooper", "Fletcher",
        "Mason", "Weaver", "Turner", "Walker", "Carter",
        "Thatcher", "Shepherd", "Fisher", "Hunter", "Knight"
    };
}

PersonaFactory::~PersonaFactory() = default;

bool PersonaFactory::loadTemplates(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    // Simplified template loading
    // In practice, would parse a structured format (JSON/XML)
    return true;
}

void PersonaFactory::registerTemplate(const PersonaTemplate& templ) {
    templates_[templ.templateId] = templ;
}

std::unique_ptr<NPCPersona> PersonaFactory::createFromTemplate(
    const std::string& templateId,
    const std::string& personaId)
{
    auto it = templates_.find(templateId);
    if (it == templates_.end()) {
        return createRandom(personaId);
    }

    return it->second.generate(personaId);
}

std::unique_ptr<NPCPersona> PersonaFactory::createRandom(const std::string& personaId) {
    auto persona = std::make_unique<NPCPersona>(personaId);
    persona->randomize(0.3);

    // Random name
    std::uniform_int_distribution<int> genderDist(0, 1);
    bool male = genderDist(rng) == 0;
    persona->gender = male ? "male" : "female";

    const auto& firstNames = male ? maleFirstNames_ : femaleFirstNames_;
    std::uniform_int_distribution<size_t> firstDist(0, firstNames.size() - 1);
    std::uniform_int_distribution<size_t> surDist(0, surnames_.size() - 1);

    persona->name = firstNames[firstDist(rng)] + " " + surnames_[surDist(rng)];

    // Random age
    std::uniform_int_distribution<int> ageDist(18, 70);
    persona->age = ageDist(rng);

    return persona;
}

const PersonaTemplate* PersonaFactory::getTemplate(const std::string& templateId) const {
    auto it = templates_.find(templateId);
    return it != templates_.end() ? &it->second : nullptr;
}

std::vector<std::string> PersonaFactory::getTemplateIds() const {
    std::vector<std::string> ids;
    for (const auto& [id, _] : templates_) {
        ids.push_back(id);
    }
    return ids;
}

} // namespace Persona
} // namespace NPC
} // namespace Ultima
