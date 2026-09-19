/**
 * FACS.h - Facial Action Coding System evaluator.
 *
 * Innermost operand of the composition: < /rig-logic + /facs >
 */

#ifndef ULTIMA_AVATAR_FACS_H
#define ULTIMA_AVATAR_FACS_H

#include "avatar/Types.h"

#include <array>
#include <string>
#include <vector>

namespace Ultima {
namespace Avatar {

enum class ActionUnit {
    AU1_InnerBrowRaiser = 0,
    AU2_OuterBrowRaiser,
    AU4_BrowLowerer,
    AU5_UpperLidRaiser,
    AU6_CheekRaiser,
    AU7_LidTightener,
    AU9_NoseWrinkler,
    AU10_UpperLipRaiser,
    AU12_LipCornerPuller,
    AU15_LipCornerDepressor,
    AU17_ChinRaiser,
    AU20_LipStretcher,
    AU23_LipTightener,
    AU24_LipPressor,
    AU25_LipsPart,
    AU26_JawDrop,
    AU27_MouthStretch,
    AU43_EyesClosed
};

struct FACSFrame {
    std::array<float, kFacsCount> au{};

    FACSFrame() { au.fill(0.0f); }

    float get(ActionUnit unit) const {
        return au[static_cast<int>(unit)];
    }

    void set(ActionUnit unit, float value) {
        au[static_cast<int>(unit)] = saturate(value);
    }

    float energy() const {
        float sum = 0.0f;
        for (float v : au) {
            sum += v;
        }
        return sum;
    }
};

class FACS {
public:
    FACS() = default;

    /**
     * Evaluate action units from transient drive (emotion recipes + viseme + gaze).
     */
    FACSFrame evaluate(const EchoDrive& drive) const;

    /**
     * Ekman emotion recipes in FACS intensity space (0-1).
     */
    static FACSFrame recipe(Emotion emotion, float intensity);

    static const char* name(ActionUnit unit);
    static std::vector<ActionUnit> allUnits();
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_FACS_H
