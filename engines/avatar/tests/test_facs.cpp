#include "test_framework.h"
#include "avatar/FACS.h"

using namespace Ultima::Avatar;

bool test_recipe_happiness_raises_smile() {
    const FACSFrame frame = FACS::recipe(Emotion::Happiness, 1.0f);
    TEST_ASSERT(frame.get(ActionUnit::AU12_LipCornerPuller) > 0.7f);
    TEST_ASSERT(frame.get(ActionUnit::AU6_CheekRaiser) > 0.4f);
    TEST_ASSERT(frame.get(ActionUnit::AU15_LipCornerDepressor) < 0.05f);
    return true;
}

bool test_recipe_sadness_depresses_corners() {
    const FACSFrame frame = FACS::recipe(Emotion::Sadness, 1.0f);
    TEST_ASSERT(frame.get(ActionUnit::AU15_LipCornerDepressor) > 0.6f);
    TEST_ASSERT(frame.get(ActionUnit::AU1_InnerBrowRaiser) > 0.5f);
    return true;
}

bool test_recipe_surprise_drops_jaw() {
    const FACSFrame frame = FACS::recipe(Emotion::Surprise, 1.0f);
    TEST_ASSERT(frame.get(ActionUnit::AU26_JawDrop) > 0.5f);
    TEST_ASSERT(frame.get(ActionUnit::AU5_UpperLidRaiser) > 0.5f);
    return true;
}

bool test_neutral_is_quiet() {
    const FACSFrame frame = FACS::recipe(Emotion::Neutral, 1.0f);
    TEST_ASSERT_FLOAT_NEAR(0.0, frame.energy(), 0.001);
    return true;
}

bool test_intensity_scales() {
    const FACSFrame full = FACS::recipe(Emotion::Anger, 1.0f);
    const FACSFrame half = FACS::recipe(Emotion::Anger, 0.5f);
    TEST_ASSERT(half.get(ActionUnit::AU4_BrowLowerer) < full.get(ActionUnit::AU4_BrowLowerer));
    TEST_ASSERT_FLOAT_NEAR(full.get(ActionUnit::AU4_BrowLowerer) * 0.5,
                           half.get(ActionUnit::AU4_BrowLowerer), 0.02);
    return true;
}

bool test_viseme_opens_jaw() {
    FACS facs;
    EchoDrive drive;
    drive.visemeOpen = 0.8f;
    const FACSFrame frame = facs.evaluate(drive);
    TEST_ASSERT(frame.get(ActionUnit::AU26_JawDrop) > 0.6f);
    TEST_ASSERT(frame.get(ActionUnit::AU25_LipsPart) > 0.3f);
    return true;
}

bool test_names_cover_all_units() {
    TEST_ASSERT_EQUAL(kFacsCount, static_cast<long long>(FACS::allUnits().size()));
    TEST_ASSERT_STRING_EQUAL("AU12", FACS::name(ActionUnit::AU12_LipCornerPuller));
    TEST_ASSERT_STRING_EQUAL("AU26", FACS::name(ActionUnit::AU26_JawDrop));
    return true;
}

int main() {
    TEST_SUITE("FACS");
    RUN_TEST("happiness smile", test_recipe_happiness_raises_smile);
    RUN_TEST("sadness frown", test_recipe_sadness_depresses_corners);
    RUN_TEST("surprise jaw", test_recipe_surprise_drops_jaw);
    RUN_TEST("neutral quiet", test_neutral_is_quiet);
    RUN_TEST("intensity scales", test_intensity_scales);
    RUN_TEST("viseme jaw", test_viseme_opens_jaw);
    RUN_TEST("unit names", test_names_cover_all_units);
    TEST_SUMMARY();
}
