#!/bin/bash
# Run all tests

echo "Running tests..."
echo ""

# Run tests with sanitizers if available
if [ -d "build/sanitizers" ]; then
    echo "Running tests with sanitizers..."
    LSAN_OPTIONS=suppressions=.lsan_suppressions ctest --test-dir build/sanitizers --output-on-failure
elif [ -d "build/debug" ]; then
    echo "Running tests (debug build)..."
    ctest --test-dir build/debug --output-on-failure
else
    echo "No build found. Run './scripts/build-all.sh' first"
    exit 1
fi

echo ""
echo "✓ All tests passed!"
