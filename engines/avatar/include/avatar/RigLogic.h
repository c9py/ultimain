/**
 * RigLogic.h - MetaHuman-style DNA rig evaluation.
 *
 * Innermost operand of the composition: < /rig-logic + /facs >
 * Controls + FACS map onto joints and blendshape channels.
 */

#ifndef ULTIMA_AVATAR_RIG_LOGIC_H
#define ULTIMA_AVATAR_RIG_LOGIC_H

#include "avatar/FACS.h"

#include <array>
#include <string>
#include <vector>

namespace Ultima {
namespace Avatar {

enum class JointId {
    Root = 0,
    Spine,
    Neck,
    Head,
    Jaw,
    EyeL,
    EyeR,
    BrowL,
    BrowR,
    Count
};

struct JointPose {
    JointId id = JointId::Root;
    std::string name;
    int parent = -1;
    Transform bind;
    Transform local;
};

struct BlendshapeChannel {
    std::string name;
    ActionUnit source = ActionUnit::AU26_JawDrop;
    float weight = 0.0f;
};

struct RigPose {
    std::vector<JointPose> joints;
    std::vector<BlendshapeChannel> blendshapes;
    float jawOpen = 0.0f;
    float blink = 0.0f;
    float smile = 0.0f;

    const JointPose* find(JointId id) const;
    JointPose* find(JointId id);
};

/**
 * DNA-backed rig definition (joints, control maps, RBF-like pose blending).
 */
class RigLogic {
public:
    RigLogic();

    RigPose evaluate(const FACSFrame& facs, const EchoDrive& drive) const;

    const RigPose& restPose() const { return rest_; }

    static const char* jointName(JointId id);

private:
    RigPose rest_;
    void buildArchetype();
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_RIG_LOGIC_H
