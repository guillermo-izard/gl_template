#!/bin/bash
# Run clang-tidy static analysis

# Find clang-tidy (try versioned and unversioned)
CLANG_TIDY=$(command -v clang-tidy-18 || command -v clang-tidy)

if [ -z "$CLANG_TIDY" ]; then
    echo "⚠ clang-tidy not found"
    echo "Install with: sudo apt-get install clang-tidy-18"
    exit 1
fi

# Ensure debug build exists (needed for compile_commands.json)
if [ ! -d "build/debug" ]; then
    echo "Configuring debug build for clang-tidy..."
    cmake --preset debug
fi

echo "Running static analysis (clang-tidy)..."
echo ""
find src -name "*.cpp" | xargs "$CLANG_TIDY" -p build/debug

echo ""
echo "✓ Static analysis complete"
