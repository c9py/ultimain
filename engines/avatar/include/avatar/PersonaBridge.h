/**
 * PersonaBridge.h - Map NPC-style affect onto an EchoDrive without
 * coupling this library to the NPC headers at compile time.
 */

#ifndef ULTIMA_AVATAR_PERSONA_BRIDGE_H
#define ULTIMA_AVATAR_PERSONA_BRIDGE_H

#include "avatar/Types.h"

namespace Ultima {
namespace Avatar {

struct AffectSample {
    float happiness = 0.0f;
    float sadness = 0.0f;
    float anger = 0.0f;
    float fear = 0.0f;
    float surprise = 0.0f;
    float disgust = 0.0f;
    float valence = 0.0f;
    float arousal = 0.5f;
    float dominance = 0.5f;
    float openness = 0.5f;
    float conscientiousness = 0.5f;
    float extraversion = 0.5f;
    float agreeableness = 0.5f;
    float neuroticism = 0.5f;
    std::string identityName = "npc";
};

class PersonaBridge {
public:
    static EchoDrive fromAffect(const AffectSample& sample);
    static Emotion dominantEmotion(const AffectSample& sample, float* intensityOut);
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_PERSONA_BRIDGE_H
