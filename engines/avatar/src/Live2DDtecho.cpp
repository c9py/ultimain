#include "avatar/Live2DDtecho.h"

namespace Ultima {
namespace Avatar {

Live2DPose Live2DDtecho::drive(const MetaEchoDNA& dna,
                               const FACSFrame& facs,
                               const EchoDrive& drive) const {
    Live2DPose pose;

    const float identityBias = (dna.get(0) - 0.5f) * 0.15f;
    const float extraMotion = dna.persona.get(EchoDimension::Adaptability) * 0.10f;
    const float gestalt = dna.persona.get(EchoDimension::Gestalt);
    const float threshold = drive.echo.attentionThreshold();

    const float mouthForm = facs.get(ActionUnit::AU12_LipCornerPuller) -
                            facs.get(ActionUnit::AU15_LipCornerDepressor);
    const float mouthOpen = facs.get(ActionUnit::AU25_LipsPart) * 0.40f +
                            facs.get(ActionUnit::AU26_JawDrop) * 0.70f +
                            facs.get(ActionUnit::AU27_MouthStretch);
    const float eyeClose = facs.get(ActionUnit::AU43_EyesClosed) +
                           facs.get(ActionUnit::AU7_LidTightener) * 0.30f;
    const float brow = facs.get(ActionUnit::AU1_InnerBrowRaiser) +
                       facs.get(ActionUnit::AU2_OuterBrowRaiser) * 0.50f -
                       facs.get(ActionUnit::AU4_BrowLowerer);

    pose.set(Live2DParam::AngleX, clampf(drive.headYaw + identityBias, -1.0f, 1.0f));
    pose.set(Live2DParam::AngleY, clampf(drive.headPitch, -1.0f, 1.0f));
    pose.set(Live2DParam::AngleZ, clampf(drive.headRoll * 0.6f, -1.0f, 1.0f));

    pose.set(Live2DParam::EyeLOpen, saturate(1.0f - eyeClose));
    pose.set(Live2DParam::EyeROpen, saturate(1.0f - eyeClose * 0.98f));
    pose.set(Live2DParam::EyeBallX, clampf(drive.gazeX, -1.0f, 1.0f));
    pose.set(Live2DParam::EyeBallY, clampf(drive.gazeY, -1.0f, 1.0f));

    pose.set(Live2DParam::BrowLY, clampf(brow, -1.0f, 1.0f));
    pose.set(Live2DParam::BrowRY, clampf(brow * 0.95f, -1.0f, 1.0f));
    pose.set(Live2DParam::BrowLForm, clampf(facs.get(ActionUnit::AU4_BrowLowerer) * -0.8f, -1.0f, 1.0f));
    pose.set(Live2DParam::BrowRForm, clampf(facs.get(ActionUnit::AU4_BrowLowerer) * -0.8f, -1.0f, 1.0f));

    pose.set(Live2DParam::MouthOpenY, saturate(mouthOpen));
    pose.set(Live2DParam::MouthForm, clampf(mouthForm, -1.0f, 1.0f));
    pose.set(Live2DParam::Cheek, saturate(facs.get(ActionUnit::AU6_CheekRaiser)));

    pose.set(Live2DParam::BodyAngleX,
             clampf(drive.headYaw * 0.25f + (gestalt - 0.5f) * extraMotion, -1.0f, 1.0f));

    // Breath / living motion from Echo energy, gated by attention.
    const float breath = saturate(drive.echo.energy * 0.35f + (1.0f - threshold) * 0.15f);
    pose.set(Live2DParam::Breath, breath);

    return pose;
}

const char* Live2DDtecho::name(Live2DParam p) {
    switch (p) {
    case Live2DParam::AngleX:
        return "ParamAngleX";
    case Live2DParam::AngleY:
        return "ParamAngleY";
    case Live2DParam::AngleZ:
        return "ParamAngleZ";
    case Live2DParam::EyeLOpen:
        return "ParamEyeLOpen";
    case Live2DParam::EyeROpen:
        return "ParamEyeROpen";
    case Live2DParam::EyeBallX:
        return "ParamEyeBallX";
    case Live2DParam::EyeBallY:
        return "ParamEyeBallY";
    case Live2DParam::BrowLY:
        return "ParamBrowLY";
    case Live2DParam::BrowRY:
        return "ParamBrowRY";
    case Live2DParam::BrowLForm:
        return "ParamBrowLForm";
    case Live2DParam::BrowRForm:
        return "ParamBrowRForm";
    case Live2DParam::MouthOpenY:
        return "ParamMouthOpenY";
    case Live2DParam::MouthForm:
        return "ParamMouthForm";
    case Live2DParam::Cheek:
        return "ParamCheek";
    case Live2DParam::BodyAngleX:
        return "ParamBodyAngleX";
    case Live2DParam::Breath:
        return "ParamBreath";
    }
    return "ParamUnknown";
}

std::vector<Live2DParam> Live2DDtecho::allParams() {
    std::vector<Live2DParam> params;
    params.reserve(kLive2DCount);
    for (int i = 0; i < kLive2DCount; ++i) {
        params.push_back(static_cast<Live2DParam>(i));
    }
    return params;
}

} // namespace Avatar
} // namespace Ultima
