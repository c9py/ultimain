/**
 * Persona.h - NPC Persona and Personality Data Structures
 *
 * Defines comprehensive personality models for NPCs including:
 * - Big Five personality traits
 * - Emotional states
 * - Motivations and goals
 * - Communication styles
 * - Behavioral tendencies
 *
 * Part of Phase 1: Foundation
 */

#ifndef ULTIMA_NPC_PERSONA_H
#define ULTIMA_NPC_PERSONA_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <chrono>

namespace Ultima {
namespace NPC {
namespace Persona {

/**
 * Big Five personality trait values (0.0 - 1.0 scale)
 * Based on OCEAN model
 */
struct BigFiveTraits {
    // Openness to Experience
    double openness = 0.5;          // Imagination, curiosity, creativity
    double intellectualCuriosity = 0.5;
    double artisticInterest = 0.5;

    // Conscientiousness
    double conscientiousness = 0.5; // Organization, dependability, self-discipline
    double orderliness = 0.5;
    double dutifulness = 0.5;
    double achievementStriving = 0.5;
    double selfDiscipline = 0.5;

    // Extraversion
    double extraversion = 0.5;      // Sociability, assertiveness, positive emotions
    double warmth = 0.5;
    double gregariousness = 0.5;
    double assertiveness = 0.5;
    double activityLevel = 0.5;

    // Agreeableness
    double agreeableness = 0.5;     // Cooperation, trust, empathy
    double trust = 0.5;
    double altruism = 0.5;
    double compliance = 0.5;
    double modesty = 0.5;

    // Neuroticism
    double neuroticism = 0.5;       // Emotional instability, anxiety, moodiness
    double anxiety = 0.5;
    double hostility = 0.5;
    double depression = 0.5;
    double selfConsciousness = 0.5;
    double impulsiveness = 0.5;
    double vulnerability = 0.5;

    /**
     * Calculate overall trait scores from facets
     */
    void calculateOverallScores();

    /**
     * Apply random variation within bounds
     */
    void applyVariation(double maxVariation = 0.1);

    /**
     * Blend with another trait set
     */
    BigFiveTraits blend(const BigFiveTraits& other, double weight) const;
};

/**
 * Emotion types
 */
enum class EmotionType {
    Happiness,
    Sadness,
    Anger,
    Fear,
    Surprise,
    Disgust,
    Trust,
    Anticipation,
    // Compound emotions
    Love,       // Joy + Trust
    Submission, // Trust + Fear
    Awe,        // Fear + Surprise
    Disapproval,// Surprise + Sadness
    Remorse,    // Sadness + Disgust
    Contempt,   // Disgust + Anger
    Aggressiveness, // Anger + Anticipation
    Optimism    // Anticipation + Joy
};

/**
 * Emotional state
 */
struct EmotionalState {
    std::map<EmotionType, double> emotions;  // Intensity 0.0 - 1.0
    double overallValence = 0.0;             // -1.0 (negative) to 1.0 (positive)
    double arousal = 0.5;                    // 0.0 (calm) to 1.0 (excited)
    double dominance = 0.5;                  // 0.0 (submissive) to 1.0 (dominant)

    // Mood (longer-term emotional tendency)
    double moodValence = 0.0;
    double moodStability = 0.5;

    // Stress and well-being
    double stressLevel = 0.0;
    double satisfaction = 0.5;

    /**
     * Get dominant emotion
     */
    EmotionType getDominantEmotion() const;

    /**
     * Apply emotion change
     */
    void applyEmotion(EmotionType emotion, double intensity, double duration = 1.0);

    /**
     * Decay emotions over time
     */
    void decay(double deltaTime, double decayRate = 0.1);

    /**
     * Update mood based on recent emotions
     */
    void updateMood(double deltaTime);
};

/**
 * Communication style
 */
enum class CommunicationStyle {
    Formal,
    Casual,
    Aggressive,
    Passive,
    Diplomatic,
    Scholarly,
    Theatrical,
    Cryptic,
    Humble,
    Boastful
};

/**
 * Communication preferences
 */
struct CommunicationPreferences {
    CommunicationStyle primaryStyle = CommunicationStyle::Casual;
    double formality = 0.5;         // 0.0 (very casual) to 1.0 (very formal)
    double verbosity = 0.5;         // 0.0 (terse) to 1.0 (verbose)
    double emotionalExpression = 0.5; // How much emotion shows in speech
    double directness = 0.5;        // 0.0 (indirect/hinting) to 1.0 (blunt)
    double politeness = 0.5;
    double humor = 0.3;

    // Speech patterns
    std::vector<std::string> favoredExpressions;
    std::vector<std::string> greetings;
    std::vector<std::string> farewells;
    std::vector<std::string> exclamations;
    std::string speechQuirk;        // Unique verbal tic or pattern
};

/**
 * Motivation types
 */
enum class MotivationType {
    // Basic needs (Maslow)
    Survival,           // Food, shelter, safety
    Security,           // Stability, protection
    Social,             // Belonging, friendship
    Esteem,             // Recognition, respect
    SelfActualization,  // Growth, fulfillment

    // Game-specific
    Wealth,             // Accumulate gold/resources
    Power,              // Gain influence/authority
    Knowledge,          // Learn skills/secrets
    Adventure,          // Seek excitement
    Revenge,            // Settle scores
    Love,               // Find/protect loved ones
    Duty,               // Fulfill obligations
    Faith,              // Religious devotion
    Justice,            // Right wrongs
    Freedom             // Independence
};

/**
 * Goal
 */
struct Goal {
    std::string id;
    std::string description;
    MotivationType motivation;
    double priority = 0.5;          // 0.0 - 1.0
    double progress = 0.0;          // 0.0 - 1.0
    bool isActive = true;
    bool isSecret = false;          // Hidden from player

    // Requirements
    std::vector<std::string> prerequisites;
    std::vector<std::string> blockers;

    // Time constraints
    bool hasDeadline = false;
    uint32_t deadlineGameTime = 0;
};

/**
 * Memory of significant event
 */
struct Memory {
    enum class Type {
        Event,
        Interaction,
        Knowledge,
        Emotion,
        Skill,
        Trauma,
        Achievement
    };

    std::string id;
    Type type;
    std::string description;
    std::string relatedEntityId;    // NPC/player/location
    double emotionalImpact = 0.0;   // -1.0 to 1.0
    double importance = 0.5;        // 0.0 to 1.0
    uint32_t gameTime = 0;          // When it happened
    uint32_t lastRecalled = 0;      // Last time accessed
    bool isRepressed = false;       // Traumatic memory suppression

    // Decay
    double clarity = 1.0;           // Fades over time
    double accuracy = 1.0;          // Can become distorted
};

/**
 * Relationship to another entity
 */
struct Relationship {
    enum class Type {
        Stranger,
        Acquaintance,
        Friend,
        CloseFriend,
        Romantic,
        Family,
        Rival,
        Enemy,
        Professional,
        Mentor,
        Student
    };

    std::string targetId;
    Type type = Type::Stranger;

    // Relationship metrics
    double familiarity = 0.0;       // How well they know each other
    double trust = 0.5;             // -1.0 (betrayed) to 1.0 (complete trust)
    double affection = 0.0;         // -1.0 (hatred) to 1.0 (love)
    double respect = 0.5;           // -1.0 (contempt) to 1.0 (admiration)
    double fear = 0.0;              // 0.0 to 1.0

    // Interaction history
    uint32_t interactionCount = 0;
    uint32_t lastInteraction = 0;
    std::vector<std::string> significantEventIds;

    // Debts and obligations
    double owesThem = 0.0;          // Magnitude of debt/favor owed
    double theyOwe = 0.0;           // Magnitude of what they owe

    /**
     * Calculate overall relationship score
     */
    double getOverallScore() const;

    /**
     * Update based on interaction
     */
    void processInteraction(double trustDelta, double affectionDelta, double respectDelta);
};

/**
 * NPC social role/occupation
 */
struct SocialRole {
    std::string title;              // e.g., "Blacksmith", "Guard", "Noble"
    std::string faction;            // Affiliated group
    int socialRank = 5;             // 1-10, affects deference behavior

    // Role-based behaviors
    std::vector<std::string> duties;
    std::vector<std::string> skills;
    std::vector<std::string> knownLocations;

    // Schedule
    struct ScheduleEntry {
        uint32_t startTime;         // Game time
        uint32_t endTime;
        std::string activity;
        std::string location;
    };
    std::vector<ScheduleEntry> dailySchedule;
};

/**
 * Behavioral tendency modifiers
 */
struct BehavioralTendencies {
    // Decision making
    double riskTolerance = 0.5;     // 0.0 (risk-averse) to 1.0 (reckless)
    double impulsiveness = 0.5;     // Vs careful deliberation
    double stubbornness = 0.5;      // Resistance to changing mind

    // Social behavior
    double cooperativeness = 0.5;
    double competitiveness = 0.5;
    double generosity = 0.5;
    double forgiveness = 0.5;
    double loyalty = 0.5;

    // Combat behavior
    double aggression = 0.3;
    double cowardice = 0.3;         // Flee threshold
    double mercy = 0.5;             // Spare defeated foes

    // Economic behavior
    double materialism = 0.5;
    double frugality = 0.5;
    double honesty = 0.7;           // In trade

    // Reactions
    double suspicion = 0.3;         // Of strangers
    double curiosity = 0.5;
    double helpfulness = 0.5;
};

/**
 * Complete NPC Persona
 */
class NPCPersona {
public:
    NPCPersona();
    NPCPersona(const std::string& id);
    ~NPCPersona();

    // Identity
    std::string id;
    std::string name;
    std::string title;
    std::string description;
    uint32_t age = 30;
    std::string gender;
    std::string race;

    // Core personality
    BigFiveTraits traits;
    EmotionalState emotionalState;
    CommunicationPreferences communication;
    BehavioralTendencies behavior;

    // Social
    SocialRole role;
    std::map<std::string, Relationship> relationships;

    // Motivations
    std::vector<Goal> goals;
    std::map<MotivationType, double> motivationStrengths;

    // Memory
    std::vector<Memory> memories;
    std::map<std::string, std::string> knownFacts;  // Key-value knowledge

    // State
    bool isAlive = true;
    bool isConscious = true;
    std::string currentActivity;
    std::string currentLocation;

    /**
     * Update persona over time
     */
    void update(double deltaTime);

    /**
     * Process interaction with another entity
     */
    void processInteraction(const std::string& entityId,
                           const std::string& interactionType,
                           double outcome);

    /**
     * Add memory
     */
    void addMemory(const Memory& memory);

    /**
     * Get relevant memories for context
     */
    std::vector<Memory> recallMemories(const std::string& context, int maxCount = 5);

    /**
     * Get relationship with entity
     */
    Relationship& getRelationship(const std::string& entityId);

    /**
     * Make decision based on personality
     * @return Decision confidence and chosen option
     */
    std::pair<double, int> makeDecision(
        const std::vector<std::string>& options,
        const std::vector<std::map<std::string, double>>& optionTraits
    );

    /**
     * Get dialogue style modifiers
     */
    std::map<std::string, double> getDialogueModifiers() const;

    /**
     * Serialize/deserialize
     */
    std::string serialize() const;
    bool deserialize(const std::string& data);

    /**
     * Clone persona as template
     */
    std::unique_ptr<NPCPersona> clone() const;

    /**
     * Generate random variation
     */
    void randomize(double variationAmount = 0.2);

private:
    void decayMemories(double deltaTime);
    void updateGoals(double deltaTime);
    double calculateDecisionWeight(
        const std::map<std::string, double>& optionTraits
    ) const;
};

/**
 * Persona template for generating similar NPCs
 */
struct PersonaTemplate {
    std::string templateId;
    std::string name;
    std::string category;           // e.g., "merchant", "guard", "peasant"

    // Base values with variation ranges
    BigFiveTraits baseTraits;
    double traitVariation = 0.15;

    CommunicationPreferences baseComm;
    BehavioralTendencies baseBehavior;

    // Common characteristics
    std::vector<std::string> possibleNames;
    std::vector<std::string> possibleTitles;
    SocialRole defaultRole;

    // Generation constraints
    int minAge = 18;
    int maxAge = 60;
    std::vector<std::string> possibleRaces;

    /**
     * Generate new persona from template
     */
    std::unique_ptr<NPCPersona> generate(const std::string& id) const;
};

/**
 * Persona Factory - Creates and manages NPC personas
 */
class PersonaFactory {
public:
    PersonaFactory();
    ~PersonaFactory();

    /**
     * Load templates from file
     */
    bool loadTemplates(const std::string& filename);

    /**
     * Register template
     */
    void registerTemplate(const PersonaTemplate& templ);

    /**
     * Generate persona from template
     */
    std::unique_ptr<NPCPersona> createFromTemplate(
        const std::string& templateId,
        const std::string& personaId
    );

    /**
     * Generate random persona
     */
    std::unique_ptr<NPCPersona> createRandom(const std::string& personaId);

    /**
     * Get template
     */
    const PersonaTemplate* getTemplate(const std::string& templateId) const;

    /**
     * List available templates
     */
    std::vector<std::string> getTemplateIds() const;

private:
    std::map<std::string, PersonaTemplate> templates_;

    // Name generation data
    std::vector<std::string> maleFirstNames_;
    std::vector<std::string> femaleFirstNames_;
    std::vector<std::string> surnames_;
};

} // namespace Persona
} // namespace NPC
} // namespace Ultima

#endif // ULTIMA_NPC_PERSONA_H
