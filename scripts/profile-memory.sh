#!/bin/bash
# Memory profiling with Valgrind (Memcheck and Massif)

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Default values
MODE="memcheck"
EXECUTABLE=""
ARGS=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --mode)
            MODE="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [OPTIONS] [-- EXECUTABLE_ARGS]"
            echo ""
            echo "Options:"
            echo "  --mode MODE    Profiling mode: memcheck, massif, or both (default: memcheck)"
            echo "  --help         Show this help message"
            echo ""
            echo "Modes:"
            echo "  memcheck  - Memory error detection (leaks, invalid access, etc.)"
            echo "  massif    - Heap profiling (memory usage over time)"
            echo "  both      - Run both memcheck and massif"
            echo ""
            echo "Examples:"
            echo "  $0                           # Run memcheck on tests"
            echo "  $0 --mode massif             # Run massif on tests"
            echo "  $0 --mode both               # Run both tools on tests"
            exit 0
            ;;
        --)
            shift
            ARGS="$@"
            break
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo -e "${YELLOW}=== Memory Profiling with Valgrind ===${NC}"
echo ""

# Check if valgrind is installed
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}✗ valgrind not found${NC}"
    echo ""
    echo "Install with:"
    echo "  Ubuntu/Debian: sudo apt-get install valgrind"
    echo "  Arch Linux:    sudo pacman -S valgrind"
    echo "  macOS:         brew install valgrind"
    exit 1
fi

# Ensure debug build exists
if [ ! -d "build/debug" ]; then
    echo -e "${BLUE}Configuring debug build...${NC}"
    cmake --preset debug
    cmake --build build/debug --parallel
fi

# Default to test executable
if [ -z "$EXECUTABLE" ]; then
    EXECUTABLE="./build/debug/bin/vibegl_tests"
fi

# Check if executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo -e "${RED}✗ Executable not found: $EXECUTABLE${NC}"
    exit 1
fi

# Create profiling output directory
mkdir -p build/debug/profiling

# Run memcheck
run_memcheck()
{
    echo -e "${BLUE}Running Memcheck (memory error detection)...${NC}"
    echo ""

    valgrind \
        --tool=memcheck \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --verbose \
        --log-file=build/debug/profiling/memcheck.log \
        $EXECUTABLE $ARGS

    echo ""
    echo -e "${GREEN}✓ Memcheck complete${NC}"
    echo -e "Report saved to: ${BLUE}build/debug/profiling/memcheck.log${NC}"
    echo ""
    echo "Summary:"
    grep -E "ERROR SUMMARY|LEAK SUMMARY" build/debug/profiling/memcheck.log || true
}

# Run massif
run_massif()
{
    echo -e "${BLUE}Running Massif (heap profiling)...${NC}"
    echo ""

    valgrind \
        --tool=massif \
        --massif-out-file=build/debug/profiling/massif.out \
        $EXECUTABLE $ARGS

    echo ""
    echo -e "${GREEN}✓ Massif complete${NC}"
    echo -e "Report saved to: ${BLUE}build/debug/profiling/massif.out${NC}"
    echo ""

    # Generate text report if ms_print is available
    if command -v ms_print &> /dev/null; then
        ms_print build/debug/profiling/massif.out > build/debug/profiling/massif.txt
        echo -e "Text report: ${BLUE}build/debug/profiling/massif.txt${NC}"
        echo ""
        echo "Peak memory usage:"
        grep -A 5 "peak" build/debug/profiling/massif.txt | head -10
    else
        echo "Install ms_print (part of valgrind) to generate text reports"
    fi
}

# Run profiling based on mode
case $MODE in
    memcheck)
        run_memcheck
        ;;
    massif)
        run_massif
        ;;
    both)
        run_memcheck
        echo ""
        run_massif
        ;;
    *)
        echo -e "${RED}✗ Unknown mode: $MODE${NC}"
        echo "Valid modes: memcheck, massif, both"
        exit 1
        ;;
esac

echo ""
echo -e "${GREEN}=== Profiling Complete ===${NC}"
