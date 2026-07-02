#!/bin/bash
# Quick rebuild script for FailFirst
# Usage: ./scripts/quick_rebuild.sh

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

echo "🔨 Building FailFirst..."

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure if needed
if [ ! -f "Makefile" ]; then
    echo "📋 Running CMake..."
    cmake -DCMAKE_BUILD_TYPE=Debug "$PROJECT_DIR"
fi

# Build
make -j$(nproc)

echo ""
echo "✅ Build complete!"
echo "Run with: $BUILD_DIR/FailFirst"
