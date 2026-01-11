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

## Build Status

- **Shared Library**: Compiles successfully with SDL3 3.5.0.
- **Exult Engine**: Build system created, but requires game data files to be generated.
- **Unified Launcher**: Compiles and runs, but requires a display environment.
- **Web Launcher**: Ready for deployment.

## How to Build and Run

### 1. Dependencies

- **CMake** 3.16+
- **GCC/G++** with C++17 support
- **SDL3** 3.5.0+ (must be built from source)
- **libvorbis**, **libogg**

```bash
# Install build tools and audio libraries
sudo apt-get update
sudo apt-get install -y cmake g++ pkg-config libvorbis-dev libogg-dev

# Build and install SDL3 from source
git clone --depth 1 https://github.com/libsdl-org/SDL.git SDL3
cd SDL3 && mkdir build && cd build
cmake .. && make -j$(nproc) && sudo make install && sudo ldconfig
```

### 2. Build the Unified Launcher

```bash
cd launcher
mkdir build && cd build
cmake ..
make
./ultima-launcher
```

### 3. Build the Exult Engine (Partial)

*Note: Full build requires game data files not included in this repository.*

```bash
cd engines/exult
mkdir build && cd build
cmake ..
make
```

### 4. Web Launcher

Open `web/index.html` in a modern web browser. The web launcher uses CheerpX to run DOS-based Ultima games and provides a unified interface for all supported titles.

## Next Steps

1.  **Complete Exult Build**: Obtain the necessary game data files to generate the remaining FLX data and complete the Exult engine build.
2.  **Integrate ScummVM Ultima8**: Extract the Ultima 8 engine from the ScummVM codebase and integrate it with the unified launcher.
3.  **CheerpX Integration**: Fully implement the CheerpX integration to run the C++ engines (Exult, ScummVM) in the browser via WebAssembly.
4.  **Data File Management**: Create a system for managing and locating the required game data files for all engines.

## License

This project is a combination of multiple open-source projects and is licensed under the GPL. See individual components for specific license details.
