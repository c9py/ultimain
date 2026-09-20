#include "test_framework.h"
#include "avatar/Avatar.h"

using namespace Ultima::Avatar;

bool test_persona_bridge_maps_affect() {
    AffectSample sample;
    sample.happiness = 0.9f;
    sample.valence = 0.7f;
    sample.arousal = 0.6f;
    sample.openness = 0.8f;
    sample.identityName = "jaana";
    const EchoDrive drive = PersonaBridge::fromAffect(sample);
    TEST_ASSERT(drive.emotion == Emotion::Happiness);
    TEST_ASSERT(drive.emotionIntensity > 0.8f);
    TEST_ASSERT_STRING_EQUAL("jaana", drive.identityName);
    TEST_ASSERT(drive.echo.get(EchoDimension::Adaptability) > 0.7f);
    return true;
}

bool test_deltecho_has_memory() {
    Pipeline pipe("echo");
    EchoDrive drive;
    drive.emotion = Emotion::Happiness;
    drive.emotionIntensity = 1.0f;
    const MotionFrame first = pipe.evaluate(drive, 0.016f);
    TEST_ASSERT(!pipe.deltecho().hasHistory() || pipe.deltecho().lastResidualEnergy() >= 0.0f);

    EchoDrive angry = drive;
    angry.emotion = Emotion::Anger;
    const MotionFrame second = pipe.evaluate(angry, 0.016f);
    TEST_ASSERT(pipe.deltecho().hasHistory());
    TEST_ASSERT(pipe.deltecho().lastResidualEnergy() > 0.0f);
    TEST_ASSERT(second.deformed.size() == first.deformed.size());
    TEST_ASSERT(second.deformed.size() > 100);
    return true;
}

bool test_deltecho_changes_second_frame() {
    Deltecho echo;
    MetaEchoDNA dna;
    EchoDrive drive;
    drive.emotion = Emotion::Surprise;
    drive.emotionIntensity = 1.0f;
    const FACSFrame facs = FACS().evaluate(drive);
    const Live2DPose live = Live2DDtecho().drive(dna, facs, drive);
    const Mesh3D mesh = Meshy3D().synthesize(live, dna);
    const RigPose rig = RigLogic().evaluate(facs, drive);
    const MotionFrame raw = MotionMesh::skin(mesh, rig, facs, live, dna);

    const MotionFrame a = echo.apply(raw, drive, 0.016f);
    EchoDrive next = drive;
    next.emotion = Emotion::Neutral;
    next.emotionIntensity = 0.0f;
    const FACSFrame facs2 = FACS().evaluate(next);
    const Live2DPose live2 = Live2DDtecho().drive(dna, facs2, next);
    const MotionFrame raw2 = MotionMesh::skin(mesh, RigLogic().evaluate(facs2, next), facs2, live2, dna);
    const MotionFrame b = echo.apply(raw2, next, 0.016f);

    TEST_ASSERT(echo.hasHistory());
    TEST_ASSERT(b.mouthOpen > raw2.mouthOpen * 0.5f || echo.lastResidualEnergy() > 0.01f);
    float diff = 0.0f;
    for (size_t i = 0; i < b.deformed.size(); ++i) {
        diff += (b.deformed[i] - raw2.deformed[i]).length();
    }
    TEST_ASSERT(diff > 0.0f);
    return true;
}

bool test_pipeline_composition_formula() {
    Pipeline pipe("avatar");
    TEST_ASSERT(pipe.hasIdentity());
    TEST_ASSERT(pipe.identity().fingerprint != 0);

    EchoDrive drive;
    drive.emotion = Emotion::Happiness;
    drive.emotionIntensity = 0.9f;
    drive.visemeOpen = 0.2f;
    const PipelineStages s = pipe.evaluateDetailed(drive, 0.016f);

    // <rig-logic + facs>
    TEST_ASSERT(s.facs.get(ActionUnit::AU12_LipCornerPuller) > 0.5f);
    TEST_ASSERT(s.rig.smile > 0.4f);
    TEST_ASSERT(s.rig.find(JointId::Jaw) != nullptr);

    // meta-echo-dna wraps that pair
    TEST_ASSERT_STRING_EQUAL("avatar", s.dna.name);
    TEST_ASSERT_EQUAL(static_cast<long long>(pipe.identity().fingerprint),
                      static_cast<long long>(s.dna.fingerprint));

    // live2d-dtecho driven by DNA
    TEST_ASSERT(s.live2d.get(Live2DParam::MouthForm) > 0.4f);

    // meshy3d binds the Live2D/DNA pair
    TEST_ASSERT(s.meshyTask.prompt.find("avatar") != std::string::npos);
    TEST_ASSERT(s.mesh.vertices.size() == s.motion.deformed.size());
    TEST_ASSERT_EQUAL(static_cast<long long>(s.dna.fingerprint),
                      static_cast<long long>(s.mesh.dnaFingerprint));

    // => avatar motion mesh /deltecho
    TEST_ASSERT(s.echoed.deformed.size() == s.motion.deformed.size());
    TEST_ASSERT(s.echoed.rest.indices.size() == s.mesh.indices.size());
    TEST_ASSERT(MotionMesh::vertexDisplacement(s.echoed) > 0.0f);
    return true;
}

bool test_pipeline_deterministic_identity() {
    Pipeline a("spark");
    Pipeline b("spark");
    TEST_ASSERT_EQUAL(static_cast<long long>(a.identity().fingerprint),
                      static_cast<long long>(b.identity().fingerprint));
    EchoDrive drive;
    drive.emotion = Emotion::Fear;
    drive.emotionIntensity = 0.5f;
    const MotionFrame fa = a.evaluate(drive, 0.016f);
    const MotionFrame fb = b.evaluate(drive, 0.016f);
    TEST_ASSERT_EQUAL(static_cast<long long>(fa.deformed.size()),
                      static_cast<long long>(fb.deformed.size()));
    TEST_ASSERT_FLOAT_NEAR(fa.mouthOpen, fb.mouthOpen, 0.001);
    return true;
}

int main() {
    TEST_SUITE("Deltecho+Pipeline");
    RUN_TEST("persona bridge", test_persona_bridge_maps_affect);
    RUN_TEST("deltecho memory", test_deltecho_has_memory);
    RUN_TEST("deltecho residual", test_deltecho_changes_second_frame);
    RUN_TEST("composition formula", test_pipeline_composition_formula);
    RUN_TEST("identity determinism", test_pipeline_deterministic_identity);
    TEST_SUMMARY();
}
