# Ultima Engines Integration

This repository contains an integrated codebase for the ScummVM Ultima VIII (Pagan) engine and the Exult (Ultima VII) engine, with a focus on sharing common components through a unified shared library built with SDL3. It also includes a unified launcher and a web-based launcher powered by CheerpX.

## Project Components

| Component | Description |
|---|---|
| **Shared Library** | `libultima_shared.a` with common audio, file formats, and scalers. |
| **Exult Engine** | Full source code for the Exult engine, ready to be built with CMake. |
| **ScummVM Ultima8** | Full source code for the ScummVM Ultima 8 engine. |
| **Unified Launcher** | A desktop launcher built with SDL3 to run both engines. |
| **Web Launcher** | An HTML/CSS/JS interface for launching games in the browser via CheerpX. |
| **OSM2Ultima** | Tool to convert OpenStreetMap data to Ultima VII/VIII game maps. |
| **NPC AI System** | Cognitive NPC system with TinyLLM and Hybrid Dialogue integration. |

## Build Status

- **Shared Library**: Compiles successfully with SDL3 3.5.0.
- **Exult Engine**: ✅ **Successfully builds** with SDL3 3.5.0 (15.8 MB binary).
- **Pentagram (Ultima 8)**: ✅ **Successfully builds** with SDL3 3.5.0 (3.0 MB binary).
- **Unified Launcher**: Compiles and runs, but requires a display environment.
- **Web Launcher**: Ready for deployment.
- **NPC AI System**: ~7,400 lines of C++ code across 12 source files.

## How to Build and Run

### 1. Dependencies

- **CMake** 3.16+
- **GCC/G++** with C++17 support
- **SDL3** 3.5.0+ (must be built from source)
- **libvorbis**, **libogg**
- **zlib**, **libpng**

```bash
# Install build tools and audio libraries
sudo apt-get update
sudo apt-get install -y cmake g++ build-essential pkg-config libvorbis-dev libogg-dev zlib1g-dev libpng-dev

# Build and install SDL3 from source
git clone --depth 1 https://github.com/libsdl-org/SDL.git SDL3
cd SDL3 && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) && sudo make install && sudo ldconfig
```

### 2. Build the Exult Engine (Ultima VII)

```bash
cd engines/exult
mkdir build && cd build
cmake ..
make -j$(nproc)

# Binary: engines/exult/build/exult
```

### 3. Build the Pentagram Engine (Ultima VIII)

```bash
cd engines/ultima8
mkdir build && cd build
cmake ..
make -j$(nproc)

# Binary: engines/ultima8/build/pentagram
```

### 4. Build the Unified Launcher

```bash
cd launcher
mkdir build && cd build
cmake ..
make
./ultima-launcher
```

### 5. Generate Maps from OpenStreetMap

```bash
cd tools/osm2ultima

# Generate a map from a place name
python osm2ultima.py --place "London, UK" --radius 500 --output london_map

# Generate a map from a bounding box
python osm2ultima.py --bbox "-0.1,51.5,0.0,51.6" --output custom_map
```

### 6. Web Launcher

Open `web/index.html` in a modern web browser. The web launcher uses CheerpX to run DOS-based Ultima games and provides a unified interface for all supported titles.

## Reference Data

The `reference/` directory contains extracted game data files from the Ultima Collection for format analysis:

- `reference/u7_static/` - Ultima VII: The Black Gate static data files (~18 MB)
- `reference/u8_static/` - Ultima VIII: Pagan static data files (~20 MB)
- `reference/u8_usecode/` - Ultima VIII usecode files (~1.3 MB)

These files are used as reference for the OSM2Ultima converter and format documentation.

## Project Structure

```
ultimain/
├── cognitive/          # NPC AI and cognitive systems
│   ├── agml/          # Auto-Gnosis ML framework
│   └── gneural-net/   # Neural network implementation
├── docs/              # Documentation and reports
├── engines/
│   ├── exult/         # Exult engine (Ultima VII)
│   ├── npc/           # NPC AI integration
│   └── ultima8/       # ScummVM Ultima 8 engine
├── launcher/          # Unified desktop launcher
├── reference/         # Reference game data files
├── research/          # Research materials and downloads
├── shared/            # Shared libraries
├── tools/
│   └── osm2ultima/    # OSM to Ultima map converter
└── web/               # Web launcher (CheerpX)
```

## Next Steps

1. ~~**Complete Exult Build**: Obtain the necessary game data files to generate the remaining FLX data and complete the Exult engine build.~~ ✅ **DONE**
2. ~~**Integrate ScummVM Ultima8**: Extract the Ultima 8 engine from the ScummVM codebase and integrate it with the unified launcher.~~ ✅ **DONE** (using original Pentagram codebase)
3. **CheerpX Integration**: Fully implement the CheerpX integration to run the C++ engines (Exult, ScummVM) in the browser via WebAssembly.
4. **Data File Management**: Create a system for managing and locating the required game data files for all engines.
5. **Expand OSM2Ultima**: Add support for more OSM features and improve the map generation quality.

## License

This project is a combination of multiple open-source projects and is licensed under the GPL. See individual components for specific license details.
