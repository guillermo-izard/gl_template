#!/bin/bash
# Build all configurations (debug, release, sanitizers)

set -e

echo "Building all configurations..."
echo ""

echo "[1/3] Debug build..."
cmake --preset debug
cmake --build build/debug --parallel
echo "✓ Debug build complete"
echo ""

echo "[2/3] Release build..."
cmake --preset release
cmake --build build/release --parallel
echo "✓ Release build complete"
echo ""

echo "[3/3] Sanitizers build..."
cmake --preset sanitizers
cmake --build build/sanitizers --parallel
echo "✓ Sanitizers build complete"
echo ""

echo "✓ All builds successful!"
