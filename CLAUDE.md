# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

VibeGL is a modern OpenGL graphics template supporting **desktop (OpenGL 4.6)** and **web (WebGL 2.0/OpenGL ES 3.0)** platforms. It uses C++23, CMake presets, comprehensive testing with sanitizers, and includes a functional demo with Dear ImGui.

## Build and Test Commands

### Quick Start
```bash
# Configure and build (debug)
cmake --preset debug
cmake --build build/debug --parallel

# Run application (must run from project root due to data/ dependency)
./build/debug/bin/vibegl

# Run tests
ctest --test-dir build/debug --output-on-failure
```

### CMake Presets
- `debug` - Debug build with symbols, tests enabled
- `release` - Optimized release build, tests disabled
- `sanitizers` - Debug + AddressSanitizer + UndefinedBehaviorSanitizer
- `coverage` - Debug + code coverage instrumentation
- `emscripten` - WebAssembly build (requires Emscripten SDK)

### Development Scripts
```bash
# Run all CI checks locally (formatting, build, tests, static analysis)
./scripts/ci-local.sh

# Auto-format code with clang-format
./scripts/format-fix.sh

# Individual operations
./scripts/build-all.sh          # Build all configurations
./scripts/test-all.sh           # Run all tests
./scripts/static-analysis.sh    # Run clang-tidy
./scripts/validate-shaders.sh   # Validate GLSL shaders
./scripts/generate-coverage.sh  # Generate HTML coverage report
./scripts/profile-memory.sh     # Run Valgrind profiling
```

### Web Build
```bash
# Activate Emscripten (adjust path as needed)
source /path/to/emsdk/emsdk_env.sh

# Build and optionally serve
./scripts/build-web.sh           # Build only
./scripts/build-web.sh --serve   # Build and serve on port 8080

# Output: build/emscripten/bin/vibegl.html
```

### Documentation
```bash
# Generate API docs (requires doxide and mkdocs-material)
doxide build          # Generate Markdown
mkdocs build         # Build HTML
mkdocs serve         # Serve locally at http://localhost:8000

# Or use CMake
cmake --build build/debug --target docs
```

## Architecture and Code Structure

### Platform Abstraction

The codebase is designed for **compile-time platform detection** with minimal runtime branching:

- **Platform.hpp**: Defines `kIsWeb`, `kIsDesktop`, `kGLSLVersionString`, `kShaderSuffix` as `constexpr` values
- **GLIncludes.hpp**: Handles OpenGL header differences (GLAD for desktop, Emscripten's ES3 for web)
- **Application.hpp/cpp**: Main loop abstraction
  - Desktop: Traditional `while(!shouldQuit())` loop
  - Web: `emscripten_set_main_loop` callback (required for browser integration)

### Core Classes

**Application (src/core/Application.hpp)**
- Base class for graphics applications
- Handles GLFW window, OpenGL context, ImGui initialization
- Derive and override `onInit()`, `onTick(float deltaTime)`, `onShutdown()`
- `run()` starts the platform-specific main loop

**ShaderManager (src/rendering/ShaderManager.hpp)**
- Automatic platform-specific shader loading
- `loadProgram("cube")` loads `cube_gl46.{vert,frag}` on desktop or `cube_es3.{vert,frag}` on web
- Shader files use suffixes: `_gl46` for desktop, `_es3` for web

**TextureLoader (src/rendering/TextureLoader.hpp)**
- Loads textures using stb_image
- Handles platform differences (RGBA vs RGB, mipmaps)

### Project Structure
```
src/
├── core/                    # Platform abstractions
│   ├── Application.hpp/cpp  # Main loop, window, ImGui setup
│   ├── Platform.hpp         # Compile-time platform detection
│   └── GLIncludes.hpp       # Platform-specific GL headers
├── rendering/               # Graphics utilities
│   ├── ShaderManager.hpp/cpp   # Shader loading with platform suffix
│   └── TextureLoader.hpp/cpp   # Texture loading via stb_image
├── VibeGLApp.hpp/cpp        # Demo application (spinning textured cube)
└── main.cpp                 # Entry point

data/
├── shaders/                 # Platform-specific GLSL shaders
│   ├── *_gl46.{vert,frag}  # Desktop (OpenGL 4.6)
│   └── *_es3.{vert,frag}   # Web (OpenGL ES 3.0)
└── textures/                # Image assets

cmake/
├── Dependencies.cmake       # FetchContent for all libraries
├── CompilerWarnings.cmake   # Strict warning flags
├── Sanitizers.cmake         # ASan/UBSan configuration
└── Coverage.cmake           # Code coverage setup

tests/                       # doctest unit tests
```

### Dependencies (Auto-fetched via CMake)

All dependencies use FetchContent:
- **GLFW** - Windowing (Emscripten provides built-in version)
- **GLAD** - OpenGL loader (desktop only; Emscripten has built-in ES3)
- **GLM** - Math library (configured for C++23)
- **Dear ImGui** - Immediate mode GUI (docking branch)
- **spdlog** - Logging (different versions for native vs Emscripten)
- **stb_image** - Texture loading
- **doctest** - Testing framework

## Important Development Notes

### Working Directory
The application expects `data/shaders/` and `data/textures/` to be accessible from the working directory. When running manually, execute from project root:
```bash
./build/debug/bin/vibegl
```
VS Code launch configurations handle this automatically.

### Sanitizers
- Tests automatically run with sanitizers when using the `sanitizers` preset
- Known false positive: LeakSanitizer may report 128-byte leak from `extensionSupportedGLX` (GLX driver initialization)
- Suppression file: `.lsan_suppressions` (automatically applied in VS Code)
- When running manually with sanitizers:
  ```bash
  LSAN_OPTIONS=suppressions=../.lsan_suppressions ./build/debug/bin/vibegl
  ```
- **Cannot run sanitizers under debuggers** (GDB/LLDB) due to ptrace limitations - use Ctrl+F5 (run without debugging)

### Shader Development
- Maintain two versions of each shader: `name_gl46.{vert,frag}` and `name_es3.{vert,frag}`
- Desktop uses GLSL 4.6 Core (`#version 460 core`)
- Web uses GLSL ES 3.0 (`#version 300 es`)
- Validate all shaders before committing: `./scripts/validate-shaders.sh`

### Cross-Platform Considerations
- Desktop uses C++23; web uses C++20 (Emscripten limitation - specified in emscripten preset)
- Use `if constexpr (vibegl::kIsWeb)` or `if constexpr (vibegl::kIsDesktop)` for platform-specific code
- ImGui backend initialized with appropriate GLSL version via `vibegl::kGLSLVersionString`

### Testing Strategy
- Unit tests in `tests/` use doctest framework
- Build with `--preset sanitizers` for memory/UB detection
- Use `--preset coverage` for code coverage analysis
- Run `./scripts/ci-local.sh` before pushing to verify all CI checks pass

### Documentation Standards
Use Doxygen-style comments for all public APIs:
```cpp
/// @brief Brief description
/// @param paramName Parameter description
/// @returns Description of return value
```
Configuration: `doxide.yaml` and `mkdocs.yml`

### Code Quality
- clang-format enforces style (`.clang-format` in root)
- clang-tidy performs static analysis with strict checks
- All warnings treated as errors in Debug builds
- LTO enabled for Release builds when supported
