#include "avatar/Deltecho.h"

#include <cmath>
#include <utility>

namespace Ultima {
namespace Avatar {

namespace {

float hash01(int a, int b) {
    const int n = (a * 374761393 + b * 668265263) ^ (a * b * 1274126177);
    const unsigned u = static_cast<unsigned>(n) * 1664525u + 1013904223u;
    return (u & 0xFFFFFFu) / static_cast<float>(0xFFFFFFu);
}

} // namespace

Deltecho::Deltecho() {
    reset();
}

void Deltecho::reset() {
    reservoir_.fill(0.0f);
    prevAu_.fill(0.0f);
    prevLive2d_.fill(0.0f);
    hasHistory_ = false;
    lastResidual_ = 0.0f;
}

void Deltecho::stepReservoir(const EchoDrive& drive, const MotionFrame& frame, float dt) {
    const float leak = clampf(0.25f * std::max(dt, 0.001f), 0.02f, 0.45f);
    std::array<float, kReservoirSize> next{};
    for (int i = 0; i < kReservoirSize; ++i) {
        float acc = 0.0f;
        for (int j = 0; j < kReservoirSize; ++j) {
            const float w = (hash01(i, j) - 0.5f) * 0.35f;
            acc += w * reservoir_[j];
        }
        for (int j = 0; j < kEchoDim; ++j) {
            acc += (hash01(i, 40 + j) - 0.4f) * 0.55f * drive.echo.dimensions[j];
        }
        for (int j = 0; j < 6; ++j) {
            acc += (hash01(i, 80 + j) - 0.5f) * 0.40f * frame.facs.au[j];
        }
        acc += drive.echo.resonance * 0.15f + drive.echo.coherence * 0.10f;
        next[i] = (1.0f - leak) * reservoir_[i] + leak * std::tanh(acc);
    }
    reservoir_ = next;
}

MotionFrame Deltecho::apply(const MotionFrame& frame, const EchoDrive& drive, float dt) {
    MotionFrame out = frame;
    stepReservoir(drive, frame, dt);

    if (!hasHistory_) {
        prevAu_ = frame.facs.au;
        prevLive2d_ = frame.live2d.params;
        hasHistory_ = true;
        lastResidual_ = 0.0f;
        return out;
    }

    float residual = 0.0f;
    FACSFrame echoedFacs = frame.facs;
    Live2DPose echoedLive = frame.live2d;

    for (int i = 0; i < kFacsCount; ++i) {
        const float delta = frame.facs.au[i] - prevAu_[i];
        const float echo = reservoir_[i % kReservoirSize] * 0.12f;
        const float mixed = saturate(frame.facs.au[i] + delta * 0.25f + echo * 0.08f);
        residual += std::fabs(mixed - frame.facs.au[i]);
        echoedFacs.au[i] = mixed;
        prevAu_[i] = mixed;
    }
    for (int i = 0; i < kLive2DCount; ++i) {
        const float delta = frame.live2d.params[i] - prevLive2d_[i];
        const float echo = reservoir_[(i + 8) % kReservoirSize] * 0.10f;
        float mixed = frame.live2d.params[i] + delta * 0.20f + echo * 0.06f;
        if (i == static_cast<int>(Live2DParam::EyeLOpen) ||
            i == static_cast<int>(Live2DParam::EyeROpen) ||
            i == static_cast<int>(Live2DParam::MouthOpenY) ||
            i == static_cast<int>(Live2DParam::Cheek) ||
            i == static_cast<int>(Live2DParam::Breath)) {
            mixed = saturate(mixed);
        } else {
            mixed = clampf(mixed, -1.0f, 1.0f);
        }
        residual += std::fabs(mixed - frame.live2d.params[i]);
        echoedLive.params[i] = mixed;
        prevLive2d_[i] = mixed;
    }

    lastResidual_ = residual;
    out.facs = echoedFacs;
    out.live2d = echoedLive;
    out.mouthOpen = echoedLive.get(Live2DParam::MouthOpenY);
    out.smile = saturate(echoedLive.get(Live2DParam::MouthForm));
    out.blink = saturate(1.0f - echoedLive.get(Live2DParam::EyeLOpen));

    // Re-skin with the echoed parameters so the mesh itself carries the delta.
    MotionFrame reskinned = MotionMesh::skin(frame.rest, frame.rig, echoedFacs, echoedLive, frame.dna);
    out.deformed = std::move(reskinned.deformed);
    out.normals = std::move(reskinned.normals);
    return out;
}

} // namespace Avatar
} // namespace Ultima
