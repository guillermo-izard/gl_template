#!/bin/bash
# Build VibeGL for WebAssembly using Emscripten
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check for Emscripten SDK
if [ -z "$EMSDK" ]; then
    echo -e "${RED}Error: EMSDK environment not set${NC}"
    echo "Run: source /path/to/emsdk/emsdk_env.sh"
    exit 1
fi

echo -e "${YELLOW}=== Building VibeGL for WebAssembly ===${NC}"

# Configure
echo -e "${YELLOW}Configuring...${NC}"
EMSDK_QUIET=1 cmake --preset emscripten

# Build
echo -e "${YELLOW}Building...${NC}"
cmake --build build/emscripten --parallel

echo -e "${GREEN}Build complete!${NC}"
echo ""
echo "Output files:"
echo "  build/emscripten/bin/vibegl.html"
echo "  build/emscripten/bin/vibegl.js"
echo "  build/emscripten/bin/vibegl.wasm"
echo "  build/emscripten/bin/vibegl.data"

# Optional: start local server
if [[ "$1" == "--serve" ]]; then
    PORT="${2:-8080}"
    echo ""
    echo -e "${GREEN}Starting server at http://localhost:$PORT/vibegl.html${NC}"
    echo "Press Ctrl+C to stop"
    cd build/emscripten/bin
    python3 -m http.server "$PORT"
fi
