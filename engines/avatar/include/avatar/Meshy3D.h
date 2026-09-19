/**
 * Meshy3D.h - 3D avatar mesh synthesis / Meshy-compatible bind.
 *
 *   /meshy3d ( /live2d-dtecho [ ... ] )
 *
 * Produces a skinned humanoid mesh whose morph targets and bone names
 * can accept a Meshy-rigged GLB or the local synthesizer fallback.
 */

#ifndef ULTIMA_AVATAR_MESHY3D_H
#define ULTIMA_AVATAR_MESHY3D_H

#include "avatar/Live2DDtecho.h"

#include <array>
#include <string>
#include <vector>

namespace Ultima {
namespace Avatar {

struct Vertex {
    Vec3 position;
    Vec3 normal;
    float u = 0.0f;
    float v = 0.0f;
    int region = 0; // 0 body, 1 brow, 2 eye, 3 mouth, 4 jaw, 5 cheek
    std::array<int, 4> bones{{-1, -1, -1, -1}};
    std::array<float, 4> weights{{1.0f, 0.0f, 0.0f, 0.0f}};
};

struct Mesh3D {
    std::string name;
    std::string source = "local-synthesize";
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    std::vector<JointPose> bones;
    std::vector<std::string> morphNames;
    std::uint64_t dnaFingerprint = 0;
};

struct MeshyTask {
    std::string mode = "local-synthesize";
    std::string prompt;
    std::string topology = "humanoid";
    std::string artStyle = "ultima-portrait";
    bool wantsAutoRig = true;
};

class Meshy3D {
public:
    Meshy3D() = default;

    /**
     * Build a Meshy-compatible task description from Live2D + DNA.
     * Does not call a network API; callers can serialize this to Meshy.
     */
    MeshyTask describe(const Live2DPose& live2d, const MetaEchoDNA& dna) const;

    /**
     * Synthesize a local humanoid avatar mesh bound to the identity DNA
     * and current Live2D pose (morph-ready, Mixamo-like bone names).
     */
    Mesh3D synthesize(const Live2DPose& live2d, const MetaEchoDNA& dna) const;

    /**
     * Bind an existing mesh (e.g. imported Meshy GLB topology) to DNA + Live2D.
     */
    Mesh3D bind(Mesh3D mesh, const Live2DPose& live2d, const MetaEchoDNA& dna) const;

private:
    Mesh3D buildHumanoid(const MetaEchoDNA& dna) const;
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_MESHY3D_H
