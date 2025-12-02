#!/bin/bash
# Auto-fix code formatting (clang-format)

# Find clang-format (try versioned and unversioned)
CLANG_FORMAT=$(command -v clang-format-18 || command -v clang-format)

if [ -z "$CLANG_FORMAT" ]; then
    echo "⚠ clang-format not found"
    echo "Install with: sudo apt-get install clang-format-18"
    exit 1
fi

echo "Auto-formatting code..."
find src tests -name "*.cpp" -o -name "*.h" | xargs "$CLANG_FORMAT" -i

echo "✓ Code formatting applied"
echo ""
echo "Review changes with: git diff"
