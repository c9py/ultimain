#include "avatar/MotionMesh.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace Ultima {
namespace Avatar {

namespace {

Vec3 facialDelta(const Vertex& vx, const FACSFrame& facs, const Live2DPose& live2d) {
    Vec3 d;
    const float smile = facs.get(ActionUnit::AU12_LipCornerPuller);
    const float frown = facs.get(ActionUnit::AU15_LipCornerDepressor);
    const float jaw = facs.get(ActionUnit::AU26_JawDrop);
    const float blink = 1.0f - live2d.get(Live2DParam::EyeLOpen);
    const float brow = live2d.get(Live2DParam::BrowLY);
    const float cheek = live2d.get(Live2DParam::Cheek);
    const float mouthOpen = live2d.get(Live2DParam::MouthOpenY);

    switch (vx.region) {
    case 1: // brow
        d.y += brow * 0.012f;
        d.z += facs.get(ActionUnit::AU4_BrowLowerer) * -0.004f;
        break;
    case 2: // eye
        d.y -= blink * 0.010f;
        d.z -= blink * 0.003f;
        break;
    case 3: // mouth
        d.x += (vx.position.x < 0.0f ? -1.0f : 1.0f) * (smile * 0.016f + frown * 0.006f);
        d.y += smile * 0.008f - frown * 0.010f - mouthOpen * 0.004f;
        d.z += mouthOpen * 0.006f;
        break;
    case 4: // jaw
        d.y -= (jaw + mouthOpen) * 0.018f;
        d.z += jaw * 0.006f;
        break;
    case 5: // cheek
        d.y += cheek * 0.007f;
        d.x += (vx.position.x < 0.0f ? -1.0f : 1.0f) * cheek * 0.006f;
        break;
    default:
        break;
    }
    return d;
}

Transform jointWorld(const RigPose& rig, int index) {
    if (index < 0 || index >= static_cast<int>(rig.joints.size())) {
        return {};
    }
    const JointPose& j = rig.joints[index];
    if (j.parent < 0) {
        return j.local;
    }
    // Local translations are stored in world bind space; apply only rotation
    // from the posed joint so skinning stays stable on the archetype.
    return j.local;
}

Vec3 skinVertex(const Vertex& vx, const RigPose& rig, const Vec3& posed) {
    Vec3 out;
    float wsum = 0.0f;
    for (int i = 0; i < 4; ++i) {
        const int b = vx.bones[i];
        const float w = vx.weights[i];
        if (b < 0 || w <= 0.0f) {
            continue;
        }
        const Transform xf = jointWorld(rig, b);
        const JointPose* rest = nullptr;
        if (b < static_cast<int>(rig.joints.size())) {
            rest = &rig.joints[b];
        }
        Vec3 local = posed;
        if (rest) {
            local = posed - rest->bind.translation;
            local = xf.rotation.rotate(local) + rest->bind.translation;
            local = local + (xf.translation - rest->bind.translation);
        }
        out += local * w;
        wsum += w;
    }
    if (wsum < 1e-6f) {
        return posed;
    }
    return out * (1.0f / wsum);
}

} // namespace

MotionFrame MotionMesh::skin(const Mesh3D& mesh,
                             const RigPose& rig,
                             const FACSFrame& facs,
                             const Live2DPose& live2d,
                             const MetaEchoDNA& dna) {
    MotionFrame frame;
    frame.rest = mesh;
    frame.rig = rig;
    frame.facs = facs;
    frame.live2d = live2d;
    frame.dna = dna;
    frame.mouthOpen = live2d.get(Live2DParam::MouthOpenY);
    frame.smile = saturate(live2d.get(Live2DParam::MouthForm));
    frame.blink = saturate(1.0f - live2d.get(Live2DParam::EyeLOpen));
    frame.deformed.resize(mesh.vertices.size());
    frame.normals.resize(mesh.vertices.size());

    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        const Vertex& vx = mesh.vertices[i];
        const Vec3 morph = vx.position + facialDelta(vx, facs, live2d);
        frame.deformed[i] = skinVertex(vx, rig, morph);
        frame.normals[i] = vx.normal;
    }
    return frame;
}

float MotionMesh::vertexDisplacement(const MotionFrame& frame) {
    float sum = 0.0f;
    const size_t n = std::min(frame.deformed.size(), frame.rest.vertices.size());
    for (size_t i = 0; i < n; ++i) {
        sum += (frame.deformed[i] - frame.rest.vertices[i].position).length();
    }
    return n == 0 ? 0.0f : sum / static_cast<float>(n);
}

std::string MotionMesh::toObj(const MotionFrame& frame) {
    std::ostringstream out;
    out << "# ultima avatar motion mesh\n";
    out << "o " << (frame.rest.name.empty() ? "avatar" : frame.rest.name) << "\n";
    for (const auto& p : frame.deformed) {
        out << "v " << p.x << " " << p.y << " " << p.z << "\n";
    }
    for (const auto& n : frame.normals) {
        out << "vn " << n.x << " " << n.y << " " << n.z << "\n";
    }
    for (size_t i = 0; i + 2 < frame.rest.indices.size(); i += 3) {
        const int a = frame.rest.indices[i] + 1;
        const int b = frame.rest.indices[i + 1] + 1;
        const int c = frame.rest.indices[i + 2] + 1;
        out << "f " << a << "//" << a << " " << b << "//" << b << " " << c << "//" << c << "\n";
    }
    return out.str();
}

std::string MotionMesh::toJson(const MotionFrame& frame, const EchoDrive& drive) {
    std::ostringstream out;
    out << "{\n";
    out << "  \"name\": \"" << frame.rest.name << "\",\n";
    out << "  \"fingerprint\": " << frame.dna.fingerprint << ",\n";
    out << "  \"emotion\": \"" << emotionName(drive.emotion) << "\",\n";
    out << "  \"intensity\": " << drive.emotionIntensity << ",\n";
    out << "  \"mouthOpen\": " << frame.mouthOpen << ",\n";
    out << "  \"smile\": " << frame.smile << ",\n";
    out << "  \"blink\": " << frame.blink << ",\n";
    out << "  \"displacement\": " << vertexDisplacement(frame) << ",\n";
    out << "  \"vertexCount\": " << frame.deformed.size() << ",\n";
    out << "  \"triangleCount\": " << (frame.rest.indices.size() / 3) << ",\n";
    out << "  \"facs\": {";
    for (int i = 0; i < kFacsCount; ++i) {
        if (i) {
            out << ", ";
        }
        out << "\"" << FACS::name(static_cast<ActionUnit>(i)) << "\": " << frame.facs.au[i];
    }
    out << "},\n  \"live2d\": {";
    for (int i = 0; i < kLive2DCount; ++i) {
        if (i) {
            out << ", ";
        }
        out << "\"" << Live2DDtecho::name(static_cast<Live2DParam>(i)) << "\": "
            << frame.live2d.params[i];
    }
    out << "},\n  \"vertices\": [";
    for (size_t i = 0; i < frame.deformed.size(); ++i) {
        if (i) {
            out << ",";
        }
        const auto& p = frame.deformed[i];
        out << "[" << p.x << "," << p.y << "," << p.z << "]";
    }
    out << "],\n  \"indices\": [";
    for (size_t i = 0; i < frame.rest.indices.size(); ++i) {
        if (i) {
            out << ",";
        }
        out << frame.rest.indices[i];
    }
    out << "]\n}\n";
    return out.str();
}

} // namespace Avatar
} // namespace Ultima
