/**
 * Live2DDtecho.h - Live2D Cubism parameters driven by Deep Tree Echo.
 *
 *   /live2d-dtecho [ /meta-echo-dna < ... > ]
 *
 * Maps Echo cognitive state + identity DNA onto a 2D puppet parameter set
 * that later binds onto the 3D motion mesh.
 */

#ifndef ULTIMA_AVATAR_LIVE2D_DTECHO_H
#define ULTIMA_AVATAR_LIVE2D_DTECHO_H

#include "avatar/MetaEchoDNA.h"

#include <array>
#include <string>
#include <vector>

namespace Ultima {
namespace Avatar {

enum class Live2DParam {
    AngleX = 0,
    AngleY,
    AngleZ,
    EyeLOpen,
    EyeROpen,
    EyeBallX,
    EyeBallY,
    BrowLY,
    BrowRY,
    BrowLForm,
    BrowRForm,
    MouthOpenY,
    MouthForm,
    Cheek,
    BodyAngleX,
    Breath
};

struct Live2DPose {
    std::array<float, kLive2DCount> params{};

    Live2DPose() {
        params.fill(0.0f);
        params[static_cast<int>(Live2DParam::EyeLOpen)] = 1.0f;
        params[static_cast<int>(Live2DParam::EyeROpen)] = 1.0f;
    }

    float get(Live2DParam p) const {
        return params[static_cast<int>(p)];
    }

    void set(Live2DParam p, float value) {
        params[static_cast<int>(p)] = value;
    }
};

class Live2DDtecho {
public:
    Live2DDtecho() = default;

    /**
     * Drive Live2D parameters from identity DNA + current Echo / FACS / drive.
     */
    Live2DPose drive(const MetaEchoDNA& dna,
                     const FACSFrame& facs,
                     const EchoDrive& drive) const;

    static const char* name(Live2DParam p);
    static std::vector<Live2DParam> allParams();
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_LIVE2D_DTECHO_H
