cmake_minimum_required(VERSION 3.16)

enable_testing()

# Test framework
add_library(npc_test_framework INTERFACE)
target_include_directories(npc_test_framework INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests)

# Test: Neural Network
add_executable(test_neural_network test_neural_network.cpp)
target_link_libraries(test_neural_network PRIVATE ultima_npc_ai npc_test_framework)
target_include_directories(test_neural_network PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_test(NAME NeuralNetworkTests COMMAND test_neural_network)

# Test: Memory System
add_executable(test_memory_system test_memory_system.cpp)
target_link_libraries(test_memory_system PRIVATE ultima_npc_ai npc_test_framework)
target_include_directories(test_memory_system PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_test(NAME MemorySystemTests COMMAND test_memory_system)

# Test: Persona
add_executable(test_persona test_persona.cpp)
target_link_libraries(test_persona PRIVATE ultima_npc_ai npc_test_framework)
target_include_directories(test_persona PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_test(NAME PersonaTests COMMAND test_persona)

# Test: TensorLogic
add_executable(test_tensor_logic test_tensor_logic.cpp)
target_link_libraries(test_tensor_logic PRIVATE ultima_npc_ai npc_test_framework)
target_include_directories(test_tensor_logic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_test(NAME TensorLogicTests COMMAND test_tensor_logic)

# Test: Avatar motion mesh (when linked)
add_executable(test_avatar_pipeline test_avatar_pipeline.cpp)
target_link_libraries(test_avatar_pipeline PRIVATE ultima_npc_ai npc_test_framework)
target_include_directories(test_avatar_pipeline PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_test(NAME AvatarPipelineTests COMMAND test_avatar_pipeline)

set(NPC_TEST_TARGETS
    test_neural_network
    test_memory_system
    test_persona
    test_tensor_logic
    test_avatar_pipeline
)

if(NPC_HAS_GNEURAL)
    add_executable(test_avatar_nav test_avatar_nav.cpp)
    target_link_libraries(test_avatar_nav PRIVATE ultima_npc_ai npc_test_framework)
    target_include_directories(test_avatar_nav PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_compile_definitions(test_avatar_nav PRIVATE
        NAV_MAP_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../data/dock_nav.map")
    add_test(NAME AvatarNavTests COMMAND test_avatar_nav)
    list(APPEND NPC_TEST_TARGETS test_avatar_nav)
endif()

# All tests target
add_custom_target(npc_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${NPC_TEST_TARGETS}
)
