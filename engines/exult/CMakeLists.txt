cmake_minimum_required(VERSION 3.16)
project(Exult VERSION 1.10.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_EXULT_STUDIO "Build Exult Studio map editor" OFF)
option(BUILD_TOOLS "Build Exult tools" OFF)
option(ENABLE_MIDI "Enable MIDI support" ON)
option(ENABLE_FLUIDSYNTH "Enable FluidSynth MIDI" OFF)
option(ENABLE_MT32EMU "Enable MT-32 emulation" OFF)

# Find dependencies using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(VORBIS vorbis vorbisfile)
pkg_check_modules(OGG ogg)
find_package(ZLIB REQUIRED)
find_package(PNG)

# Optional dependencies
pkg_check_modules(SDL3_IMAGE sdl3-image)
pkg_check_modules(FLUIDSYNTH fluidsynth)

message(STATUS "SDL3 version: ${SDL3_VERSION}")
message(STATUS "Vorbis found: ${VORBIS_FOUND}")
message(STATUS "SDL3_IMAGE found: ${SDL3_IMAGE_FOUND}")

# Generate config.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

# Common include directories
set(EXULT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/midi_drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/conf
    ${CMAKE_CURRENT_SOURCE_DIR}/files
    ${CMAKE_CURRENT_SOURCE_DIR}/files/zip
    ${CMAKE_CURRENT_SOURCE_DIR}/flic
    ${CMAKE_CURRENT_SOURCE_DIR}/gamemgr
    ${CMAKE_CURRENT_SOURCE_DIR}/gumps
    ${CMAKE_CURRENT_SOURCE_DIR}/imagewin
    ${CMAKE_CURRENT_SOURCE_DIR}/objs
    ${CMAKE_CURRENT_SOURCE_DIR}/pathfinder
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/shapes
    ${CMAKE_CURRENT_SOURCE_DIR}/shapes/shapeinf
    ${CMAKE_CURRENT_SOURCE_DIR}/usecode
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SDL3_INCLUDE_DIRS}
)

# Common compile definitions
set(EXULT_COMPILE_DEFS
    HAVE_CONFIG_H
    EXULT_DATADIR="${CMAKE_INSTALL_PREFIX}/share/exult"
)

if(VORBIS_FOUND)
    list(APPEND EXULT_COMPILE_DEFS HAVE_VORBIS)
endif()

if(SDL3_IMAGE_FOUND)
    list(APPEND EXULT_COMPILE_DEFS HAVE_SDL_IMAGE)
endif()

# ============================================================================
# Sub-libraries
# ============================================================================

# Files library
file(GLOB FILES_SOURCES
    files/*.cc
    files/sha1/*.cc
    files/zip/*.cc
)
add_library(exult_files STATIC ${FILES_SOURCES})
target_include_directories(exult_files PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_files PUBLIC ${EXULT_COMPILE_DEFS})
target_link_libraries(exult_files PUBLIC ZLIB::ZLIB)

# Configuration library
file(GLOB CONF_SOURCES conf/*.cc)
add_library(exult_conf STATIC ${CONF_SOURCES})
target_include_directories(exult_conf PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_conf PUBLIC ${EXULT_COMPILE_DEFS})

# Imagewin library
file(GLOB IMAGEWIN_SOURCES imagewin/*.cc imagewin/*.cpp)
add_library(exult_imagewin STATIC ${IMAGEWIN_SOURCES})
target_include_directories(exult_imagewin PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_imagewin PUBLIC ${EXULT_COMPILE_DEFS})
target_link_libraries(exult_imagewin PUBLIC ${SDL3_LIBRARIES})

# Shapes library
file(GLOB SHAPES_SOURCES shapes/*.cc shapes/shapeinf/*.cc)
add_library(exult_shapes STATIC ${SHAPES_SOURCES})
target_include_directories(exult_shapes PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_shapes PUBLIC ${EXULT_COMPILE_DEFS})

# Flic library
file(GLOB FLIC_SOURCES flic/*.cc)
add_library(exult_flic STATIC ${FLIC_SOURCES})
target_include_directories(exult_flic PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_flic PUBLIC ${EXULT_COMPILE_DEFS})

# Pathfinder library
file(GLOB PATHFINDER_SOURCES pathfinder/*.cc)
add_library(exult_pathfinder STATIC ${PATHFINDER_SOURCES})
target_include_directories(exult_pathfinder PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_pathfinder PUBLIC ${EXULT_COMPILE_DEFS})

# Usecode library
file(GLOB USECODE_SOURCES usecode/*.cc)
add_library(exult_usecode STATIC ${USECODE_SOURCES})
target_include_directories(exult_usecode PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_usecode PUBLIC ${EXULT_COMPILE_DEFS})

# Objs library
file(GLOB OBJS_SOURCES objs/*.cc)
add_library(exult_objs STATIC ${OBJS_SOURCES})
target_include_directories(exult_objs PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_objs PUBLIC ${EXULT_COMPILE_DEFS})

# Gumps library
file(GLOB GUMPS_SOURCES gumps/*.cc)
add_library(exult_gumps STATIC ${GUMPS_SOURCES})
target_include_directories(exult_gumps PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_gumps PUBLIC ${EXULT_COMPILE_DEFS})

# Game manager library
file(GLOB GAMEMGR_SOURCES gamemgr/*.cc)
add_library(exult_gamemgr STATIC ${GAMEMGR_SOURCES})
target_include_directories(exult_gamemgr PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_gamemgr PUBLIC ${EXULT_COMPILE_DEFS})

# Server library
file(GLOB SERVER_SOURCES server/*.cc)
add_library(exult_server STATIC ${SERVER_SOURCES})
target_include_directories(exult_server PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_server PUBLIC ${EXULT_COMPILE_DEFS})

# Audio library
file(GLOB AUDIO_SOURCES 
    audio/*.cc
    audio/midi_drivers/*.cc
    audio/midi_drivers/timidity/*.cc
)
# Exclude platform-specific files
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*CoreAudio.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*CoreMidi.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*windows.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*Windows.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*alsa.*")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*ALSA.*")

add_library(exult_audio STATIC ${AUDIO_SOURCES})
target_include_directories(exult_audio PUBLIC ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult_audio PUBLIC ${EXULT_COMPILE_DEFS})
target_link_libraries(exult_audio PUBLIC ${SDL3_LIBRARIES})
if(VORBIS_FOUND)
    target_link_libraries(exult_audio PUBLIC ${VORBIS_LIBRARIES} ${OGG_LIBRARIES})
    target_include_directories(exult_audio PUBLIC ${VORBIS_INCLUDE_DIRS})
endif()

# ============================================================================
# Main Exult executable
# ============================================================================

set(EXULT_MAIN_SOURCES
    actions.cc
    actorio.cc
    actors.cc
    args.cc
    browser.cc
    cheat.cc
    cheat_screen.cc
    combat.cc
    dir.cc
    drag.cc
    effects.cc
    exult.cc
    exultmenu.cc
    game.cc
    gameclk.cc
    gamedat.cc
    gamemap.cc
    gamerend.cc
    gamewin.cc
    istring.cc
    keys.cc
    keyactions.cc
    menulist.cc
    monsters.cc
    mouse.cc
    npcnear.cc
    npctime.cc
    palette.cc
    party.cc
    paths.cc
    playscene.cc
    readnpcs.cc
    schedule.cc
    shapeid.cc
    touchui.cc
    tqueue.cc
    txtscroll.cc
    verify.cc
    version.cc
)

add_executable(exult ${EXULT_MAIN_SOURCES})

target_include_directories(exult PRIVATE ${EXULT_INCLUDE_DIRS})
target_compile_definitions(exult PRIVATE ${EXULT_COMPILE_DEFS})

target_link_libraries(exult PRIVATE
    exult_audio
    exult_conf
    exult_files
    exult_flic
    exult_gamemgr
    exult_gumps
    exult_imagewin
    exult_objs
    exult_pathfinder
    exult_server
    exult_shapes
    exult_usecode
    ${SDL3_LIBRARIES}
    ZLIB::ZLIB
    pthread
)

if(VORBIS_FOUND)
    target_link_libraries(exult PRIVATE ${VORBIS_LIBRARIES} ${OGG_LIBRARIES})
endif()

if(SDL3_IMAGE_FOUND)
    target_link_libraries(exult PRIVATE ${SDL3_IMAGE_LIBRARIES})
endif()

if(PNG_FOUND)
    target_link_libraries(exult PRIVATE PNG::PNG)
endif()

# ============================================================================
# Installation
# ============================================================================

install(TARGETS exult RUNTIME DESTINATION bin)

# Summary
message(STATUS "")
message(STATUS "Exult Build Configuration:")
message(STATUS "  Version:        ${PROJECT_VERSION}")
message(STATUS "  SDL3:           ${SDL3_VERSION}")
message(STATUS "  Vorbis:         ${VORBIS_FOUND}")
message(STATUS "  SDL3_IMAGE:     ${SDL3_IMAGE_FOUND}")
message(STATUS "  PNG:            ${PNG_FOUND}")
message(STATUS "  Exult Studio:   ${BUILD_EXULT_STUDIO}")
message(STATUS "  Tools:          ${BUILD_TOOLS}")
message(STATUS "")
