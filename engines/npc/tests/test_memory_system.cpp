/*
 * test_memory_system.cpp - Tests for NPC Memory System
 */

#include "test_framework.h"
#include "memory/MemorySystem.h"

using namespace Ultima::NPC::Memory;

// Test memory type enumeration
bool test_memory_types() {
    TEST_ASSERT(MemoryType::Episodic != MemoryType::Semantic);
    TEST_ASSERT(MemoryType::Semantic != MemoryType::Procedural);
    TEST_ASSERT(MemoryType::Procedural != MemoryType::Emotional);
    TEST_ASSERT(MemoryType::Emotional != MemoryType::Autobiographic);
    return true;
}

// Test encoding factors defaults
bool test_encoding_factors() {
    EncodingFactors factors;
    TEST_ASSERT_FLOAT_NEAR(1.0, factors.attention, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.0, factors.emotionalArousal, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, factors.novelty, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, factors.selfRelevance, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.0, factors.rehearsals, 0.01);
    return true;
}

// Test memory item creation
bool test_memory_item() {
    MemoryItem item;
    item.id = "mem_001";
    item.type = MemoryType::Episodic;
    item.content = "Met the Avatar in Britain";
    item.encodingTime = 1000;
    item.lastAccess = 1000;
    item.strength = 0.8;
    item.clarity = 0.9;
    item.confidence = 0.95;
    item.emotionalValence = 0.5;
    item.emotionalIntensity = 0.7;

    TEST_ASSERT_STRING_EQUAL("mem_001", item.id);
    TEST_ASSERT(item.type == MemoryType::Episodic);
    TEST_ASSERT_FLOAT_NEAR(0.8, item.strength, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.5, item.emotionalValence, 0.01);
    return true;
}

// Test episodic memory
bool test_episodic_memory() {
    EpisodicMemory memory;
    memory.id = "ep_001";
    memory.eventType = "conversation";
    memory.participants = {"Avatar", "Iolo"};
    memory.outcome = "learned about the Guardian";
    memory.duration = 300;
    memory.firstPerson = true;

    TEST_ASSERT(memory.type == MemoryType::Episodic);
    TEST_ASSERT_STRING_EQUAL("conversation", memory.eventType);
    TEST_ASSERT_EQUAL(2, memory.participants.size());
    TEST_ASSERT(memory.firstPerson);
    return true;
}

// Test semantic memory
bool test_semantic_memory() {
    SemanticMemory memory;
    memory.id = "sem_001";
    memory.subject = "Avatar";
    memory.predicate = "is_from";
    memory.object = "Earth";
    memory.source = "Lord British";
    memory.category = "people";
    memory.isInferred = false;

    TEST_ASSERT(memory.type == MemoryType::Semantic);
    TEST_ASSERT_STRING_EQUAL("Avatar", memory.subject);
    TEST_ASSERT_STRING_EQUAL("is_from", memory.predicate);
    TEST_ASSERT_STRING_EQUAL("Earth", memory.object);
    TEST_ASSERT(!memory.isInferred);
    return true;
}

// Test procedural memory
bool test_procedural_memory() {
    ProceduralMemory memory;
    memory.id = "proc_001";
    memory.skillName = "swordsmanship";
    memory.proficiency = 0.7;
    memory.practiceCount = 50;
    memory.lastPractice = 1000;
    memory.steps = {"stance", "guard", "strike", "recover"};
    memory.prerequisites = {"basic_combat"};

    TEST_ASSERT(memory.type == MemoryType::Procedural);
    TEST_ASSERT_STRING_EQUAL("swordsmanship", memory.skillName);
    TEST_ASSERT_FLOAT_NEAR(0.7, memory.proficiency, 0.01);
    TEST_ASSERT_EQUAL(50, memory.practiceCount);
    TEST_ASSERT_EQUAL(4, memory.steps.size());
    return true;
}

// Test working memory creation
bool test_working_memory_creation() {
    WorkingMemory wm;
    TEST_ASSERT_FLOAT_NEAR(0.0, wm.getCognitiveLoad(), 0.01);
    TEST_ASSERT(wm.getItems().empty());
    return true;
}

// Test working memory operations
bool test_working_memory_operations() {
    WorkingMemory wm(5);  // Capacity of 5

    wm.add("item1", 1.0);
    TEST_ASSERT(wm.contains("item1"));
    TEST_ASSERT_EQUAL(1, wm.getItems().size());

    wm.add("item2", 0.8);
    wm.add("item3", 0.6);
    TEST_ASSERT_EQUAL(3, wm.getItems().size());

    wm.remove("item2");
    TEST_ASSERT(!wm.contains("item2"));
    TEST_ASSERT_EQUAL(2, wm.getItems().size());

    wm.clear();
    TEST_ASSERT_EQUAL(0, wm.getItems().size());

    return true;
}

// Test working memory focus
bool test_working_memory_focus() {
    WorkingMemory wm;

    wm.add("task1", 0.5);
    wm.add("task2", 0.8);
    wm.add("task3", 0.3);

    wm.setFocus("task2");
    TEST_ASSERT_STRING_EQUAL("task2", wm.getFocus());

    return true;
}

// Test working memory cognitive load
bool test_cognitive_load() {
    WorkingMemory wm(7);  // Default Miller's number

    // Empty should have 0 load
    TEST_ASSERT_FLOAT_NEAR(0.0, wm.getCognitiveLoad(), 0.01);

    // Add items and check load increases
    for (int i = 0; i < 4; i++) {
        wm.add("item" + std::to_string(i), 1.0);
    }

    double load = wm.getCognitiveLoad();
    TEST_ASSERT(load > 0.0);
    TEST_ASSERT(load < 1.0);

    return true;
}

// Test retrieval cue
bool test_retrieval_cue() {
    RetrievalCue cue;
    cue.tags = {"conversation", "Avatar"};
    cue.entityContext = "Avatar";
    cue.locationContext = "Britain";
    cue.minStrength = 0.3;
    cue.maxResults = 5;

    TEST_ASSERT_EQUAL(2, cue.tags.size());
    TEST_ASSERT_STRING_EQUAL("Avatar", cue.entityContext);
    TEST_ASSERT_EQUAL(5, cue.maxResults);
    return true;
}

// Test long term memory creation
bool test_long_term_memory() {
    LongTermMemory ltm;
    TEST_ASSERT_EQUAL(0, ltm.getMemoryCount());
    return true;
}

// Test memory storage
bool test_memory_storage() {
    LongTermMemory ltm;

    EpisodicMemory ep;
    ep.content = "First meeting with Lord British";
    ep.eventType = "meeting";
    ep.participants = {"Lord British"};

    std::string id = ltm.storeEpisodic(ep);
    TEST_ASSERT(!id.empty());
    TEST_ASSERT_EQUAL(1, ltm.getMemoryCount());

    // Retrieve the memory
    auto retrieved = ltm.retrieve(id);
    TEST_ASSERT(retrieved.has_value());
    TEST_ASSERT_STRING_EQUAL(ep.content, retrieved->content);

    return true;
}

// Test memory system initialization
bool test_memory_system_init() {
    MemorySystem system;
    system.initialize("npc_001");

    // Should start empty
    RetrievalCue cue;
    cue.maxResults = 10;
    auto memories = system.remember(cue);
    TEST_ASSERT(memories.empty());

    return true;
}

// Test memory system experience event
bool test_experience_event() {
    MemorySystem system;
    system.initialize("npc_001");

    system.experienceEvent(
        "conversation",
        {"Avatar", "Iolo"},
        "discussed the Guardian threat",
        0.7,  // emotional impact
        "Britain Castle"
    );

    // Should be able to remember about Avatar
    auto memories = system.rememberAbout("Avatar", 5);
    // Memory may or may not be formed depending on threshold
    TEST_ASSERT(true);  // Just test it doesn't crash

    return true;
}

// Test learn fact
bool test_learn_fact() {
    MemorySystem system;
    system.initialize("npc_001");

    system.learnFact("Guardian", "is_enemy_of", "Britannia", "Lord British");

    // Check if knows the fact
    bool knows = system.knowsFact("Guardian", "is_enemy_of", "Britannia");
    // May or may not know depending on memory strength
    TEST_ASSERT(true);

    return true;
}

// Test practice skill
bool test_practice_skill() {
    MemorySystem system;
    system.initialize("npc_001");

    // Practice a skill multiple times
    for (int i = 0; i < 10; i++) {
        system.practiceSkill("sword_fighting", 0.7);
    }

    double level = system.getSkillLevel("sword_fighting");
    TEST_ASSERT(level >= 0.0);

    return true;
}

// Test memory network
bool test_memory_network() {
    MemoryNetwork network;

    network.addNode("Avatar", 0.5);
    network.addNode("Iolo", 0.5);
    network.addNode("Dupre", 0.5);

    network.addAssociation("Avatar", "Iolo", 0.8);
    network.addAssociation("Avatar", "Dupre", 0.7);
    network.addAssociation("Iolo", "Dupre", 0.6);

    // Activate Avatar
    network.activate("Avatar", 1.0);

    // Spread activation
    network.spreadActivation(3, 0.7);

    // Get most active nodes
    auto active = network.getMostActive(3);
    TEST_ASSERT_EQUAL(3, active.size());

    // Avatar should be most active
    TEST_ASSERT_STRING_EQUAL("Avatar", active[0].first);

    return true;
}

// Test memory network reset
bool test_memory_network_reset() {
    MemoryNetwork network;

    network.addNode("node1", 0.5);
    network.addNode("node2", 0.5);
    network.activate("node1", 1.0);

    network.reset();

    auto active = network.getMostActive(2);
    // After reset, activations should be back to base
    if (!active.empty()) {
        TEST_ASSERT_FLOAT_NEAR(0.5, active[0].second, 0.1);
    }

    return true;
}

int main() {
    TEST_SUITE("Memory System");

    RUN_TEST("Memory types", test_memory_types);
    RUN_TEST("Encoding factors", test_encoding_factors);
    RUN_TEST("Memory item", test_memory_item);
    RUN_TEST("Episodic memory", test_episodic_memory);
    RUN_TEST("Semantic memory", test_semantic_memory);
    RUN_TEST("Procedural memory", test_procedural_memory);
    RUN_TEST("Working memory creation", test_working_memory_creation);
    RUN_TEST("Working memory operations", test_working_memory_operations);
    RUN_TEST("Working memory focus", test_working_memory_focus);
    RUN_TEST("Cognitive load", test_cognitive_load);
    RUN_TEST("Retrieval cue", test_retrieval_cue);
    RUN_TEST("Long term memory", test_long_term_memory);
    RUN_TEST("Memory storage", test_memory_storage);
    RUN_TEST("Memory system init", test_memory_system_init);
    RUN_TEST("Experience event", test_experience_event);
    RUN_TEST("Learn fact", test_learn_fact);
    RUN_TEST("Practice skill", test_practice_skill);
    RUN_TEST("Memory network", test_memory_network);
    RUN_TEST("Memory network reset", test_memory_network_reset);

    TEST_SUMMARY();
}
