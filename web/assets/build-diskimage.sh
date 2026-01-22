#!/bin/bash
# Build CheerpX disk image for Ultima Engines
# This script generates the ext2 disk image containing Exult and Pentagram

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOTFS_DIR="$SCRIPT_DIR/rootfs"
OUTPUT_FILE="$SCRIPT_DIR/ultima-engines.ext2"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "=== Building CheerpX Disk Image ==="
echo "Project root: $PROJECT_ROOT"
echo "Output: $OUTPUT_FILE"

# Check for required tools
command -v genext2fs >/dev/null 2>&1 || { echo "Error: genext2fs not installed. Run: apt install genext2fs"; exit 1; }

# Check if engine binaries exist
EXULT_BIN="$PROJECT_ROOT/engines/exult/build/exult"
PENTAGRAM_BIN="$PROJECT_ROOT/engines/ultima8/build/pentagram"

if [ ! -f "$EXULT_BIN" ]; then
    echo "Error: Exult binary not found at $EXULT_BIN"
    echo "Build it first: cd engines/exult && mkdir -p build && cd build && cmake .. && make -j\$(nproc)"
    exit 1
fi

if [ ! -f "$PENTAGRAM_BIN" ]; then
    echo "Error: Pentagram binary not found at $PENTAGRAM_BIN"
    echo "Build it first: cd engines/ultima8 && mkdir -p build && cd build && cmake .. && make -j\$(nproc)"
    exit 1
fi

# Create rootfs directories
echo "Creating rootfs structure..."
mkdir -p "$ROOTFS_DIR"/{bin,lib,lib64,usr/bin,usr/lib,game,root,tmp,dev,proc,sys}

# Copy engine binaries
echo "Copying engine binaries..."
cp "$EXULT_BIN" "$ROOTFS_DIR/usr/bin/"
cp "$PENTAGRAM_BIN" "$ROOTFS_DIR/usr/bin/"

# Copy required shared libraries
echo "Copying shared libraries..."

# SDL3
if [ -f /usr/local/lib/libSDL3.so.0 ]; then
    cp /usr/local/lib/libSDL3.so.0* "$ROOTFS_DIR/lib/"
else
    echo "Warning: libSDL3.so.0 not found in /usr/local/lib"
fi

# System libraries (adjust paths as needed for your system)
LIB_SRC="/lib/x86_64-linux-gnu"
[ -f "$LIB_SRC/libz.so.1" ] && cp "$LIB_SRC/libz.so.1" "$ROOTFS_DIR/lib/"
[ -f "$LIB_SRC/libpng16.so.16" ] && cp "$LIB_SRC/libpng16.so.16" "$ROOTFS_DIR/lib/"
[ -f "$LIB_SRC/libvorbisfile.so.3" ] && cp "$LIB_SRC/libvorbisfile.so.3" "$ROOTFS_DIR/lib/"
[ -f "$LIB_SRC/libvorbis.so.0" ] && cp "$LIB_SRC/libvorbis.so.0" "$ROOTFS_DIR/lib/"
[ -f "$LIB_SRC/libogg.so.0" ] && cp "$LIB_SRC/libogg.so.0" "$ROOTFS_DIR/lib/"
[ -f "$LIB_SRC/libstdc++.so.6" ] && cp "$LIB_SRC/libstdc++.so.6" "$ROOTFS_DIR/lib/"
[ -f "$LIB_SRC/libm.so.6" ] && cp "$LIB_SRC/libm.so.6" "$ROOTFS_DIR/lib/"
[ -f "$LIB_SRC/libgcc_s.so.1" ] && cp "$LIB_SRC/libgcc_s.so.1" "$ROOTFS_DIR/lib/"
[ -f "$LIB_SRC/libc.so.6" ] && cp "$LIB_SRC/libc.so.6" "$ROOTFS_DIR/lib/"
[ -f /lib64/ld-linux-x86-64.so.2 ] && cp /lib64/ld-linux-x86-64.so.2 "$ROOTFS_DIR/lib64/"

# Calculate required size (rootfs size + 50% headroom)
ROOTFS_SIZE=$(du -s "$ROOTFS_DIR" | cut -f1)
IMAGE_BLOCKS=$((ROOTFS_SIZE * 3 / 2))
# Minimum 64MB
[ $IMAGE_BLOCKS -lt 65536 ] && IMAGE_BLOCKS=65536

echo "Creating ext2 image ($IMAGE_BLOCKS blocks)..."
rm -f "$OUTPUT_FILE"
genext2fs -d "$ROOTFS_DIR" -b $IMAGE_BLOCKS -N 1024 "$OUTPUT_FILE"

# Optionally compress
if command -v gzip >/dev/null 2>&1; then
    echo "Creating compressed version..."
    gzip -k -9 -f "$OUTPUT_FILE"
fi

echo ""
echo "=== Build Complete ==="
ls -lh "$OUTPUT_FILE"*
echo ""
echo "Disk image contents:"
e2ls "$OUTPUT_FILE:/usr/bin" 2>/dev/null || echo "(install e2tools to verify contents)"
