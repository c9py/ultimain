#include "avatar/Pipeline.h"

namespace Ultima {
namespace Avatar {

Pipeline::Pipeline() : Pipeline("avatar") {}

Pipeline::Pipeline(const std::string& identityName) {
    EchoState persona;
    persona.dimensions[static_cast<int>(EchoDimension::Identity)] = 0.62f;
    persona.dimensions[static_cast<int>(EchoDimension::Memory)] = 0.70f;
    persona.dimensions[static_cast<int>(EchoDimension::Gestalt)] = 0.58f;
    persona.dimensions[static_cast<int>(EchoDimension::Purpose)] = 0.66f;
    encodeIdentity(persona, identityName);
}

const MetaEchoDNA& Pipeline::encodeIdentity(const EchoState& persona, const std::string& name) {
    EchoDrive rest;
    rest.echo = persona;
    rest.identityName = name;
    const FACSFrame facs = facs_.evaluate(rest);
    const RigPose rig = rigLogic_.evaluate(facs, rest);
    identity_ = codec_.encode(rig, facs, persona, name);
    hasIdentity_ = true;
    deltecho_.reset();
    return identity_;
}

PipelineStages Pipeline::evaluateDetailed(const EchoDrive& drive, float dt) {
    EchoDrive local = drive;
    if (local.identityName.empty()) {
        local.identityName = identity_.name;
    }
    if (!hasIdentity_) {
        encodeIdentity(local.echo, local.identityName);
    }

    PipelineStages s;
    s.facs = facs_.evaluate(local);
    s.rig = rigLogic_.evaluate(s.facs, local);
    // Identity stays stable; frame still re-encodes a *view* of (rig+facs)
    // so callers can inspect the innermost <rig-logic + facs> pair.
    s.dna = identity_;
    s.live2d = live2d_.drive(s.dna, s.facs, local);
    s.meshyTask = meshy_.describe(s.live2d, s.dna);
    s.mesh = meshy_.synthesize(s.live2d, s.dna);
    s.motion = MotionMesh::skin(s.mesh, s.rig, s.facs, s.live2d, s.dna);
    s.echoed = deltecho_.apply(s.motion, local, dt);
    lastFrame_ = s.echoed;
    return s;
}

const MotionFrame& Pipeline::evaluate(const EchoDrive& drive, float dt) {
    evaluateDetailed(drive, dt);
    return lastFrame_;
}

} // namespace Avatar
} // namespace Ultima
