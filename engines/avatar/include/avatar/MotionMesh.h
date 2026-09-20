/**
 * MotionMesh.h - Skinned avatar mesh with FACS / Live2D / Rig pose applied.
 *
 * Output type of the composition: => avatar motion mesh
 */

#ifndef ULTIMA_AVATAR_MOTION_MESH_H
#define ULTIMA_AVATAR_MOTION_MESH_H

#include "avatar/Meshy3D.h"

namespace Ultima {
namespace Avatar {

struct MotionFrame {
    Mesh3D rest;
    std::vector<Vec3> deformed;
    std::vector<Vec3> normals;
    RigPose rig;
    FACSFrame facs;
    Live2DPose live2d;
    MetaEchoDNA dna;
    float mouthOpen = 0.0f;
    float smile = 0.0f;
    float blink = 0.0f;
};

class MotionMesh {
public:
    static MotionFrame skin(const Mesh3D& mesh,
                            const RigPose& rig,
                            const FACSFrame& facs,
                            const Live2DPose& live2d,
                            const MetaEchoDNA& dna);

    static std::string toObj(const MotionFrame& frame);
    static std::string toJson(const MotionFrame& frame, const EchoDrive& drive);

    static float vertexDisplacement(const MotionFrame& frame);
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_MOTION_MESH_H
