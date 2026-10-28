# CMakeLists.txt for NPC AI Module
# Cognitive NPC Architecture with Neuro-Symbolic Reasoning

cmake_minimum_required(VERSION 3.16)
project(UltimaNPCAI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(NPC_BUILD_TESTS "Build NPC AI tests" OFF)
option(NPC_BUILD_EXAMPLES "Build NPC AI examples" OFF)
option(NPC_USE_GNEURAL "Use GNeural-Net library" ON)
option(NPC_USE_AVATAR "Link avatar motion mesh pipeline" ON)

# Include directories
set(NPC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files - Phase 1: Foundation
set(PHASE1_SOURCES
    src/NeuralNetwork.cpp
    src/AIMLEngine.cpp
    src/Persona.cpp
)

# Source files - Phase 2: Reasoning
set(PHASE2_SOURCES
    src/TensorLogic.cpp
    src/RuleSet.cpp
    src/MemorySystem.cpp
)

# Source files - Phase 3: Social Dynamics
set(PHASE3_SOURCES
    src/RelationshipSystem.cpp
)

# Source files - Phase 4: Economy
set(PHASE4_SOURCES
    src/EconomicSystem.cpp
)

# Source files - Phase 5: Urban Dynamics
set(PHASE5_SOURCES
    src/UrbanDynamics.cpp
    src/NPCSystem.cpp
)

# Source files - Phase 6: LLM and Hybrid Dialogue
set(PHASE6_SOURCES
    src/TinyLLM.cpp
    src/HybridDialogue.cpp
)

# All sources
set(NPC_SOURCES
    ${PHASE1_SOURCES}
    ${PHASE2_SOURCES}
    ${PHASE3_SOURCES}
    ${PHASE4_SOURCES}
    ${PHASE5_SOURCES}
    ${PHASE6_SOURCES}
)

# Avatar navigation trains gneural-net on player controls over a reference map.
set(NPC_HAS_GNEURAL FALSE)
if(NPC_USE_GNEURAL AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cognitive/gneural-net/CMakeLists.txt")
    list(APPEND NPC_SOURCES src/AvatarNavController.cpp)
    list(APPEND NPC_HEADERS include/navigation/AvatarNavController.h)
    set(NPC_HAS_GNEURAL TRUE)
endif()

# Header files
set(NPC_HEADERS
    # Phase 1
    include/neural/NeuralNetwork.h
    include/aiml/AIMLEngine.h
    include/persona/Persona.h

    # Phase 2
    include/reasoning/TensorLogic.h
    include/reasoning/RuleSet.h
    include/memory/MemorySystem.h

    # Phase 3
    include/social/RelationshipSystem.h

    # Phase 4
    include/economy/EconomicSystem.h

    # Phase 5
    include/urban/UrbanDynamics.h
    # Phase 6
    include/llm/TinyLLM.h
    include/aiml/HybridDialogue.h
)

# Create static library
add_library(ultima_npc_ai STATIC ${NPC_SOURCES})

target_include_directories(ultima_npc_ai PUBLIC
    ${NPC_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cognitive/gneural-net/include
)

# Avatar motion mesh: meshy3d(live2d-dtecho[meta-echo-dna<rig-logic+facs>]) /deltecho
if(NPC_USE_AVATAR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../avatar/CMakeLists.txt")
    if(NOT TARGET ultima_avatar)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../avatar
                         ${CMAKE_CURRENT_BINARY_DIR}/avatar)
    endif()
    target_link_libraries(ultima_npc_ai PUBLIC ultima_avatar)
    target_compile_definitions(ultima_npc_ai PUBLIC ULTIMA_NPC_HAS_AVATAR=1)
endif()

# Link GNeural-Net when the navigation controller is part of the library.
if(NPC_HAS_GNEURAL)
    enable_language(C)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../cognitive/gneural-net
                     ${CMAKE_CURRENT_BINARY_DIR}/gneural-net)
    target_link_libraries(ultima_npc_ai PUBLIC gneural)
    target_compile_definitions(ultima_npc_ai PUBLIC ULTIMA_NPC_HAS_GNEURAL=1)
endif()

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ultima_npc_ai PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Export compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Tests
if(NPC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(NPC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# SDL3 view of the trained route. Built whenever SDL3 and gneural are present
# so the navigation controller can be exercised without the other demos.
if(NPC_HAS_GNEURAL)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL3 IMPORTED_TARGET QUIET sdl3>=3.2.0)
        if(SDL3_FOUND)
            add_executable(avatar_nav_sdl examples/avatar_nav_sdl.cpp)
            target_link_libraries(avatar_nav_sdl PRIVATE ultima_npc_ai PkgConfig::SDL3)
            target_include_directories(avatar_nav_sdl PRIVATE ${NPC_INCLUDE_DIR})
            target_compile_definitions(avatar_nav_sdl PRIVATE
                NAV_MAP_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/dock_nav.map")
        endif()
    endif()
endif()

# Install rules
install(TARGETS ultima_npc_ai
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/ultima/npc
    FILES_MATCHING PATTERN "*.h"
)

# Summary
message(STATUS "")
message(STATUS "Ultima NPC AI Configuration:")
message(STATUS "  Version:     ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Use GNeural:  ${NPC_USE_GNEURAL}")
message(STATUS "  Build Tests:  ${NPC_BUILD_TESTS}")
message(STATUS "  Build Examples: ${NPC_BUILD_EXAMPLES}")
message(STATUS "")
message(STATUS "Components:")
message(STATUS "  Phase 1 - Foundation:    Neural Network, AIML Engine, Persona")
message(STATUS "  Phase 2 - Reasoning:     Tensor Logic, Rule Set, Memory System")
message(STATUS "  Phase 3 - Social:        Relationship System, Emotional State Machine")
message(STATUS "  Phase 4 - Economy:       Economic Agents, Market Simulation")
message(STATUS "  Phase 5 - Urban:         City Simulation, OSM Integration")
message(STATUS "  Phase 6 - Dialogue:      TinyLLM, Hybrid AIML+LLM Dialogue")
message(STATUS "  Avatar motion mesh:      ${NPC_USE_AVATAR}")
message(STATUS "")
