#!/bin/bash
# Validate GLSL shaders using glslangValidator

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}=== Shader Validation ===${NC}"
echo ""

# Check if glslangValidator is installed
if ! command -v glslangValidator &> /dev/null; then
    echo -e "${RED}✗ glslangValidator not found${NC}"
    echo ""
    echo "Install with:"
    echo "  Ubuntu/Debian: sudo apt-get install glslang-tools"
    echo "  Arch Linux:    sudo pacman -S glslang"
    echo "  macOS:         brew install glslang"
    exit 1
fi

# Find all shader files
SHADER_FILES=$(find data/shaders -type f \( -name "*.vert" -o -name "*.frag" -o -name "*.geom" -o -name "*.comp" -o -name "*.tesc" -o -name "*.tese" -o -name "*.glsl" \) 2>/dev/null || true)

if [ -z "$SHADER_FILES" ]; then
    echo -e "${YELLOW}⚠ No shader files found in data/shaders/${NC}"
    exit 0
fi

TOTAL=0
PASSED=0
FAILED=0

# Validate each shader
for shader in $SHADER_FILES; do
    TOTAL=$((TOTAL + 1))
    echo -n "Validating $shader... "

    # Run glslangValidator
    # -V for Vulkan SPIR-V (optional, comment out if OpenGL-only)
    # --target-env opengl for OpenGL target
    if glslangValidator --target-env opengl "$shader" > /dev/null 2>&1; then
        echo -e "${GREEN}✓${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗${NC}"
        FAILED=$((FAILED + 1))
        # Show detailed error
        echo -e "${RED}Error details:${NC}"
        glslangValidator --target-env opengl "$shader" 2>&1 | sed 's/^/  /'
        echo ""
    fi
done

echo ""
echo -e "Results: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC} (Total: $TOTAL)"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All shaders valid${NC}"
    exit 0
else
    echo -e "${RED}✗ Some shaders failed validation${NC}"
    exit 1
fi
