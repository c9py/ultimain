#include "test_framework.h"
#include "NPCSystem.h"

using namespace Ultima::NPC;

bool test_npc_evaluates_avatar_mesh() {
#ifdef ULTIMA_NPC_HAS_AVATAR
    NPCEntity npc("iolo");
    npc.getEmotionalState().applyEmotion(Persona::EmotionType::Happiness, 0.9, 1.0);
    const auto& frame = npc.evaluateAvatar(0.016);
    TEST_ASSERT(frame.deformed.size() > 100);
    TEST_ASSERT(frame.rest.indices.size() >= 300);
    TEST_ASSERT(npc.avatarIdentity().name == "iolo" ||
                npc.avatarIdentity().fingerprint != 0);
    TEST_ASSERT(npc.lastAvatarFrame().deformed.size() == frame.deformed.size());
    return true;
#else
    TEST_ASSERT(true);
    return true;
#endif
}

bool test_npc_update_ticks_avatar() {
#ifdef ULTIMA_NPC_HAS_AVATAR
    NPCEntity npc("dupre");
    npc.getEmotionalState().applyEmotion(Persona::EmotionType::Surprise, 0.8, 1.0);
    npc.update(0.016);
    TEST_ASSERT(npc.lastAvatarFrame().deformed.size() > 100);
    TEST_ASSERT(npc.lastAvatarFrame().mouthOpen >= 0.0f);
    return true;
#else
    TEST_ASSERT(true);
    return true;
#endif
}

int main() {
    TEST_SUITE("NPC Avatar Motion Mesh");
    RUN_TEST("evaluateAvatar mesh", test_npc_evaluates_avatar_mesh);
    RUN_TEST("update ticks avatar", test_npc_update_ticks_avatar);
    TEST_SUMMARY();
}
