/**
 * Types.h - Shared math and drive types for the avatar motion mesh pipeline.
 *
 * Composition:
 *   meshy3d( live2d-dtecho[ meta-echo-dna< rig-logic + facs > ] )
 *     => avatar motion mesh /deltecho
 */

#ifndef ULTIMA_AVATAR_TYPES_H
#define ULTIMA_AVATAR_TYPES_H

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace Ultima {
namespace Avatar {

inline float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

inline float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float saturate(float v) {
    return clampf(v, 0.0f, 1.0f);
}

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3& operator+=(const Vec3& o) {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }

    float length() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3 normalized() const {
        const float len = length();
        if (len < 1e-8f) {
            return {0.0f, 0.0f, 0.0f};
        }
        return {x / len, y / len, z / len};
    }
};

inline Vec3 operator*(float s, const Vec3& v) {
    return v * s;
}

struct Quat {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    static Quat identity() { return {}; }

    static Quat fromEulerYxz(float pitch, float yaw, float roll) {
        const float hx = pitch * 0.5f;
        const float hy = yaw * 0.5f;
        const float hz = roll * 0.5f;
        const float cx = std::cos(hx);
        const float sx = std::sin(hx);
        const float cy = std::cos(hy);
        const float sy = std::sin(hy);
        const float cz = std::cos(hz);
        const float sz = std::sin(hz);
        Quat q;
        q.w = cx * cy * cz + sx * sy * sz;
        q.x = sx * cy * cz - cx * sy * sz;
        q.y = cx * sy * cz + sx * cy * sz;
        q.z = cx * cy * sz - sx * sy * cz;
        return q;
    }

    Vec3 rotate(const Vec3& v) const {
        const Vec3 qv{x, y, z};
        const Vec3 t = Vec3{2.0f * (qv.y * v.z - qv.z * v.y),
                            2.0f * (qv.z * v.x - qv.x * v.z),
                            2.0f * (qv.x * v.y - qv.y * v.x)};
        return v + t * w + Vec3{qv.y * t.z - qv.z * t.y,
                                qv.z * t.x - qv.x * t.z,
                                qv.x * t.y - qv.y * t.x};
    }
};

struct Transform {
    Vec3 translation;
    Quat rotation;
    Vec3 scale{1.0f, 1.0f, 1.0f};

    Vec3 apply(const Vec3& p) const {
        Vec3 r = rotation.rotate(Vec3{p.x * scale.x, p.y * scale.y, p.z * scale.z});
        return r + translation;
    }
};

constexpr int kEchoDim = 8;
constexpr int kDnaDim = 32;
constexpr int kFacsCount = 18;
constexpr int kLive2DCount = 16;
constexpr int kReservoirSize = 24;

enum class EchoDimension {
    Identity = 0,
    Adaptability = 1,
    Collaboration = 2,
    Memory = 3,
    Gestalt = 4,
    Autonomy = 5,
    Exploration = 6,
    Purpose = 7
};

enum class Emotion {
    Neutral,
    Happiness,
    Sadness,
    Anger,
    Fear,
    Surprise,
    Disgust
};

struct EchoState {
    std::array<float, kEchoDim> dimensions{};
    float cognitiveLoad = 0.35f;
    float recentActivity = 0.20f;
    float resonance = 0.50f;
    float coherence = 0.70f;
    float energy = 0.60f;
    int recursiveDepth = 2;

    EchoState() {
        dimensions.fill(0.5f);
    }

    float attentionThreshold() const {
        return clampf(0.5f + cognitiveLoad * 0.3f - recentActivity * 0.2f, 0.05f, 0.95f);
    }

    float get(EchoDimension d) const {
        return dimensions[static_cast<int>(d)];
    }
};

/**
 * Per-frame drive into the composition. Transient affect + optional viseme.
 */
struct EchoDrive {
    EchoState echo;
    Emotion emotion = Emotion::Neutral;
    float emotionIntensity = 0.0f;
    float valence = 0.0f;
    float arousal = 0.5f;
    float dominance = 0.5f;
    float visemeOpen = 0.0f;
    float gazeX = 0.0f;
    float gazeY = 0.0f;
    float headYaw = 0.0f;
    float headPitch = 0.0f;
    float headRoll = 0.0f;
    std::string identityName = "avatar";
};

inline const char* emotionName(Emotion e) {
    switch (e) {
    case Emotion::Happiness:
        return "happiness";
    case Emotion::Sadness:
        return "sadness";
    case Emotion::Anger:
        return "anger";
    case Emotion::Fear:
        return "fear";
    case Emotion::Surprise:
        return "surprise";
    case Emotion::Disgust:
        return "disgust";
    case Emotion::Neutral:
    default:
        return "neutral";
    }
}

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_TYPES_H
