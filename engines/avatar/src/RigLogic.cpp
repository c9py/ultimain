#include "avatar/RigLogic.h"

namespace Ultima {
namespace Avatar {

namespace {

JointPose makeJoint(JointId id, int parent, const Vec3& bindPos) {
    JointPose j;
    j.id = id;
    j.name = RigLogic::jointName(id);
    j.parent = parent;
    j.bind.translation = bindPos;
    j.local.translation = bindPos;
    return j;
}

BlendshapeChannel makeChannel(const char* name, ActionUnit src, float weight) {
    BlendshapeChannel c;
    c.name = name;
    c.source = src;
    c.weight = saturate(weight);
    return c;
}

} // namespace

const char* RigLogic::jointName(JointId id) {
    switch (id) {
    case JointId::Root:
        return "mixamorig:Hips";
    case JointId::Spine:
        return "mixamorig:Spine";
    case JointId::Neck:
        return "mixamorig:Neck";
    case JointId::Head:
        return "mixamorig:Head";
    case JointId::Jaw:
        return "mixamorig:Jaw";
    case JointId::EyeL:
        return "mixamorig:LeftEye";
    case JointId::EyeR:
        return "mixamorig:RightEye";
    case JointId::BrowL:
        return "mixamorig:LeftBrow";
    case JointId::BrowR:
        return "mixamorig:RightBrow";
    case JointId::Count:
        break;
    }
    return "unknown";
}

const JointPose* RigPose::find(JointId id) const {
    for (const auto& j : joints) {
        if (j.id == id) {
            return &j;
        }
    }
    return nullptr;
}

JointPose* RigPose::find(JointId id) {
    for (auto& j : joints) {
        if (j.id == id) {
            return &j;
        }
    }
    return nullptr;
}

RigLogic::RigLogic() {
    buildArchetype();
}

void RigLogic::buildArchetype() {
    rest_.joints.clear();
    rest_.joints.push_back(makeJoint(JointId::Root, -1, {0.0f, 0.95f, 0.0f}));
    rest_.joints.push_back(makeJoint(JointId::Spine, 0, {0.0f, 1.25f, 0.0f}));
    rest_.joints.push_back(makeJoint(JointId::Neck, 1, {0.0f, 1.50f, 0.0f}));
    rest_.joints.push_back(makeJoint(JointId::Head, 2, {0.0f, 1.62f, 0.0f}));
    rest_.joints.push_back(makeJoint(JointId::Jaw, 3, {0.0f, 1.54f, 0.04f}));
    rest_.joints.push_back(makeJoint(JointId::EyeL, 3, {-0.035f, 1.66f, 0.09f}));
    rest_.joints.push_back(makeJoint(JointId::EyeR, 3, {0.035f, 1.66f, 0.09f}));
    rest_.joints.push_back(makeJoint(JointId::BrowL, 3, {-0.035f, 1.70f, 0.09f}));
    rest_.joints.push_back(makeJoint(JointId::BrowR, 3, {0.035f, 1.70f, 0.09f}));
}

RigPose RigLogic::evaluate(const FACSFrame& facs, const EchoDrive& drive) const {
    RigPose pose = rest_;

    const float jaw = facs.get(ActionUnit::AU26_JawDrop) * 0.35f +
                      facs.get(ActionUnit::AU27_MouthStretch) * 0.18f +
                      drive.visemeOpen * 0.12f;
    const float smile = facs.get(ActionUnit::AU12_LipCornerPuller);
    const float frown = facs.get(ActionUnit::AU15_LipCornerDepressor);
    const float blink = facs.get(ActionUnit::AU43_EyesClosed);
    const float browUp = facs.get(ActionUnit::AU1_InnerBrowRaiser) +
                         facs.get(ActionUnit::AU2_OuterBrowRaiser) * 0.5f;
    const float browDown = facs.get(ActionUnit::AU4_BrowLowerer);

    pose.jawOpen = saturate(jaw / 0.45f);
    pose.smile = saturate(smile - frown);
    pose.blink = saturate(blink);

    if (auto* head = pose.find(JointId::Head)) {
        head->local.rotation = Quat::fromEulerYxz(
            drive.headPitch * 0.35f,
            drive.headYaw * 0.45f,
            drive.headRoll * 0.20f);
    }
    if (auto* jawJ = pose.find(JointId::Jaw)) {
        jawJ->local.rotation = Quat::fromEulerYxz(jaw, 0.0f, 0.0f);
        jawJ->local.translation.y -= jaw * 0.03f;
    }
    if (auto* eyeL = pose.find(JointId::EyeL)) {
        eyeL->local.rotation = Quat::fromEulerYxz(
            -drive.gazeY * 0.25f + blink * 0.12f,
            drive.gazeX * 0.30f,
            0.0f);
    }
    if (auto* eyeR = pose.find(JointId::EyeR)) {
        eyeR->local.rotation = Quat::fromEulerYxz(
            -drive.gazeY * 0.25f + blink * 0.12f,
            drive.gazeX * 0.30f,
            0.0f);
    }
    if (auto* browL = pose.find(JointId::BrowL)) {
        browL->local.translation.y += (browUp - browDown) * 0.012f;
    }
    if (auto* browR = pose.find(JointId::BrowR)) {
        browR->local.translation.y += (browUp - browDown) * 0.012f;
    }

    pose.blendshapes.clear();
    pose.blendshapes.push_back(makeChannel("jawOpen", ActionUnit::AU26_JawDrop,
                                           facs.get(ActionUnit::AU26_JawDrop)));
    pose.blendshapes.push_back(makeChannel("mouthSmile", ActionUnit::AU12_LipCornerPuller, smile));
    pose.blendshapes.push_back(makeChannel("mouthFrown", ActionUnit::AU15_LipCornerDepressor, frown));
    pose.blendshapes.push_back(makeChannel("eyeBlink", ActionUnit::AU43_EyesClosed, blink));
    pose.blendshapes.push_back(makeChannel("browUp", ActionUnit::AU1_InnerBrowRaiser, browUp * 0.6f));
    pose.blendshapes.push_back(makeChannel("browDown", ActionUnit::AU4_BrowLowerer, browDown));
    pose.blendshapes.push_back(makeChannel("cheekRaise", ActionUnit::AU6_CheekRaiser,
                                           facs.get(ActionUnit::AU6_CheekRaiser)));
    pose.blendshapes.push_back(makeChannel("noseWrinkle", ActionUnit::AU9_NoseWrinkler,
                                           facs.get(ActionUnit::AU9_NoseWrinkler)));
    return pose;
}

} // namespace Avatar
} // namespace Ultima
