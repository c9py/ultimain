/**
 * Deltecho.h - Deep-Tree Echo residual applied to a motion mesh.
 *
 * Post-process of the composition: avatar motion mesh /deltecho
 *
 * An Echo State reservoir remembers prior affect and applies a temporal
 * delta so the mesh carries living echo instead of popping to the
 * instantaneous FACS pose.
 */

#ifndef ULTIMA_AVATAR_DELTECHO_H
#define ULTIMA_AVATAR_DELTECHO_H

#include "avatar/MotionMesh.h"

#include <array>

namespace Ultima {
namespace Avatar {

class Deltecho {
public:
    Deltecho();

    void reset();

    MotionFrame apply(const MotionFrame& frame, const EchoDrive& drive, float dt);

    const std::array<float, kReservoirSize>& reservoir() const { return reservoir_; }
    float lastResidualEnergy() const { return lastResidual_; }
    bool hasHistory() const { return hasHistory_; }

private:
    std::array<float, kReservoirSize> reservoir_{};
    std::array<float, kFacsCount> prevAu_{};
    std::array<float, kLive2DCount> prevLive2d_{};
    bool hasHistory_ = false;
    float lastResidual_ = 0.0f;

    void stepReservoir(const EchoDrive& drive, const MotionFrame& frame, float dt);
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_DELTECHO_H
