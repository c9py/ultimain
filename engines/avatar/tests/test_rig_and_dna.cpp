#include "test_framework.h"
#include "avatar/MetaEchoDNA.h"

#include <cmath>

using namespace Ultima::Avatar;

bool test_rig_has_mixamo_joints() {
    RigLogic rig;
    TEST_ASSERT_EQUAL(static_cast<long long>(JointId::Count),
                      static_cast<long long>(rig.restPose().joints.size()));
    TEST_ASSERT_STRING_EQUAL("mixamorig:Jaw", RigLogic::jointName(JointId::Jaw));
    TEST_ASSERT_STRING_EQUAL("mixamorig:Head", RigLogic::jointName(JointId::Head));
    return true;
}

bool test_jaw_responds_to_au26() {
    FACSFrame facs;
    facs.set(ActionUnit::AU26_JawDrop, 1.0f);
    EchoDrive drive;
    const RigPose posed = RigLogic().evaluate(facs, drive);
    const RigPose rest = RigLogic().restPose();
    const auto* posedJaw = posed.find(JointId::Jaw);
    const auto* restJaw = rest.find(JointId::Jaw);
    TEST_ASSERT_NOT_NULL(posedJaw);
    TEST_ASSERT_NOT_NULL(restJaw);
    TEST_ASSERT(posed.jawOpen > 0.5f);
    TEST_ASSERT(posedJaw->local.translation.y < restJaw->local.translation.y);
    return true;
}

bool test_head_follows_drive() {
    EchoDrive drive;
    drive.headYaw = 0.8f;
    const RigPose posed = RigLogic().evaluate(FACSFrame{}, drive);
    const auto* head = posed.find(JointId::Head);
    TEST_ASSERT_NOT_NULL(head);
    TEST_ASSERT(std::fabs(head->local.rotation.y) > 0.05f);
    return true;
}

bool test_dna_fingerprint_stable() {
    MetaEchoCodec codec;
    EchoState persona;
    persona.dimensions[0] = 0.8f;
    const MetaEchoDNA a = codec.encode(RigLogic().restPose(), FACSFrame{}, persona, "iolo");
    const MetaEchoDNA b = codec.encode(RigLogic().restPose(), FACSFrame{}, persona, "iolo");
    TEST_ASSERT_EQUAL(static_cast<long long>(a.fingerprint),
                      static_cast<long long>(b.fingerprint));
    TEST_ASSERT(a.fingerprint != 0);
    TEST_ASSERT_STRING_EQUAL("iolo", a.name);
    return true;
}

bool test_dna_changes_with_facs_rest() {
    MetaEchoCodec codec;
    FACSFrame smile = FACS::recipe(Emotion::Happiness, 0.4f);
    const MetaEchoDNA a = codec.encode(RigLogic().restPose(), FACSFrame{}, EchoState{}, "a");
    const MetaEchoDNA b = codec.encode(RigLogic().restPose(), smile, EchoState{}, "b");
    TEST_ASSERT(a.fingerprint != b.fingerprint);
    return true;
}

bool test_dna_blend_and_decode() {
    MetaEchoCodec codec;
    FACSFrame sad = FACS::recipe(Emotion::Sadness, 1.0f);
    const MetaEchoDNA a = codec.encode(RigLogic().restPose(), FACSFrame{}, EchoState{}, "a");
    const MetaEchoDNA b = codec.encode(RigLogic().restPose(), sad, EchoState{}, "b");
    const MetaEchoDNA mid = codec.blend(a, b, 0.5f);
    TEST_ASSERT(mid.fingerprint != a.fingerprint);
    TEST_ASSERT(mid.fingerprint != b.fingerprint);
    const FACSFrame decoded = codec.decodeRestFacs(b);
    TEST_ASSERT_FLOAT_NEAR(sad.get(ActionUnit::AU15_LipCornerDepressor),
                           decoded.get(ActionUnit::AU15_LipCornerDepressor), 0.02);
    return true;
}

int main() {
    TEST_SUITE("RigLogic+MetaEchoDNA");
    RUN_TEST("mixamo joints", test_rig_has_mixamo_joints);
    RUN_TEST("jaw from AU26", test_jaw_responds_to_au26);
    RUN_TEST("head from drive", test_head_follows_drive);
    RUN_TEST("dna fingerprint", test_dna_fingerprint_stable);
    RUN_TEST("dna facs identity", test_dna_changes_with_facs_rest);
    RUN_TEST("dna blend decode", test_dna_blend_and_decode);
    TEST_SUMMARY();
}
