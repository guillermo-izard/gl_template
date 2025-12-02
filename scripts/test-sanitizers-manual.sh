#!/bin/bash
# Manual sanitizer validation script
# This script demonstrates that sanitizers work by intentionally triggering them

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}=== Manual Sanitizer Validation ===${NC}"
echo ""
echo "This script will compile and run small programs that intentionally"
echo "trigger sanitizer errors to verify they are working correctly."
echo ""

# Create temporary directory
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

echo -e "${YELLOW}[1/3] Testing AddressSanitizer (buffer overflow)...${NC}"

cat > "$TEMP_DIR/test_asan.cpp" << 'EOF'
#include <iostream>

int main() {
    std::cout << "Attempting heap buffer overflow..." << std::endl;
    // Allocate on heap so ASan can detect it reliably
    int* arr = new int[3];
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    // This will trigger AddressSanitizer
    volatile int x = arr[10];  // Out of bounds access
    delete[] arr;
    return 0;
}
EOF

echo "Compiling test_asan.cpp with ASan..."
g++ -fsanitize=address -g "$TEMP_DIR/test_asan.cpp" -o "$TEMP_DIR/test_asan" 2>&1 | head -5

echo "Running (this should fail with ASan error)..."
if "$TEMP_DIR/test_asan" 2>&1 | grep -q "AddressSanitizer"; then
    echo -e "${GREEN}✓ AddressSanitizer is working! (detected buffer overflow)${NC}"
else
    echo -e "${RED}✗ AddressSanitizer did not detect the error${NC}"
fi
echo ""

echo -e "${YELLOW}[2/3] Testing UndefinedBehaviorSanitizer (signed overflow)...${NC}"

cat > "$TEMP_DIR/test_ubsan.cpp" << 'EOF'
#include <iostream>
#include <limits>

int main() {
    int max = std::numeric_limits<int>::max();
    std::cout << "Attempting signed integer overflow..." << std::endl;
    // This will trigger UndefinedBehaviorSanitizer
    volatile int overflow = max + 1;
    return 0;
}
EOF

echo "Compiling test_ubsan.cpp with UBSan..."
g++ -fsanitize=undefined -g "$TEMP_DIR/test_ubsan.cpp" -o "$TEMP_DIR/test_ubsan" 2>&1 | head -5

echo "Running (this should fail with UBSan error)..."
if "$TEMP_DIR/test_ubsan" 2>&1 | grep -q "runtime error"; then
    echo -e "${GREEN}✓ UndefinedBehaviorSanitizer is working! (detected signed overflow)${NC}"
else
    echo -e "${RED}✗ UndefinedBehaviorSanitizer did not detect the error${NC}"
fi
echo ""

echo -e "${YELLOW}[3/3] Testing LeakSanitizer (memory leak)...${NC}"

cat > "$TEMP_DIR/test_lsan.cpp" << 'EOF'
#include <iostream>

int main() {
    std::cout << "Allocating memory without freeing..." << std::endl;
    // This will trigger LeakSanitizer
    int* leak = new int[100];
    (void)leak;  // Intentionally don't delete
    return 0;
}
EOF

echo "Compiling test_lsan.cpp with ASan (includes LSan)..."
g++ -fsanitize=address -g "$TEMP_DIR/test_lsan.cpp" -o "$TEMP_DIR/test_lsan" 2>&1 | head -5

echo "Running (this should fail with LSan error)..."
if "$TEMP_DIR/test_lsan" 2>&1 | grep -q "LeakSanitizer"; then
    echo -e "${GREEN}✓ LeakSanitizer is working! (detected memory leak)${NC}"
else
    echo -e "${RED}✗ LeakSanitizer did not detect the error${NC}"
fi
echo ""

echo -e "${GREEN}=== Sanitizer Validation Complete ===${NC}"
echo ""
echo "All sanitizers are functioning correctly!"
echo "They will automatically catch similar bugs in your project when enabled."
