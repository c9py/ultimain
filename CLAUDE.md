# CLAUDE.md - Project Guide for Ultima Engines Integration

## Project Overview

This is a unified codebase integrating multiple Ultima game engines with shared components, AI-powered NPCs, and tooling. The project combines:

- **Exult Engine** - Ultima VII: The Black Gate/Serpent Isle reimplementation
- **Pentagram Engine** - Ultima VIII: Pagan reimplementation (originally from ScummVM)
- **Shared Library** - Common audio, file formats, and image scalers
- **NPC AI System** - Cognitive NPC system with TinyLLM integration
- **OSM2Ultima** - OpenStreetMap to Ultima map converter
- **Unified Launcher** - Desktop launcher for both engines
- **Web Launcher** - Browser-based launcher using CheerpX

## Build System

The project uses **CMake 3.16+** with **C++17** and requires **SDL3 3.5.0+** built from source.

### Quick Build Commands

```bash
# Root shared library
mkdir build && cd build && cmake .. && make

# Exult engine (Ultima VII)
cd engines/exult && mkdir build && cd build && cmake .. && make -j$(nproc)

# Pentagram engine (Ultima VIII)
cd engines/ultima8 && mkdir build && cd build && cmake .. && make -j$(nproc)

# NPC AI system
cd engines/npc && mkdir build && cd build && cmake .. && make

# Unified launcher
cd launcher && mkdir build && cd build && cmake .. && make
```

### Dependencies

- CMake 3.16+, GCC/G++ with C++17
- SDL3 3.5.0+ (must be built from source)
- libvorbis, libogg, zlib, libpng
- Python 3 with requests, osmium (for osm2ultima)

## Project Structure

```
ultimain/
├── cognitive/           # NPC AI and neural network systems
│   ├── agml/           # Auto-Gnosis ML framework
│   ├── gneural-net/    # GNU Neural Network (C, autotools build)
│   ├── llm/            # LLM integration
│   └── dream-vortex/   # Dream/procedural generation
├── docs/               # Architecture docs and build reports
├── engines/
│   ├── exult/          # Exult engine - full source (~15.8 MB binary)
│   ├── ultima8/        # Pentagram engine - full source (~3.0 MB binary)
│   └── npc/            # NPC AI integration library
├── launcher/           # SDL3-based unified desktop launcher
├── reference/          # Reference game data files for format analysis
│   ├── u7_static/      # Ultima VII static data (~18 MB)
│   ├── u8_static/      # Ultima VIII static data (~20 MB)
│   └── u8_usecode/     # Ultima VIII usecode (~1.3 MB)
├── research/           # Research materials and downloads
├── shared/             # Shared libraries (audio, files, scalers)
│   ├── audio/          # Audio mixing (from Pentagram)
│   ├── files/          # Flex/U7 file format handling
│   └── scalers/        # Image scalers (bilinear, hq2x, hq3x, xBR, etc.)
├── tools/
│   └── osm2ultima/     # OSM to Ultima map converter (Python)
└── web/                # CheerpX web launcher (HTML/CSS/JS)
```

## Key Files

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Root build config, builds `libultima_shared.a` |
| `engines/exult/CMakeLists.txt` | Exult engine build |
| `engines/ultima8/CMakeLists.txt` | Pentagram engine build |
| `engines/npc/CMakeLists.txt` | NPC AI library build |
| `shared/config.h` | Shared library configuration |
| `tools/osm2ultima/osm2ultima.py` | Main OSM converter script |

## Engine-Specific Notes

### Exult (Ultima VII)
- Location: `engines/exult/`
- Binary: `engines/exult/build/exult`
- Requires game data files to run (STATIC/, GAMEDAT/)
- Uses autotools for original build, CMake added for integration

### Pentagram (Ultima VIII)
- Location: `engines/ultima8/`
- Binary: `engines/ultima8/build/pentagram`
- Standalone extraction from ScummVM Ultima engine
- Requires Pagan game data to run

### NPC AI System
- Location: `engines/npc/`
- ~7,400 lines of C++ across 12 source files
- Headers in `engines/npc/include/`
- Integrates TinyLLM and hybrid dialogue systems

## Cognitive Subsystem

The `cognitive/` directory contains AI/ML components:

### gneural-net
- GNU Neural Network library (C, GPL-3.0)
- Uses GNU Autotools: `./configure && make`
- Has unit tests: `cd tests/unit && make && ./test_runner`
- See `cognitive/gneural-net/CLAUDE.md` for detailed guide

## Tools

### OSM2Ultima
Converts OpenStreetMap data to Ultima VII/VIII map format.

```bash
cd tools/osm2ultima

# Generate from place name
python osm2ultima.py --place "London, UK" --radius 500 --output london_map

# Generate from bounding box
python osm2ultima.py --bbox "-0.1,51.5,0.0,51.6" --output custom_map
```

## Development Guidelines

### Code Style
- C++17 standard
- Use CMake for new components
- Keep shared code in `shared/` directory
- Document file formats in `docs/`

### Build Artifacts
Build directories are gitignored. Each component builds in its own `build/` subdirectory.

### Testing
- Exult/Pentagram: Require game data files to test
- gneural-net: Has 69 unit tests in `tests/unit/`
- OSM2Ultima: Test data in `tools/osm2ultima/test_osm_data.json`

## Common Tasks

### Adding Shared Functionality
1. Add source files to `shared/` in appropriate subdirectory
2. Update root `CMakeLists.txt` to include new sources
3. Rebuild with `cd build && make`

### Modifying Engine Code
1. Make changes in `engines/exult/` or `engines/ultima8/`
2. Rebuild: `cd engines/<engine>/build && make`

### Format Research
- Reference game files in `reference/`
- Format documentation in `docs/` and `research/`

## Documentation

- `docs/cognitive_npc_architecture.md` - NPC AI system design
- `docs/world_map_structure.md` - Ultima map format documentation
- `docs/gis_format_comparison.md` - GIS format analysis for OSM2Ultima
- `docs/exult_build_report.md` - Exult build process notes
- `docs/pentagram_build_report.md` - Pentagram build process notes

## License

GPL - See individual components for specific license details.
