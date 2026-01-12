cmake_minimum_required(VERSION 3.16)
project(UltimaEngines VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_ULTIMA8 "Build Ultima VIII (Pagan) engine" ON)
option(BUILD_EXULT "Build Exult (Ultima VII) engine" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTING "Build unit tests" OFF)

# Find SDL3 using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

# Find other dependencies
find_package(ZLIB QUIET)
find_package(PNG QUIET)

message(STATUS "SDL3 found: ${SDL3_VERSION}")
message(STATUS "SDL3 include dirs: ${SDL3_INCLUDE_DIRS}")
message(STATUS "SDL3 libraries: ${SDL3_LIBRARIES}")

# Shared components library
add_library(ultima_shared STATIC
    # Audio (Pentagram)
    shared/audio/AudioChannel.cc
    shared/audio/AudioMixer.cc
    shared/audio/AudioSample.cc
    shared/audio/RawAudioSample.cc
    
    # File formats
    shared/files/Flex.cc
    shared/files/U7file.cc
    shared/files/utils.cc
    
    # Scalers
    shared/scalers/BilinearScaler.cpp
    shared/scalers/BilinearScalerInternal_2x.cpp
    shared/scalers/BilinearScalerInternal_Arb.cpp
    shared/scalers/BilinearScalerInternal_X1Y12.cpp
    shared/scalers/BilinearScalerInternal_X2Y24.cpp
    shared/scalers/PointScaler.cpp
    shared/scalers/scale_2x.cc
    shared/scalers/scale_2xSaI.cc
    shared/scalers/scale_bilinear.cc
    shared/scalers/scale_hq2x.cc
    shared/scalers/scale_hq3x.cc
    shared/scalers/scale_hq4x.cc
    shared/scalers/scale_interlace.cc
    shared/scalers/scale_point.cc
    shared/scalers/scale_xbr.cc
)

target_include_directories(ultima_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/files
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/scalers
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/files
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/conf
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/imagewin
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/shapes
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/audio/midi_drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/objs
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/usecode
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/gumps
    ${CMAKE_CURRENT_SOURCE_DIR}/engines/exult/pathfinder
    ${SDL3_INCLUDE_DIRS}
)

# Link SDL3
target_link_libraries(ultima_shared PUBLIC ${SDL3_LIBRARIES})
target_compile_options(ultima_shared PUBLIC ${SDL3_CFLAGS_OTHER})

# Define Exult configuration macros
target_compile_definitions(ultima_shared PUBLIC
    EXULT_DATADIR="data"
    HAVE_CONFIG_H=1
)

# Ultima 8 engine (requires ScummVM integration)
if(BUILD_ULTIMA8)
    message(STATUS "Ultima VIII engine configured (requires ScummVM for full build)")
    # Note: Full Ultima 8 build requires ScummVM framework
    # This is a placeholder for standalone components
endif()

# Exult engine
if(BUILD_EXULT)
    message(STATUS "Exult engine configured")
    # Note: Full Exult build uses autotools
    # This provides CMake alternative for shared components
endif()

# Install rules
install(TARGETS ultima_shared
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY shared/
    DESTINATION include/ultima
    FILES_MATCHING PATTERN "*.h"
)

# Testing
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Summary
message(STATUS "")
message(STATUS "Ultima Engines Integration Configuration:")
message(STATUS "  Ultima VIII (Pagan): ${BUILD_ULTIMA8}")
message(STATUS "  Exult (Ultima VII):  ${BUILD_EXULT}")
message(STATUS "  SDL3 Version:        ${SDL3_VERSION}")
message(STATUS "  Shared library:      ultima_shared")
message(STATUS "  Build Tests:         ${BUILD_TESTING}")
message(STATUS "")
