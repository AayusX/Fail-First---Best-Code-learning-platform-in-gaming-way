#!/bin/bash
# Cross-compile FailFirst for Windows using MXE
# Prerequisites: Install MXE with Qt5 static

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build-windows"

# MXE path - adjust if installed elsewhere
MXE_PATH="${MXE_PATH:-/usr/lib/mxe}"

if [ ! -d "$MXE_PATH" ]; then
    echo "❌ MXE not found at $MXE_PATH"
    echo "Install with: sudo apt install mxe-x86-64-w64-mingw32.static-qtbase"
    echo "Or set MXE_PATH environment variable"
    exit 1
fi

export PATH="$MXE_PATH/usr/bin:$PATH"

echo "🔨 Building FailFirst for Windows..."
echo "Project: $PROJECT_DIR"
echo "Output: $BUILD_DIR"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with MXE toolchain
x86_64-w64-mingw32.static-cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DSTATIC_BUILD=ON \
    "$PROJECT_DIR"

# Build
make -j$(nproc)

if [ -f "FailFirst.exe" ]; then
    echo ""
    echo "✅ Build successful!"
    echo "Output: $BUILD_DIR/FailFirst.exe"
    echo ""
    echo "File size: $(du -h FailFirst.exe | cut -f1)"
else
    echo "❌ Build failed"
    exit 1
fi
