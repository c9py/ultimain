#include "test_framework.h"
#include "avatar/MotionMesh.h"

using namespace Ultima::Avatar;

bool test_live2d_smile_and_mouth() {
    MetaEchoDNA dna;
    dna.name = "dupre";
    FACSFrame facs = FACS::recipe(Emotion::Happiness, 1.0f);
    EchoDrive drive;
    drive.emotion = Emotion::Happiness;
    drive.emotionIntensity = 1.0f;
    const Live2DPose pose = Live2DDtecho().drive(dna, facs, drive);
    TEST_ASSERT(pose.get(Live2DParam::MouthForm) > 0.5f);
    TEST_ASSERT(pose.get(Live2DParam::EyeLOpen) > 0.7f);
    TEST_ASSERT_STRING_EQUAL("ParamMouthForm", Live2DDtecho::name(Live2DParam::MouthForm));
    return true;
}

bool test_live2d_blink_and_surprise() {
    MetaEchoDNA dna;
    FACSFrame blink;
    blink.set(ActionUnit::AU43_EyesClosed, 1.0f);
    const Live2DPose closed = Live2DDtecho().drive(dna, blink, EchoDrive{});
    TEST_ASSERT(closed.get(Live2DParam::EyeLOpen) < 0.15f);

    FACSFrame surprise = FACS::recipe(Emotion::Surprise, 1.0f);
    EchoDrive drive;
    drive.emotion = Emotion::Surprise;
    drive.emotionIntensity = 1.0f;
    const Live2DPose open = Live2DDtecho().drive(dna, surprise, drive);
    TEST_ASSERT(open.get(Live2DParam::MouthOpenY) > 0.35f);
    TEST_ASSERT(open.get(Live2DParam::BrowLY) > 0.4f);
    return true;
}

bool test_meshy_task_and_mesh() {
    MetaEchoCodec codec;
    EchoState persona;
    persona.dimensions[0] = 0.7f;
    const MetaEchoDNA dna = codec.encode(RigLogic().restPose(), FACSFrame{}, persona, "shamino");
    Live2DPose live2d;
    live2d.set(Live2DParam::MouthOpenY, 0.4f);
    Meshy3D meshy;
    const MeshyTask task = meshy.describe(live2d, dna);
    TEST_ASSERT(task.prompt.find("shamino") != std::string::npos);
    TEST_ASSERT_STRING_EQUAL("humanoid", task.topology);
    TEST_ASSERT(task.wantsAutoRig);

    const Mesh3D mesh = meshy.synthesize(live2d, dna);
    TEST_ASSERT(mesh.vertices.size() > 100);
    TEST_ASSERT(mesh.indices.size() >= 300);
    TEST_ASSERT(mesh.bones.size() == static_cast<size_t>(JointId::Count));
    TEST_ASSERT_EQUAL(static_cast<long long>(dna.fingerprint),
                      static_cast<long long>(mesh.dnaFingerprint));
    bool sawMouth = false;
    bool sawJaw = false;
    for (const auto& v : mesh.vertices) {
        if (v.region == 3) {
            sawMouth = true;
        }
        if (v.region == 4) {
            sawJaw = true;
        }
    }
    TEST_ASSERT(sawMouth);
    TEST_ASSERT(sawJaw);
    return true;
}

bool test_motion_mesh_smile_moves_mouth() {
    MetaEchoDNA dna;
    dna.name = "avatar";
    EchoDrive happy;
    happy.emotion = Emotion::Happiness;
    happy.emotionIntensity = 1.0f;
    const FACSFrame facsH = FACS().evaluate(happy);
    const FACSFrame facsN = FACS().evaluate(EchoDrive{});
    const Live2DPose liveH = Live2DDtecho().drive(dna, facsH, happy);
    const Live2DPose liveN = Live2DDtecho().drive(dna, facsN, EchoDrive{});
    const Mesh3D mesh = Meshy3D().synthesize(liveN, dna);
    const RigPose rigH = RigLogic().evaluate(facsH, happy);
    const RigPose rigN = RigLogic().evaluate(facsN, EchoDrive{});
    const MotionFrame n = MotionMesh::skin(mesh, rigN, facsN, liveN, dna);
    const MotionFrame h = MotionMesh::skin(mesh, rigH, facsH, liveH, dna);
    TEST_ASSERT(MotionMesh::vertexDisplacement(h) > MotionMesh::vertexDisplacement(n));
    TEST_ASSERT(h.smile > n.smile);
    float mouthMove = 0.0f;
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        if (mesh.vertices[i].region == 3) {
            mouthMove += (h.deformed[i] - n.deformed[i]).length();
        }
    }
    TEST_ASSERT(mouthMove > 0.01f);
    TEST_ASSERT(MotionMesh::toObj(h).find("v ") != std::string::npos);
    TEST_ASSERT(MotionMesh::toJson(h, happy).find("happiness") != std::string::npos);
    return true;
}

int main() {
    TEST_SUITE("Live2D-DTECHO+Meshy3D+MotionMesh");
    RUN_TEST("live2d smile", test_live2d_smile_and_mouth);
    RUN_TEST("live2d blink surprise", test_live2d_blink_and_surprise);
    RUN_TEST("meshy mesh", test_meshy_task_and_mesh);
    RUN_TEST("motion smile displacement", test_motion_mesh_smile_moves_mouth);
    TEST_SUMMARY();
}
