#!/bin/bash
# Generate code coverage report using gcov/lcov

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${YELLOW}=== Code Coverage Report Generation ===${NC}"
echo ""

# Check if lcov is installed
if ! command -v lcov &> /dev/null; then
    echo -e "${RED}✗ lcov not found${NC}"
    echo ""
    echo "Install with:"
    echo "  Ubuntu/Debian: sudo apt-get install lcov"
    echo "  Arch Linux:    sudo pacman -S lcov"
    echo "  macOS:         brew install lcov"
    exit 1
fi

# Check if genhtml is installed (usually comes with lcov)
if ! command -v genhtml &> /dev/null; then
    echo -e "${RED}✗ genhtml not found (should be installed with lcov)${NC}"
    exit 1
fi

# Step 1: Configure with coverage enabled
echo -e "${BLUE}[1/5] Configuring coverage build...${NC}"
cmake --preset coverage

# Step 2: Build
echo -e "${BLUE}[2/5] Building...${NC}"
cmake --build build/coverage --parallel

# Step 3: Run tests
echo -e "${BLUE}[3/5] Running tests...${NC}"
ctest --test-dir build/coverage --output-on-failure

# Step 4: Capture coverage data
echo -e "${BLUE}[4/5] Capturing coverage data...${NC}"

# Create coverage directory
mkdir -p build/coverage/coverage-report

# Capture initial baseline (to include files with no coverage)
lcov --capture --initial \
     --directory build/coverage \
     --output-file build/coverage/coverage-base.info \
     --quiet

# Capture test coverage
lcov --capture \
     --directory build/coverage \
     --output-file build/coverage/coverage-test.info \
     --quiet

# Combine baseline and test coverage
lcov --add-tracefile build/coverage/coverage-base.info \
     --add-tracefile build/coverage/coverage-test.info \
     --output-file build/coverage/coverage-total.info \
     --quiet

# Remove third-party library coverage (but keep our tests and src)
lcov --remove build/coverage/coverage-total.info \
     '/usr/*' \
     '*/build/*/_deps/*' \
     --output-file build/coverage/coverage-filtered.info \
     --quiet

# Step 5: Generate HTML report
echo -e "${BLUE}[5/5] Generating HTML report...${NC}"
genhtml build/coverage/coverage-filtered.info \
        --output-directory build/coverage/coverage-report \
        --title "VibeGL Code Coverage" \
        --show-details \
        --legend \
        --quiet

echo ""
echo -e "${GREEN}✓ Coverage report generated successfully!${NC}"
echo ""
echo -e "${BLUE}Summary:${NC}"
lcov --list build/coverage/coverage-filtered.info

echo ""
echo -e "${GREEN}Open the report:${NC}"
echo -e "  ${BLUE}build/coverage/coverage-report/index.html${NC}"
echo ""
echo "You can open it with:"
echo "  xdg-open build/coverage/coverage-report/index.html   # Linux"
echo "  open build/coverage/coverage-report/index.html       # macOS"
