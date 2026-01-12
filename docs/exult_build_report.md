# Exult Engine Build Report

**Date**: January 12, 2026  
**Status**: ✅ Successfully Built

## Summary

The Exult engine has been successfully built with SDL3 3.5.0 support. The build process required several fixes to the CMakeLists.txt and generation of missing header files.

## Build Environment

| Component | Version |
|-----------|---------|
| OS | Ubuntu 22.04 |
| Compiler | GCC 11.4.0 |
| CMake | 3.22.1 |
| SDL3 | 3.5.0 (built from source) |
| libvorbis | 1.3.7 |
| zlib | 1.2.11 |
| libpng | 1.6.37 |

## Build Output

- **Binary**: `engines/exult/build/exult` (15.8 MB)
- **Static Libraries**: 9 libraries totaling ~50 MB
  - `libexult_audio.a` (7.6 MB)
  - `libexult_shapes.a` (9.8 MB)
  - `libexult_gumps.a` (8.4 MB)
  - `libexult_imagewin.a` (6.4 MB)
  - `libexult_usecode.a` (5.4 MB)
  - `libexult_objs.a` (5.1 MB)
  - `libexult_files.a` (3.5 MB)
  - `libexult_gamemgr.a` (2.2 MB)
  - `libexult_conf.a`, `libexult_flic.a`, `libexult_pathfinder.a`, `libexult_server.a`

## Issues Resolved

### 1. SDL3 API Compatibility

The Exult source code was written for SDL3, which has significant API changes from SDL2. SDL3 was built from source and installed system-wide.

### 2. Missing FLX Header Files

The build system required header files generated from FLX data archives:

- `data/exult_flx.h` - Main Exult resource indices
- `data/exult_bg_flx.h` - Black Gate resource indices
- `data/exult_si_flx.h` - Serpent Isle resource indices
- `data/bg/introsfx_mt32_flx.h` - MT-32 intro sound effects
- `data/bg/introsfx_sb_flx.h` - Sound Blaster intro sound effects

These were generated using the `expack` tool and manual header creation.

### 3. CMakeLists.txt Fixes

Several fixes were required to the CMakeLists.txt:

1. **Include Paths**: Added `data/`, `tools/`, and `data/bg/` to include directories
2. **Audio Sources**: Added `*.cpp` glob pattern for MIDI driver sources
3. **SHA1 Sources**: Added `files/sha1/*.cpp` to FILES_SOURCES
4. **Link Order**: Reordered library linking to resolve symbol dependencies

### 4. Link Order Dependencies

The final link order that resolved all symbol dependencies:

```cmake
target_link_libraries(exult PRIVATE
    exult_gamemgr
    exult_usecode
    exult_gumps
    exult_objs
    exult_shapes
    exult_audio
    exult_server
    exult_flic
    exult_imagewin
    exult_pathfinder
    exult_files
    exult_conf
    ${SDL3_LIBRARIES}
    ZLIB::ZLIB
    pthread
)
```

## Reference Data Obtained

Game data files were extracted from the Ultima Collection ISO (Archive.org) for format reference:

| Directory | Contents | Size |
|-----------|----------|------|
| `reference/u7_static/` | Ultima VII static data | 18 MB |
| `reference/u8_static/` | Ultima VIII static data | 20 MB |
| `reference/u8_usecode/` | Ultima VIII usecode | 1.3 MB |

### Key U7 Data Files

- `SHAPES.VGA` - Main shape graphics (3.9 MB)
- `USECODE` - Game scripts (1.5 MB)
- `U7MAP` - World map data (73 KB)
- `U7SPEECH.SPC` - Speech data (1.5 MB)
- `MAINSHP.FLX` - Main shapes archive (2.9 MB)
- `FACES.VGA` - NPC face graphics (1.9 MB)

## Next Steps

1. **Test Runtime**: The binary needs to be tested with actual game data files
2. **Integrate with Launcher**: Connect the Exult binary to the unified launcher
3. **OSM2Ultima Integration**: Use reference data to improve map generation
4. **Ultima 8 Build**: Apply similar fixes to the ScummVM Ultima 8 engine

## Files Modified

```
engines/exult/CMakeLists.txt
engines/exult/data/exult_flx.h (created)
engines/exult/data/exult_bg_flx.h (created)
engines/exult/data/exult_si_flx.h (created)
engines/exult/data/bg/introsfx_mt32_flx.h (created)
engines/exult/data/bg/introsfx_sb_flx.h (created)
```

## Build Commands

```bash
# Prerequisites
sudo apt-get install -y cmake g++ build-essential pkg-config \
    libvorbis-dev libogg-dev zlib1g-dev libpng-dev

# Build SDL3
git clone --depth 1 https://github.com/libsdl-org/SDL.git ~/SDL3
cd ~/SDL3 && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) && sudo make install && sudo ldconfig

# Build Exult
cd engines/exult
rm -rf build && mkdir build && cd build
cmake ..
make -j$(nproc)
```
