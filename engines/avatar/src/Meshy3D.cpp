#include "avatar/Meshy3D.h"

#include <cmath>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Ultima {
namespace Avatar {

namespace {

void addVertex(Mesh3D& mesh, const Vec3& p, const Vec3& n, float u, float v,
               int region, int bone, float w0 = 1.0f, int bone2 = -1, float w1 = 0.0f) {
    Vertex vx;
    vx.position = p;
    vx.normal = n.normalized();
    vx.u = u;
    vx.v = v;
    vx.region = region;
    vx.bones[0] = bone;
    vx.weights[0] = w0;
    vx.bones[1] = bone2;
    vx.weights[1] = w1;
    mesh.vertices.push_back(vx);
}

void addTri(Mesh3D& mesh, int a, int b, int c) {
    mesh.indices.push_back(a);
    mesh.indices.push_back(b);
    mesh.indices.push_back(c);
}

int regionOfHead(const Vec3& local) {
    // local is relative to head center, +Z forward, +Y up
    if (local.y > 0.045f && local.z > 0.02f) {
        return 1; // brow
    }
    if (std::fabs(local.x) > 0.012f && local.y > 0.005f && local.y < 0.045f && local.z > 0.04f) {
        return 2; // eye
    }
    if (local.y < -0.015f && local.y > -0.055f && local.z > 0.03f) {
        return 3; // mouth
    }
    if (local.y < -0.055f) {
        return 4; // jaw
    }
    if (std::fabs(local.x) > 0.05f && local.y < 0.02f && local.z > 0.0f) {
        return 5; // cheek
    }
    return 0;
}

} // namespace

MeshyTask Meshy3D::describe(const Live2DPose& live2d, const MetaEchoDNA& dna) const {
    MeshyTask task;
    task.mode = "local-synthesize";
    task.topology = "humanoid";
    task.artStyle = "ultima-portrait";
    task.wantsAutoRig = true;

    std::ostringstream prompt;
    prompt << "Humanoid Ultima avatar named " << (dna.name.empty() ? "avatar" : dna.name)
           << ", identity fingerprint " << dna.fingerprint
           << ", mouthOpen=" << live2d.get(Live2DParam::MouthOpenY)
           << ", mouthForm=" << live2d.get(Live2DParam::MouthForm)
           << ", eyeOpen=" << live2d.get(Live2DParam::EyeLOpen)
           << ", T-pose ready for Mixamo-compatible auto-rig";
    task.prompt = prompt.str();
    return task;
}

Mesh3D Meshy3D::buildHumanoid(const MetaEchoDNA& dna) const {
    Mesh3D mesh;
    mesh.name = dna.name.empty() ? "avatar" : dna.name;
    mesh.source = "local-synthesize";
    mesh.dnaFingerprint = dna.fingerprint;
    mesh.morphNames = {"jawOpen", "mouthSmile", "mouthFrown", "eyeBlink",
                       "browUp", "browDown", "cheekRaise", "noseWrinkle"};

    mesh.bones = RigLogic().restPose().joints;

    const float faceWidth = 0.11f + (dna.get(0) - 0.5f) * 0.03f;
    const float faceHeight = 0.13f + (dna.get(4) - 0.5f) * 0.02f;
    const float faceDepth = 0.10f + (dna.get(6) - 0.5f) * 0.02f;
    const Vec3 headC{0.0f, 1.62f, 0.0f};

    constexpr int stacks = 10;
    constexpr int slices = 14;
    const int headStart = static_cast<int>(mesh.vertices.size());

    for (int i = 0; i <= stacks; ++i) {
        const float v = static_cast<float>(i) / stacks;
        const float phi = static_cast<float>(M_PI) * v;
        const float sy = std::cos(phi);
        const float r = std::sin(phi);
        for (int j = 0; j <= slices; ++j) {
            const float u = static_cast<float>(j) / slices;
            const float theta = 2.0f * static_cast<float>(M_PI) * u;
            const float sx = std::cos(theta) * r;
            const float sz = std::sin(theta) * r;
            const Vec3 local{sx * faceWidth, sy * faceHeight, sz * faceDepth};
            const Vec3 pos = headC + local;
            const Vec3 n = local.normalized();
            const int region = regionOfHead(local);
            int bone = static_cast<int>(JointId::Head);
            float w0 = 1.0f;
            int bone2 = -1;
            float w1 = 0.0f;
            if (region == 4) {
                bone = static_cast<int>(JointId::Jaw);
                bone2 = static_cast<int>(JointId::Head);
                w0 = 0.75f;
                w1 = 0.25f;
            } else if (region == 1) {
                bone = local.x < 0 ? static_cast<int>(JointId::BrowL)
                                   : static_cast<int>(JointId::BrowR);
                bone2 = static_cast<int>(JointId::Head);
                w0 = 0.55f;
                w1 = 0.45f;
            } else if (region == 2) {
                bone = local.x < 0 ? static_cast<int>(JointId::EyeL)
                                   : static_cast<int>(JointId::EyeR);
                bone2 = static_cast<int>(JointId::Head);
                w0 = 0.40f;
                w1 = 0.60f;
            }
            addVertex(mesh, pos, n, u, v, region, bone, w0, bone2, w1);
        }
    }

    const int ring = slices + 1;
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            const int a = headStart + i * ring + j;
            const int b = a + 1;
            const int c = a + ring;
            const int d = c + 1;
            addTri(mesh, a, c, b);
            addTri(mesh, b, c, d);
        }
    }

    // Neck + torso (simple lathe) so the output is an avatar, not a floating head.
    const Vec3 torsoPts[] = {
        {0.0f, 1.48f, 0.0f},
        {0.0f, 1.30f, 0.0f},
        {0.0f, 1.10f, 0.0f},
        {0.0f, 0.90f, 0.0f},
    };
    const float torsoR[] = {0.04f, 0.10f, 0.14f, 0.12f};
    const int torsoBones[] = {
        static_cast<int>(JointId::Neck),
        static_cast<int>(JointId::Spine),
        static_cast<int>(JointId::Spine),
        static_cast<int>(JointId::Root),
    };
    const int torsoStart = static_cast<int>(mesh.vertices.size());
    constexpr int tSlices = 10;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j <= tSlices; ++j) {
            const float u = static_cast<float>(j) / tSlices;
            const float theta = 2.0f * static_cast<float>(M_PI) * u;
            const Vec3 p{std::cos(theta) * torsoR[i], torsoPts[i].y, std::sin(theta) * torsoR[i] * 0.7f};
            addVertex(mesh, p, Vec3{p.x, 0.0f, p.z}.normalized(), u, 1.0f - i / 3.0f, 0,
                      torsoBones[i]);
        }
    }
    const int tRing = tSlices + 1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < tSlices; ++j) {
            const int a = torsoStart + i * tRing + j;
            const int b = a + 1;
            const int c = a + tRing;
            const int d = c + 1;
            addTri(mesh, a, c, b);
            addTri(mesh, b, c, d);
        }
    }

    return mesh;
}

Mesh3D Meshy3D::synthesize(const Live2DPose& live2d, const MetaEchoDNA& dna) const {
    Mesh3D mesh = buildHumanoid(dna);
    return bind(mesh, live2d, dna);
}

Mesh3D Meshy3D::bind(Mesh3D mesh, const Live2DPose& live2d, const MetaEchoDNA& dna) const {
    mesh.dnaFingerprint = dna.fingerprint;
    if (mesh.name.empty()) {
        mesh.name = dna.name;
    }
    // Live2D pose is applied at skin time; bind records identity + morph names.
    (void)live2d;
    if (mesh.morphNames.empty()) {
        mesh.morphNames = {"jawOpen", "mouthSmile", "mouthFrown", "eyeBlink",
                           "browUp", "browDown", "cheekRaise", "noseWrinkle"};
    }
    return mesh;
}

} // namespace Avatar
} // namespace Ultima
