#!/usr/bin/env bash
#
# Cloud Agent environment bootstrap for the Ultima Engines Integration repo.
#
# Idempotent: safe to re-run. Installs system packages, builds and installs
# SDL3 and SDL3_ttf from source (skipped if already present), then builds the
# C/C++ components and installs Python deps for the OSM2Ultima tool.
#
# Mirrors the recipe in .github/workflows/ci.yml, which is the source of truth
# for how these components are known to build.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

# Match the CI launcher job: SDL3_ttf 3.2.2 needs SDL3 3.2.6+.
SDL_VERSION="release-3.2.6"
SDL_TTF_VERSION="release-3.2.2"
JOBS="$(nproc)"

log() { printf '\n=== %s ===\n' "$1"; }

# --- 1. System packages ---------------------------------------------------
log "Installing system packages"
export DEBIAN_FRONTEND=noninteractive
sudo apt-get update -y
sudo apt-get install -y --no-install-recommends \
  build-essential \
  g++-14 libstdc++-14-dev \
  cmake ninja-build pkg-config \
  autoconf automake libtool \
  cppcheck \
  git ca-certificates \
  python3 python3-pip \
  libvorbis-dev libogg-dev zlib1g-dev libpng-dev libfreetype-dev libharfbuzz-dev \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev \
  libxi-dev libxss-dev libwayland-dev libxkbcommon-dev libegl1-mesa-dev \
  libibus-1.0-dev \
  xvfb x11-xserver-utils ffmpeg xdotool

# --- 2. SDL3 from source (idempotent) -------------------------------------
if pkg-config --atleast-version=3.2.6 sdl3 2>/dev/null; then
  log "SDL3 already installed ($(pkg-config --modversion sdl3)); skipping build"
else
  log "Building SDL3 ${SDL_VERSION} from source"
  SDL_SRC="/tmp/SDL3"
  rm -rf "$SDL_SRC"
  git clone --depth 1 --branch "$SDL_VERSION" https://github.com/libsdl-org/SDL.git "$SDL_SRC"
  cmake -S "$SDL_SRC" -B "$SDL_SRC/build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DSDL_SHARED=ON \
    -DSDL_STATIC=ON
  cmake --build "$SDL_SRC/build" -j "$JOBS"
  sudo cmake --install "$SDL_SRC/build"
  sudo ldconfig
fi

# --- 2b. SDL3_ttf from source (idempotent) --------------------------------
# The unified launcher requires sdl3-ttf>=3.0.0 (see launcher/CMakeLists.txt).
# CI builds this from source in the build-launcher job; a system package is
# not available for SDL3_ttf on the Cloud Agent image.
if pkg-config --atleast-version=3.0.0 sdl3-ttf 2>/dev/null; then
  log "SDL3_ttf already installed ($(pkg-config --modversion sdl3-ttf)); skipping build"
else
  log "Building SDL3_ttf ${SDL_TTF_VERSION} from source"
  TTF_SRC="/tmp/SDL3_ttf"
  rm -rf "$TTF_SRC"
  git clone --depth 1 --branch "$SDL_TTF_VERSION" https://github.com/libsdl-org/SDL_ttf.git "$TTF_SRC"
  cmake -S "$TTF_SRC" -B "$TTF_SRC/build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DSDLTTF_VENDORED=OFF
  cmake --build "$TTF_SRC/build" -j "$JOBS"
  sudo cmake --install "$TTF_SRC/build"
  sudo ldconfig
fi

# --- 3. Shared library + NPC AI (with tests) ------------------------------
log "Building shared library + NPC AI module"
rm -rf build/CMakeCache.txt build/CMakeFiles
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build -j "$JOBS"

# --- 4. Standalone NPC AI module (with tests) -----------------------------
log "Building standalone NPC AI module"
cmake -S engines/npc -B engines/npc/build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DNPC_BUILD_TESTS=ON -DNPC_USE_GNEURAL=OFF
cmake --build engines/npc/build -j "$JOBS"

# --- 5. gneural-net (autotools) -------------------------------------------
log "Building gneural-net"
(
  cd cognitive/gneural-net
  autoreconf -i
  ./configure
  make -j "$JOBS"
  cd tests/unit && make
)

# --- 6. Unified launcher --------------------------------------------------
# Wipe any stale cache (CI does the same). Then require the binary to actually
# link libSDL3_ttf — a first-pass link has been observed to drop it under
# --as-needed even when cmake found the package.
build_launcher() {
  rm -rf launcher/build/CMakeCache.txt launcher/build/CMakeFiles
  cmake -S launcher -B launcher/build -G Ninja -DCMAKE_BUILD_TYPE=Release
  cmake --build launcher/build -j "$JOBS"
}

launcher_links_ttf() {
  ldd launcher/build/ultima-launcher 2>/dev/null | grep -q 'libSDL3_ttf'
}

log "Building unified launcher"
build_launcher
if ! launcher_links_ttf; then
  log "Launcher missing libSDL3_ttf; performing a clean rebuild"
  rm -rf launcher/build
  build_launcher
fi
if ! launcher_links_ttf; then
  echo "ERROR: ultima-launcher did not link libSDL3_ttf" >&2
  ldd launcher/build/ultima-launcher >&2 || true
  exit 1
fi

# --- 7. Pentagram engine (best-effort) ------------------------------------
# The Pentagram (Ultima VIII) engine currently fails to compile under C++17
# due to legacy 'register' storage-class specifiers in its source. CI treats
# this as continue-on-error ("requires additional porting work"), so we only
# attempt configuration here and never fail the bootstrap on it.
log "Configuring Pentagram engine (best-effort)"
cmake -S engines/ultima8 -B engines/ultima8/build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release || echo "Pentagram configure skipped"

# --- 8. Python tooling (OSM2Ultima) ---------------------------------------
log "Installing Python dependencies for OSM2Ultima"
python3 -m pip install --user --break-system-packages --upgrade \
  pytest pytest-cov requests || \
  python3 -m pip install --user --upgrade pytest pytest-cov requests
python3 -m pip install --user --break-system-packages osmium 2>/dev/null || \
  echo "osmium is optional; skipping"

log "Environment setup complete"
