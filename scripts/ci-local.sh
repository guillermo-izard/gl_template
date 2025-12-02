#!/bin/bash
# Local CI validation script
# Runs the same checks as GitHub Actions CI

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Running Local CI Checks ===${NC}\n"

# Track overall status
FAILED=0

# Function to print status
print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ $2${NC}"
    else
        echo -e "${RED}✗ $2${NC}"
        FAILED=1
    fi
}

# Check 1: Code Formatting
echo -e "${YELLOW}[1/4] Checking code formatting...${NC}"
CLANG_FORMAT=$(command -v clang-format-18 || command -v clang-format)
if [ -n "$CLANG_FORMAT" ]; then
    find src tests -name "*.cpp" -o -name "*.h" | xargs "$CLANG_FORMAT" --dry-run --Werror 2>&1
    print_status $? "Code formatting check"
else
    echo -e "${YELLOW}⚠ clang-format not found, skipping formatting check${NC}"
fi

echo ""

# Check 2: Build with sanitizers
echo -e "${YELLOW}[2/4] Building with sanitizers...${NC}"
if [ ! -d "build/sanitizers" ]; then
    cmake --preset sanitizers > /dev/null
fi
cmake --build build/sanitizers --parallel
print_status $? "Build with sanitizers"

echo ""

# Check 3: Run tests
echo -e "${YELLOW}[3/4] Running tests...${NC}"
ctest --test-dir build/sanitizers --output-on-failure
print_status $? "Tests"

echo ""

# Check 4: Static analysis (clang-tidy)
echo -e "${YELLOW}[4/4] Running static analysis (clang-tidy)...${NC}"
CLANG_TIDY=$(command -v clang-tidy-18 || command -v clang-tidy)
if [ -n "$CLANG_TIDY" ]; then
    if [ ! -d "build/debug" ]; then
        echo "Configuring debug build for clang-tidy..."
        cmake --preset debug > /dev/null
    fi
    find src -name "*.cpp" | xargs "$CLANG_TIDY" -p build/debug
    print_status $? "Static analysis"
else
    echo -e "${YELLOW}⚠ clang-tidy not found, skipping static analysis${NC}"
fi

echo ""

# Summary
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}=== All checks passed! ✓ ===${NC}"
    exit 0
else
    echo -e "${RED}=== Some checks failed! ✗ ===${NC}"
    exit 1
fi
