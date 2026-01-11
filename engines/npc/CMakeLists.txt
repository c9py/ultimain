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
)

# All sources
set(NPC_SOURCES
    ${PHASE1_SOURCES}
    ${PHASE2_SOURCES}
    ${PHASE3_SOURCES}
    ${PHASE4_SOURCES}
    ${PHASE5_SOURCES}
)

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
)

# Create static library
add_library(ultima_npc_ai STATIC ${NPC_SOURCES})

target_include_directories(ultima_npc_ai PUBLIC
    ${NPC_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cognitive/gneural-net/include
)

# Link GNeural-Net if available
if(NPC_USE_GNEURAL)
    # Add GNeural-Net as subdirectory if needed
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cognitive/gneural-net/CMakeLists.txt")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../cognitive/gneural-net
                         ${CMAKE_CURRENT_BINARY_DIR}/gneural-net)
        target_link_libraries(ultima_npc_ai PUBLIC gneural)
    endif()
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
message(STATUS "")
