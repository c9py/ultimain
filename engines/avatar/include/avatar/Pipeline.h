/**
 * Pipeline.h - Composed avatar motion mesh:
 *
 *   { /meshy3d ( /live2d-dtecho [ /meta-echo-dna < /rig-logic + /facs > ] )
 *       => avatar motion mesh /deltecho }
 */

#ifndef ULTIMA_AVATAR_PIPELINE_H
#define ULTIMA_AVATAR_PIPELINE_H

#include "avatar/Deltecho.h"

namespace Ultima {
namespace Avatar {

struct PipelineStages {
    FACSFrame facs;
    RigPose rig;
    MetaEchoDNA dna;
    Live2DPose live2d;
    MeshyTask meshyTask;
    Mesh3D mesh;
    MotionFrame motion;
    MotionFrame echoed;
};

class Pipeline {
public:
    Pipeline();
    explicit Pipeline(const std::string& identityName);

    /**
     * Encode a stable identity from rest-pose rig+facs and Echo persona.
     */
    const MetaEchoDNA& encodeIdentity(const EchoState& persona, const std::string& name);

    /**
     * Evaluate the full composition for one frame.
     */
    const MotionFrame& evaluate(const EchoDrive& drive, float dt);

    /**
     * Same as evaluate, but also returns every intermediate stage.
     */
    PipelineStages evaluateDetailed(const EchoDrive& drive, float dt);

    const MetaEchoDNA& identity() const { return identity_; }
    const MotionFrame& lastFrame() const { return lastFrame_; }
    Deltecho& deltecho() { return deltecho_; }
    const Deltecho& deltecho() const { return deltecho_; }

    bool hasIdentity() const { return hasIdentity_; }

private:
    FACS facs_;
    RigLogic rigLogic_;
    MetaEchoCodec codec_;
    Live2DDtecho live2d_;
    Meshy3D meshy_;
    Deltecho deltecho_;
    MetaEchoDNA identity_;
    MotionFrame lastFrame_;
    bool hasIdentity_ = false;
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_PIPELINE_H
