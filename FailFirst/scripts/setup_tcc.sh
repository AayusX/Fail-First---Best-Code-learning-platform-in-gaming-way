#!/bin/bash
# Download TCC (Tiny C Compiler) for portable C/C++ support
# Run this script to set up portable compilers

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
PORTABLE_DIR="$PROJECT_DIR/portable"

echo "📥 Setting up portable compiler environment..."

mkdir -p "$PORTABLE_DIR/tcc"

# Check if we're on Windows (for using this in MSYS/Git Bash)
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" ]]; then
    echo "🪟 Windows detected"
    TCC_URL="http://download.savannah.gnu.org/releases/tinycc/tcc-0.9.27-win64-bin.zip"
    
    cd "$PORTABLE_DIR"
    if command -v curl &> /dev/null; then
        curl -L "$TCC_URL" -o tcc.zip
    elif command -v wget &> /dev/null; then
        wget "$TCC_URL" -O tcc.zip
    else
        echo "❌ Please install curl or wget"
        exit 1
    fi
    
    unzip -o tcc.zip -d tcc_temp
    mv tcc_temp/tcc/* tcc/
    rm -rf tcc_temp tcc.zip
    
    echo "✅ TCC installed to: $PORTABLE_DIR/tcc/"
    echo "   TCC executable: $PORTABLE_DIR/tcc/tcc.exe"
else
    echo "🐧 Linux detected"
    echo ""
    echo "For Linux development, TCC can be installed via:"
    echo "  sudo apt install tcc    # Debian/Ubuntu"
    echo "  sudo dnf install tcc    # Fedora"
    echo ""
    echo "For Windows builds, run this script on Windows or in MSYS2."
    echo ""
    
    # Check if TCC is already available
    if command -v tcc &> /dev/null; then
        echo "✅ System TCC found: $(which tcc)"
    else
        echo "ℹ️  System TCC not installed, but g++ will be used as fallback."
    fi
fi

echo ""
echo "Done! The FailFirst app will automatically detect and use TCC if available."
