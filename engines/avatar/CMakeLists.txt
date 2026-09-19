# CMakeLists.txt for Avatar Motion Mesh Pipeline
# { meshy3d ( live2d-dtecho [ meta-echo-dna < rig-logic + facs > ] ) => motion mesh /deltecho }

cmake_minimum_required(VERSION 3.16)
project(UltimaAvatar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(AVATAR_BUILD_TESTS "Build avatar motion mesh tests" OFF)
option(AVATAR_BUILD_EXAMPLES "Build avatar pipeline demo" OFF)

set(AVATAR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(AVATAR_SOURCES
    src/FACS.cpp
    src/RigLogic.cpp
    src/MetaEchoDNA.cpp
    src/Live2DDtecho.cpp
    src/Meshy3D.cpp
    src/MotionMesh.cpp
    src/Deltecho.cpp
    src/Pipeline.cpp
    src/PersonaBridge.cpp
)

add_library(ultima_avatar STATIC ${AVATAR_SOURCES})

target_include_directories(ultima_avatar PUBLIC
    ${AVATAR_INCLUDE_DIR}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ultima_avatar PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(AVATAR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(AVATAR_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(TARGETS ultima_avatar
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/ultima/avatar
    FILES_MATCHING PATTERN "*.h"
)

message(STATUS "")
message(STATUS "Ultima Avatar Motion Mesh:")
message(STATUS "  Composition: meshy3d(live2d-dtecho[meta-echo-dna<rig-logic+facs>]) /deltecho")
message(STATUS "  Build Tests:    ${AVATAR_BUILD_TESTS}")
message(STATUS "  Build Examples: ${AVATAR_BUILD_EXAMPLES}")
message(STATUS "")
