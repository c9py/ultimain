#include "avatar/FACS.h"

namespace Ultima {
namespace Avatar {

namespace {

void add(FACSFrame& frame, ActionUnit unit, float value) {
    frame.set(unit, frame.get(unit) + value);
}

} // namespace

FACSFrame FACS::recipe(Emotion emotion, float intensity) {
    FACSFrame frame;
    const float i = saturate(intensity);
    switch (emotion) {
    case Emotion::Happiness:
        frame.set(ActionUnit::AU6_CheekRaiser, 0.60f * i);
        frame.set(ActionUnit::AU12_LipCornerPuller, 0.85f * i);
        break;
    case Emotion::Sadness:
        frame.set(ActionUnit::AU1_InnerBrowRaiser, 0.65f * i);
        frame.set(ActionUnit::AU4_BrowLowerer, 0.40f * i);
        frame.set(ActionUnit::AU15_LipCornerDepressor, 0.75f * i);
        break;
    case Emotion::Anger:
        frame.set(ActionUnit::AU4_BrowLowerer, 0.80f * i);
        frame.set(ActionUnit::AU5_UpperLidRaiser, 0.35f * i);
        frame.set(ActionUnit::AU7_LidTightener, 0.55f * i);
        frame.set(ActionUnit::AU23_LipTightener, 0.60f * i);
        break;
    case Emotion::Fear:
        frame.set(ActionUnit::AU1_InnerBrowRaiser, 0.50f * i);
        frame.set(ActionUnit::AU2_OuterBrowRaiser, 0.50f * i);
        frame.set(ActionUnit::AU4_BrowLowerer, 0.30f * i);
        frame.set(ActionUnit::AU5_UpperLidRaiser, 0.60f * i);
        frame.set(ActionUnit::AU20_LipStretcher, 0.50f * i);
        frame.set(ActionUnit::AU26_JawDrop, 0.25f * i);
        break;
    case Emotion::Surprise:
        frame.set(ActionUnit::AU1_InnerBrowRaiser, 0.70f * i);
        frame.set(ActionUnit::AU2_OuterBrowRaiser, 0.70f * i);
        frame.set(ActionUnit::AU5_UpperLidRaiser, 0.70f * i);
        frame.set(ActionUnit::AU26_JawDrop, 0.60f * i);
        break;
    case Emotion::Disgust:
        frame.set(ActionUnit::AU9_NoseWrinkler, 0.70f * i);
        frame.set(ActionUnit::AU10_UpperLipRaiser, 0.50f * i);
        frame.set(ActionUnit::AU17_ChinRaiser, 0.40f * i);
        break;
    case Emotion::Neutral:
    default:
        break;
    }
    return frame;
}

FACSFrame FACS::evaluate(const EchoDrive& drive) const {
    FACSFrame frame = recipe(drive.emotion, drive.emotionIntensity);

    // Valence / arousal / dominance color the face beyond the named recipe.
    if (drive.valence > 0.0f) {
        add(frame, ActionUnit::AU12_LipCornerPuller, drive.valence * 0.25f);
        add(frame, ActionUnit::AU6_CheekRaiser, drive.valence * 0.12f);
    } else if (drive.valence < 0.0f) {
        add(frame, ActionUnit::AU15_LipCornerDepressor, -drive.valence * 0.30f);
        add(frame, ActionUnit::AU4_BrowLowerer, -drive.valence * 0.10f);
    }

    add(frame, ActionUnit::AU5_UpperLidRaiser, drive.arousal * 0.15f);
    add(frame, ActionUnit::AU26_JawDrop, drive.visemeOpen * 0.85f);
    add(frame, ActionUnit::AU25_LipsPart, drive.visemeOpen * 0.55f);

    if (drive.arousal < 0.25f && drive.emotionIntensity < 0.15f) {
        add(frame, ActionUnit::AU43_EyesClosed, (0.25f - drive.arousal) * 0.35f);
    }

    // Echo cognitive load slightly tightens the lids / brows.
    add(frame, ActionUnit::AU7_LidTightener, drive.echo.cognitiveLoad * 0.08f);
    add(frame, ActionUnit::AU4_BrowLowerer, drive.echo.cognitiveLoad * 0.05f);

    return frame;
}

const char* FACS::name(ActionUnit unit) {
    switch (unit) {
    case ActionUnit::AU1_InnerBrowRaiser:
        return "AU1";
    case ActionUnit::AU2_OuterBrowRaiser:
        return "AU2";
    case ActionUnit::AU4_BrowLowerer:
        return "AU4";
    case ActionUnit::AU5_UpperLidRaiser:
        return "AU5";
    case ActionUnit::AU6_CheekRaiser:
        return "AU6";
    case ActionUnit::AU7_LidTightener:
        return "AU7";
    case ActionUnit::AU9_NoseWrinkler:
        return "AU9";
    case ActionUnit::AU10_UpperLipRaiser:
        return "AU10";
    case ActionUnit::AU12_LipCornerPuller:
        return "AU12";
    case ActionUnit::AU15_LipCornerDepressor:
        return "AU15";
    case ActionUnit::AU17_ChinRaiser:
        return "AU17";
    case ActionUnit::AU20_LipStretcher:
        return "AU20";
    case ActionUnit::AU23_LipTightener:
        return "AU23";
    case ActionUnit::AU24_LipPressor:
        return "AU24";
    case ActionUnit::AU25_LipsPart:
        return "AU25";
    case ActionUnit::AU26_JawDrop:
        return "AU26";
    case ActionUnit::AU27_MouthStretch:
        return "AU27";
    case ActionUnit::AU43_EyesClosed:
        return "AU43";
    }
    return "AU?";
}

std::vector<ActionUnit> FACS::allUnits() {
    std::vector<ActionUnit> units;
    units.reserve(kFacsCount);
    for (int i = 0; i < kFacsCount; ++i) {
        units.push_back(static_cast<ActionUnit>(i));
    }
    return units;
}

} // namespace Avatar
} // namespace Ultima
