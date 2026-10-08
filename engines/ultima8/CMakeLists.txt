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

option(PENTAGRAM_USE_TIMIDITY_MIDI "Link the Timidity software MIDI synth" ON)
option(PENTAGRAM_USE_FMOPL_MIDI "Link the FMOPL software MIDI synth" ON)
option(PENTAGRAM_USE_SDL_TTF "Compile SDL3_ttf fonts (off: bitmap fonts from u8fonts.flx)" OFF)

# Collect source files using glob
file(GLOB_RECURSE AUDIO_SOURCES "audio/*.cpp")

# Software MIDI (Timidity, FMOPL, XMIDI) feeds AudioMixer. Platform drivers stay out.
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*XMidiStubs\\.cpp$")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*FluidSynthMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*UnixSeqMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*CoreAudioMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*CoreMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*WindowsMidiDriver.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*ALSAMidiDriver.*")
if(NOT PENTAGRAM_USE_TIMIDITY_MIDI)
    list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*/timidity/.*")
    list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*TimidityMidiDriver.*")
endif()
if(NOT PENTAGRAM_USE_FMOPL_MIDI)
    list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*FMOplMidiDriver.*")
    list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*fmopl.*")
endif()
file(GLOB_RECURSE CONF_SOURCES "conf/*.cpp")
file(GLOB_RECURSE CONVERT_SOURCES "convert/*.cpp")
file(GLOB_RECURSE FILESYS_SOURCES "filesys/*.cpp" "filesys/*.c")
file(GLOB_RECURSE GAMES_SOURCES "games/*.cpp")
file(GLOB_RECURSE GRAPHICS_SOURCES "graphics/*.cpp")
if(NOT PENTAGRAM_USE_SDL_TTF)
    # Bitmap fonts are the operational path. TTFont.cpp still calls SDL2_ttf entry points.
    list(FILTER GRAPHICS_SOURCES EXCLUDE REGEX ".*TTFont.*")
    list(FILTER GRAPHICS_SOURCES EXCLUDE REGEX ".*TTFRenderedText.*")
endif()
file(GLOB_RECURSE GUMPS_SOURCES "gumps/*.cpp")
file(GLOB_RECURSE KERNEL_SOURCES "kernel/*.cpp")
list(FILTER KERNEL_SOURCES EXCLUDE REGEX ".*JoystickStubs\\.cpp$")
file(GLOB_RECURSE MISC_SOURCES "misc/*.cpp")
file(GLOB_RECURSE USECODE_SOURCES "usecode/*.cpp")
file(GLOB_RECURSE WORLD_SOURCES "world/*.cpp")
# Tools excluded - they have their own main() functions and are standalone utilities
# file(GLOB_RECURSE TOOLS_SOURCES "tools/disasm/*.cpp" "tools/compile/*.cpp" "tools/fold/*.cpp")

# Disasm/compile processes are standalone tools. ConApp only needs the stubs.
set(STUB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/ToolStubs.cpp
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

# Compile definitions. Feature macros must match the sources actually linked.
target_compile_definitions(pentagram PRIVATE
    HAVE_CONFIG_H
    PENTAGRAM_NEW
)
if(PENTAGRAM_USE_TIMIDITY_MIDI)
    target_compile_definitions(pentagram PRIVATE USE_TIMIDITY_MIDI)
endif()
if(PENTAGRAM_USE_FMOPL_MIDI)
    target_compile_definitions(pentagram PRIVATE USE_FMOPL_MIDI)
endif()
if(PENTAGRAM_USE_SDL_TTF)
    find_package(SDL3_ttf REQUIRED CONFIG)
    target_compile_definitions(pentagram PRIVATE USE_SDL_TTF)
    target_link_libraries(pentagram PRIVATE SDL3_ttf::SDL3_ttf)
endif()

# pentagram.cpp defines WinMain on Windows and undefines SDL's main macro.
# SDL3::SDL3 also pulls SDL3main, which defines WinMain again. Link the
# shared or static SDL3 target directly so the engine entry point is used.
if(WIN32 AND TARGET SDL3::SDL3-shared)
    set(PENTAGRAM_SDL3_TARGET SDL3::SDL3-shared)
elseif(WIN32 AND TARGET SDL3::SDL3-static)
    set(PENTAGRAM_SDL3_TARGET SDL3::SDL3-static)
else()
    set(PENTAGRAM_SDL3_TARGET SDL3::SDL3)
endif()

# Link libraries
target_link_libraries(pentagram PRIVATE
    ${PENTAGRAM_SDL3_TARGET}
    ZLIB::ZLIB
    PNG::PNG
    ${FREETYPE_LIBRARIES}
    pthread
)

# Install
install(TARGETS pentagram RUNTIME DESTINATION bin)
