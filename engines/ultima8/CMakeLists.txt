cmake_minimum_required(VERSION 3.16)
project(pentagram VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3
find_package(SDL3 REQUIRED CONFIG)

# Find other dependencies
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)

# Collect source files using glob
file(GLOB_RECURSE AUDIO_SOURCES "audio/*.cpp")

# Exclude MIDI drivers that need SDL2 threading API (to be ported later)
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*LowLevelMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*FMOplMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*FluidSynthMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*TimidityMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*UnixSeqMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*timidity/.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*MidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*XMidi.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*fmopl.*")
file(GLOB_RECURSE CONF_SOURCES "conf/*.cpp")
file(GLOB_RECURSE CONVERT_SOURCES "convert/*.cpp")
file(GLOB_RECURSE FILESYS_SOURCES "filesys/*.cpp" "filesys/*.c")
file(GLOB_RECURSE GAMES_SOURCES "games/*.cpp")
file(GLOB_RECURSE GRAPHICS_SOURCES "graphics/*.cpp")
# Exclude TTF font support (requires SDL3_ttf which is not available)
list(FILTER GRAPHICS_SOURCES EXCLUDE REGEX ".*TTFont.*")
list(FILTER GRAPHICS_SOURCES EXCLUDE REGEX ".*TTFRenderedText.*")
file(GLOB_RECURSE GUMPS_SOURCES "gumps/*.cpp")
file(GLOB_RECURSE KERNEL_SOURCES "kernel/*.cpp")
# Exclude joystick support (needs SDL3 joystick API porting)
list(FILTER KERNEL_SOURCES EXCLUDE REGEX ".*Joystick.*")
file(GLOB_RECURSE MISC_SOURCES "misc/*.cpp")
file(GLOB_RECURSE USECODE_SOURCES "usecode/*.cpp")
file(GLOB_RECURSE WORLD_SOURCES "world/*.cpp")
# Tools excluded - they have their own main() functions and are standalone utilities
# file(GLOB_RECURSE TOOLS_SOURCES "tools/disasm/*.cpp" "tools/compile/*.cpp" "tools/fold/*.cpp")

# Exclude tools and system-specific files
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*CoreAudioMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*CoreMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*WindowsMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*ALSAMidiDriver.*")

# Stub implementations for disabled features
set(STUB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/JoystickStubs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/ToolStubs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/XMidiStubs.cpp
)

# Collect all sources
set(ALL_SOURCES
    pentagram.cpp
    ${AUDIO_SOURCES}
    ${CONF_SOURCES}
    ${CONVERT_SOURCES}
    ${FILESYS_SOURCES}
    ${GAMES_SOURCES}
    ${GRAPHICS_SOURCES}
    ${GUMPS_SOURCES}
    ${KERNEL_SOURCES}
    ${MISC_SOURCES}
    ${USECODE_SOURCES}
    ${WORLD_SOURCES}
    ${STUB_SOURCES}
)

# Create executable
add_executable(pentagram ${ALL_SOURCES})

# Include directories
target_include_directories(pentagram PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/midi
    ${CMAKE_CURRENT_SOURCE_DIR}/conf
    ${CMAKE_CURRENT_SOURCE_DIR}/convert
    ${CMAKE_CURRENT_SOURCE_DIR}/filesys
    ${CMAKE_CURRENT_SOURCE_DIR}/games
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/scalers
    ${CMAKE_CURRENT_SOURCE_DIR}/gumps
    ${CMAKE_CURRENT_SOURCE_DIR}/gumps/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel
    ${CMAKE_CURRENT_SOURCE_DIR}/misc
    ${CMAKE_CURRENT_SOURCE_DIR}/usecode
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/world/actors
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/disasm
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/compile
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/fold
    ${SDL3_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

# Compile definitions
target_compile_definitions(pentagram PRIVATE
    HAVE_CONFIG_H
    PENTAGRAM_NEW
)

# Link libraries
target_link_libraries(pentagram PRIVATE
    SDL3::SDL3
    ZLIB::ZLIB
    PNG::PNG
    ${FREETYPE_LIBRARIES}
    pthread
)

# Install
install(TARGETS pentagram RUNTIME DESTINATION bin)
