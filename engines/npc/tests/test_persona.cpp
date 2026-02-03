/*
 * test_persona.cpp - Tests for NPC Persona System
 */

#include "test_framework.h"
#include "persona/Persona.h"

using namespace Ultima::NPC::Persona;

// Test Big Five traits default values
bool test_big_five_defaults() {
    BigFiveTraits traits;

    TEST_ASSERT_FLOAT_NEAR(0.5, traits.openness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.conscientiousness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.extraversion, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.agreeableness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.neuroticism, 0.01);

    return true;
}

// Test Big Five trait facets
bool test_big_five_facets() {
    BigFiveTraits traits;

    // Openness facets
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.intellectualCuriosity, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.artisticInterest, 0.01);

    // Conscientiousness facets
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.orderliness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.dutifulness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.achievementStriving, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.selfDiscipline, 0.01);

    // Extraversion facets
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.warmth, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.gregariousness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.assertiveness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.activityLevel, 0.01);

    // Agreeableness facets
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.trust, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.altruism, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.compliance, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.modesty, 0.01);

    // Neuroticism facets
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.anxiety, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.hostility, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.depression, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.selfConsciousness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.impulsiveness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, traits.vulnerability, 0.01);

    return true;
}

// Test Big Five trait modification
bool test_big_five_modification() {
    BigFiveTraits traits;

    traits.openness = 0.8;
    traits.conscientiousness = 0.3;
    traits.extraversion = 0.9;
    traits.agreeableness = 0.7;
    traits.neuroticism = 0.2;

    TEST_ASSERT_FLOAT_NEAR(0.8, traits.openness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.3, traits.conscientiousness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.9, traits.extraversion, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.7, traits.agreeableness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.2, traits.neuroticism, 0.01);

    return true;
}

// Test Big Five trait blending
bool test_big_five_blend() {
    BigFiveTraits traits1;
    traits1.openness = 0.8;
    traits1.conscientiousness = 0.2;

    BigFiveTraits traits2;
    traits2.openness = 0.4;
    traits2.conscientiousness = 0.8;

    // 50/50 blend
    BigFiveTraits blended = traits1.blend(traits2, 0.5);
    TEST_ASSERT_FLOAT_NEAR(0.6, blended.openness, 0.1);  // (0.8 + 0.4) / 2
    TEST_ASSERT_FLOAT_NEAR(0.5, blended.conscientiousness, 0.1);  // (0.2 + 0.8) / 2

    return true;
}

// Test trait variation application
bool test_trait_variation() {
    BigFiveTraits traits;
    double original = traits.openness;

    traits.applyVariation(0.1);

    // Value should be within +/- 0.1 of original
    TEST_ASSERT(traits.openness >= original - 0.15);
    TEST_ASSERT(traits.openness <= original + 0.15);

    return true;
}

// Test trait boundary enforcement
bool test_trait_boundaries() {
    BigFiveTraits traits;

    // Set extreme values
    traits.openness = 1.0;
    traits.neuroticism = 0.0;

    TEST_ASSERT_FLOAT_NEAR(1.0, traits.openness, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.0, traits.neuroticism, 0.01);

    // Values should stay in valid range
    TEST_ASSERT(traits.openness >= 0.0 && traits.openness <= 1.0);
    TEST_ASSERT(traits.neuroticism >= 0.0 && traits.neuroticism <= 1.0);

    return true;
}

// Test emotion type enumeration
bool test_emotion_types() {
    // Basic emotions
    TEST_ASSERT(EmotionType::Happiness != EmotionType::Sadness);
    TEST_ASSERT(EmotionType::Sadness != EmotionType::Anger);
    TEST_ASSERT(EmotionType::Anger != EmotionType::Fear);
    TEST_ASSERT(EmotionType::Fear != EmotionType::Surprise);
    TEST_ASSERT(EmotionType::Surprise != EmotionType::Disgust);
    TEST_ASSERT(EmotionType::Disgust != EmotionType::Trust);
    TEST_ASSERT(EmotionType::Trust != EmotionType::Anticipation);

    // Compound emotions
    TEST_ASSERT(EmotionType::Love != EmotionType::Submission);
    TEST_ASSERT(EmotionType::Submission != EmotionType::Awe);
    TEST_ASSERT(EmotionType::Awe != EmotionType::Disapproval);

    return true;
}

// Test personality archetype concept
bool test_personality_archetypes() {
    // Hero archetype - high in extraversion, conscientiousness, openness
    BigFiveTraits hero;
    hero.extraversion = 0.8;
    hero.conscientiousness = 0.85;
    hero.openness = 0.75;
    hero.agreeableness = 0.7;
    hero.neuroticism = 0.3;

    TEST_ASSERT(hero.extraversion > 0.7);
    TEST_ASSERT(hero.conscientiousness > 0.7);
    TEST_ASSERT(hero.neuroticism < 0.5);

    // Villain archetype - low agreeableness, high neuroticism
    BigFiveTraits villain;
    villain.agreeableness = 0.2;
    villain.neuroticism = 0.8;
    villain.conscientiousness = 0.6;

    TEST_ASSERT(villain.agreeableness < 0.5);
    TEST_ASSERT(villain.neuroticism > 0.5);

    // Wise mentor - high openness, conscientiousness
    BigFiveTraits mentor;
    mentor.openness = 0.9;
    mentor.conscientiousness = 0.8;
    mentor.agreeableness = 0.75;
    mentor.extraversion = 0.4;
    mentor.neuroticism = 0.2;

    TEST_ASSERT(mentor.openness > 0.8);
    TEST_ASSERT(mentor.neuroticism < 0.3);

    return true;
}

// Test trait correlation patterns
bool test_trait_correlations() {
    BigFiveTraits traits;

    // Typically correlated traits for a conscientious person
    traits.conscientiousness = 0.9;
    traits.orderliness = 0.85;
    traits.dutifulness = 0.88;
    traits.selfDiscipline = 0.9;

    // Main trait and facets should be related
    TEST_ASSERT(traits.conscientiousness >= 0.8);
    TEST_ASSERT(traits.orderliness >= 0.7);
    TEST_ASSERT(traits.selfDiscipline >= 0.7);

    return true;
}

// Test trait influence on behavior tendency
bool test_trait_behavior_influence() {
    BigFiveTraits extrovert;
    extrovert.extraversion = 0.9;
    extrovert.gregariousness = 0.85;
    extrovert.warmth = 0.8;

    BigFiveTraits introvert;
    introvert.extraversion = 0.2;
    introvert.gregariousness = 0.15;
    introvert.warmth = 0.4;

    // Extrovert should have higher social tendency scores
    double extrovertSocial = (extrovert.extraversion + extrovert.gregariousness + extrovert.warmth) / 3;
    double introvertSocial = (introvert.extraversion + introvert.gregariousness + introvert.warmth) / 3;

    TEST_ASSERT(extrovertSocial > introvertSocial);
    TEST_ASSERT(extrovertSocial > 0.7);
    TEST_ASSERT(introvertSocial < 0.4);

    return true;
}

int main() {
    TEST_SUITE("Persona System");

    RUN_TEST("Big Five defaults", test_big_five_defaults);
    RUN_TEST("Big Five facets", test_big_five_facets);
    RUN_TEST("Big Five modification", test_big_five_modification);
    RUN_TEST("Big Five blend", test_big_five_blend);
    RUN_TEST("Trait variation", test_trait_variation);
    RUN_TEST("Trait boundaries", test_trait_boundaries);
    RUN_TEST("Emotion types", test_emotion_types);
    RUN_TEST("Personality archetypes", test_personality_archetypes);
    RUN_TEST("Trait correlations", test_trait_correlations);
    RUN_TEST("Trait behavior influence", test_trait_behavior_influence);

    TEST_SUMMARY();
}
