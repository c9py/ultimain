#include "avatar/PersonaBridge.h"

namespace Ultima {
namespace Avatar {

Emotion PersonaBridge::dominantEmotion(const AffectSample& sample, float* intensityOut) {
    struct Pair {
        Emotion emotion;
        float value;
    };
    const Pair pairs[] = {
        {Emotion::Happiness, sample.happiness},
        {Emotion::Sadness, sample.sadness},
        {Emotion::Anger, sample.anger},
        {Emotion::Fear, sample.fear},
        {Emotion::Surprise, sample.surprise},
        {Emotion::Disgust, sample.disgust},
    };
    Pair best{Emotion::Neutral, 0.08f};
    for (const auto& p : pairs) {
        if (p.value > best.value) {
            best = p;
        }
    }
    if (intensityOut) {
        *intensityOut = best.emotion == Emotion::Neutral ? 0.0f : saturate(best.value);
    }
    return best.emotion;
}

EchoDrive PersonaBridge::fromAffect(const AffectSample& sample) {
    EchoDrive drive;
    drive.identityName = sample.identityName;
    drive.valence = clampf(sample.valence, -1.0f, 1.0f);
    drive.arousal = saturate(sample.arousal);
    drive.dominance = saturate(sample.dominance);
    drive.emotion = dominantEmotion(sample, &drive.emotionIntensity);

    drive.echo.dimensions[static_cast<int>(EchoDimension::Identity)] = 0.55f;
    drive.echo.dimensions[static_cast<int>(EchoDimension::Adaptability)] = saturate(sample.openness);
    drive.echo.dimensions[static_cast<int>(EchoDimension::Collaboration)] =
        saturate(sample.agreeableness);
    drive.echo.dimensions[static_cast<int>(EchoDimension::Memory)] = 0.60f;
    drive.echo.dimensions[static_cast<int>(EchoDimension::Gestalt)] =
        saturate((sample.openness + sample.conscientiousness) * 0.5f);
    drive.echo.dimensions[static_cast<int>(EchoDimension::Autonomy)] =
        saturate(sample.extraversion);
    drive.echo.dimensions[static_cast<int>(EchoDimension::Exploration)] =
        saturate(sample.openness * 0.7f + sample.extraversion * 0.3f);
    drive.echo.dimensions[static_cast<int>(EchoDimension::Purpose)] =
        saturate(sample.conscientiousness);
    drive.echo.cognitiveLoad = saturate(sample.neuroticism * 0.5f + sample.arousal * 0.3f);
    drive.echo.energy = saturate(0.35f + sample.arousal * 0.5f);
    drive.echo.coherence = saturate(1.0f - sample.neuroticism * 0.4f);
    drive.echo.resonance = saturate(0.4f + sample.agreeableness * 0.3f);
    return drive;
}

} // namespace Avatar
} // namespace Ultima
