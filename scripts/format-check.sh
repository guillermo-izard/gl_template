#!/bin/bash
# Check code formatting (clang-format)

# Find clang-format (try versioned and unversioned)
CLANG_FORMAT=$(command -v clang-format-18 || command -v clang-format)

if [ -z "$CLANG_FORMAT" ]; then
    echo "⚠ clang-format not found"
    echo "Install with: sudo apt-get install clang-format-18"
    exit 0
fi

echo "Checking code formatting..."
find src tests -name "*.cpp" -o -name "*.h" | xargs "$CLANG_FORMAT" --dry-run --Werror

if [ $? -eq 0 ]; then
    echo "✓ All files are properly formatted"
else
    echo "✗ Formatting errors found"
    echo ""
    echo "To fix automatically, run:"
    echo "  ./scripts/format-fix.sh"
fi
