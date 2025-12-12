# VibeGL

A modern OpenGL graphics programming template with C++23, featuring a clean CMake build system, comprehensive testing, and cross-platform support.

## Features

- **Modern C++23** with strict compiler warnings and static analysis
- **Cross-platform** support for Windows, Linux, and **Web (WebAssembly)**
- **OpenGL 4.6 Core Profile** (desktop) / **WebGL 2.0** (web) with GLFW and GLAD2
- **Dear ImGui** with docking support for interactive UI
- **GLM** for mathematics operations
- **spdlog** for fast, structured logging
- **stb_image** for texture loading
- **Comprehensive testing** with doctest and sanitizers
- **CMake Presets** for easy build configuration (debug, release, sanitizers, coverage)
- **GitHub Actions CI/CD** with multiple compiler support
- **Code quality tools**: clang-format, clang-tidy, shader validation
- **Development tools**: code coverage, memory profiling (Valgrind)
- **API Documentation** with Doxide and MkDocs Material, automatically generated and deployed

## Demo & Documentation

**[Try the live WebAssembly demo!](https://kunst.github.io/vibegl/)** | **[Browse API Documentation](https://kunst.github.io/vibegl/docs/)**

The template includes a fully functional demo that renders a spinning textured cube with interactive controls:

- Textured cube with customizable rotation
- ImGui control panel with:
  - FPS counter
  - Rotation axis control (X, Y, Z)
  - Rotation velocity control
  - Color picker for cube tinting
- ESC key to exit

## Requirements

### All Platforms

- CMake 3.28 or newer
- Ninja build system (recommended)
- A C++23 compatible compiler:
  - GCC 13+ (Linux)
  - Clang 18+ (Linux/macOS)
  - MSVC 2022+ (Windows)

### Linux

```bash
# Ubuntu/Debian - Base requirements
sudo apt-get install ninja-build libxrandr-dev libxinerama-dev \
                     libxcursor-dev libxi-dev libgl1-mesa-dev

# Arch Linux - Base requirements
sudo pacman -S ninja libxrandr libxinerama libxcursor libxi mesa
```

**Optional development tools:**
```bash
# Ubuntu/Debian
sudo apt-get install clang-format-18 clang-tidy-18 glslang-tools lcov valgrind

# Arch Linux
sudo pacman -S clang glslang lcov valgrind
```

### Windows

- Visual Studio 2022 with C++ Desktop Development workload
- Ninja: `choco install ninja` (or install manually)

## Building

### Using CMake Presets (Recommended)

```bash
# Configure for debug build
cmake --preset debug

# Build
cmake --build build/debug --parallel

# Run the application (from project root)
./build/debug/bin/vibegl

# Run tests
ctest --test-dir build/debug --output-on-failure
```

### Available Presets

- **debug**: Debug build with symbols and no optimizations
- **release**: Release build with optimizations
- **sanitizers**: Debug build with AddressSanitizer and UndefinedBehaviorSanitizer
- **coverage**: Debug build with code coverage instrumentation
- **emscripten**: WebAssembly build for browsers

### Web Build (Emscripten)

Build for web browsers using WebAssembly:

**Prerequisites:**
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)

**Building:**
```bash
# Activate Emscripten environment
source /path/to/emsdk/emsdk_env.sh

# Build (with optional local server)
./scripts/build-web.sh           # Build only
./scripts/build-web.sh --serve   # Build and serve on port 8080
./scripts/build-web.sh --serve 3000  # Build and serve on custom port
```

**Output files:**
```
build/emscripten/bin/
├── vibegl.html   # Main HTML page
├── vibegl.js     # JavaScript glue code
├── vibegl.wasm   # WebAssembly binary
└── vibegl.data   # Preloaded assets (shaders, textures)
```

**Manual serving:**
```bash
cd build/emscripten/bin
python3 -m http.server 8080
# Open http://localhost:8080/vibegl.html
```

**Automatic deployment:**
Every push to `master` automatically builds and deploys to GitHub Pages at:
https://kunst.github.io/vibegl/

### Manual Configuration

```bash
# Configure
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build --parallel

# Run tests
ctest --test-dir build --output-on-failure
```

## Running

The application expects to be run from a directory where it can access `data/shaders/` and `data/textures/`. The build system places executables in `build/<preset>/bin/`.

```bash
# From project root
cd data
../build/debug/bin/vibegl
```

Or use the VS Code launch configurations which automatically set the correct working directory.

## Generating Documentation

VibeGL uses [Doxide](https://doxide.org/) to generate API documentation with modern, responsive output via MkDocs Material.

### Online Documentation

**[Browse the latest API documentation](https://kunst.github.io/vibegl/docs/)** - automatically generated and deployed on every push to master.

### Local Documentation Generation

**Prerequisites:**
```bash
# Install Doxide (Ubuntu 24.04)
echo 'deb http://download.indii.org/deb noble main' | sudo tee /etc/apt/sources.list.d/indii.org.list
curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
sudo apt update
sudo apt install doxide

# Install MkDocs Material
pip install mkdocs mkdocs-material
```

**Generate docs:**
```bash
# Generate Markdown with Doxide
doxide build

# Build HTML with MkDocs
mkdocs build

# Serve locally
mkdocs serve
# Open http://localhost:8000
```

**Or use CMake:**
```bash
cmake --build build/debug --target docs
```

The documentation is generated from specially formatted comments in the source code (Doxygen-style):
```cpp
/// @brief Brief description of the function
/// @param paramName Description of the parameter
/// @returns Description of return value
```

Documentation configuration is in `doxide.yaml` and `mkdocs.yml`.

## Development

### Local CI Validation

**Run all CI checks before pushing:**

```bash
./scripts/ci-local.sh
```

This validates formatting, builds, tests, and static analysis - the same checks that run on GitHub Actions.

**Individual scripts:**

```bash
./scripts/format-check.sh         # Check code formatting
./scripts/format-fix.sh           # Auto-format code
./scripts/build-all.sh            # Build all configurations
./scripts/test-all.sh             # Run all tests
./scripts/static-analysis.sh      # Run clang-tidy
./scripts/validate-shaders.sh     # Validate GLSL shaders
./scripts/generate-coverage.sh    # Generate code coverage report
./scripts/profile-memory.sh       # Run Valgrind memory profiling
```

See [scripts/README.md](scripts/README.md) for detailed documentation.

### Quality Assurance Tools

**Shader Validation:**
```bash
./scripts/validate-shaders.sh
```
Validates all GLSL shaders before runtime using glslangValidator.

**Code Coverage:**
```bash
./scripts/generate-coverage.sh
```
Generates HTML coverage report showing which code is tested. Open `build/coverage/coverage-report/index.html` to view results.

**Memory Profiling:**
```bash
./scripts/profile-memory.sh              # Memory error detection
./scripts/profile-memory.sh --mode massif  # Heap profiling
./scripts/profile-memory.sh --mode both    # Both tools
```
Uses Valgrind to detect memory leaks and profile heap usage.

### VS Code

The project includes VS Code configurations for building and debugging:

- **Tasks**: `Ctrl+Shift+B` to build (default: debug configuration)
- **Debug**: `F5` to build and launch with debugger
  - Automatically sets working directory to `data/`
  - Includes configurations for both main app and tests
```

### Sanitizers

The project is configured to automatically enable AddressSanitizer and UndefinedBehaviorSanitizer when running tests:

```bash
# Build and run tests with sanitizers
cmake --preset sanitizers
cmake --build build/sanitizers --parallel
ctest --test-dir build/sanitizers --output-on-failure
```

You can also manually enable sanitizers for the main application:

```bash
cmake --preset debug -DENABLE_SANITIZERS=ON
```

**Known False Positive:** LeakSanitizer may report a 128-byte leak from `extensionSupportedGLX` in the GLFW/Mesa/GLX initialization code. This is a false positive - the GLX driver allocates static data during initialization that persists for the program's lifetime. A suppression file (`.lsan_suppressions`) is included to suppress this known issue. When running from the command line, use:

```bash
cd data
LSAN_OPTIONS=suppressions=../.lsan_suppressions ../build/debug/bin/vibegl
```

The VS Code tasks and CMake extension are already configured to use the suppression file automatically.

**Testing Sanitizers:**

The test suite includes validation that sanitizers are enabled and working:
- Compile-time feature detection tests
- Memory-intensive tests that would trigger sanitizers if bugs existed

Run tests with sanitizers:
```bash
./scripts/test-all.sh
```

Manually verify sanitizers are working:
```bash
./scripts/test-sanitizers-manual.sh
```

This script intentionally triggers sanitizer errors to confirm they're catching bugs.

**Important Note:** Sanitizers cannot run under debuggers (GDB/LLDB) due to ptrace limitations. If you see:
```
LeakSanitizer has encountered a fatal error.
HINT: LeakSanitizer does not work under ptrace (strace, gdb, etc)
```

This is expected behavior. To use sanitizers:
- Run without debugging (Ctrl+F5 in VS Code)
- Or run tests (sanitizers auto-enabled)
- Or run directly from terminal with sanitizers build

## Project Structure

```
vibegl/
├── .github/workflows/    # CI/CD configuration
├── .vscode/             # VS Code configuration
├── cmake/               # CMake modules
│   ├── Dependencies.cmake    # FetchContent library setup
│   ├── CompilerWarnings.cmake # Compiler-specific warnings
│   └── Sanitizers.cmake      # Sanitizer configuration
├── src/                 # Application source code
│   ├── core/           # Platform abstractions
│   │   ├── Application.hpp/cpp  # Main loop abstraction
│   │   ├── GLIncludes.hpp       # Platform-specific GL headers
│   │   └── Platform.hpp         # Compile-time platform detection
│   ├── rendering/      # Graphics utilities
│   │   ├── ShaderManager.hpp/cpp   # Shader loading
│   │   └── TextureLoader.hpp/cpp   # Texture loading
│   ├── VibeGLApp.hpp/cpp  # Demo application
│   ├── main.cpp           # Entry point
│   └── CMakeLists.txt
├── tests/               # Unit tests
│   ├── test_main.cpp
│   └── CMakeLists.txt
├── data/                # Runtime assets
│   ├── shaders/        # GLSL shaders (*_gl46 for desktop, *_es3 for web)
│   └── textures/       # Texture files
├── scripts/             # Build and development scripts
│   └── build-web.sh    # Emscripten build script
├── CMakeLists.txt      # Root CMake configuration
└── CMakePresets.json   # Build presets
```

## Dependencies

All dependencies are automatically fetched and built by CMake:

- [GLFW](https://github.com/glfw/glfw) - Windowing and input
- [GLAD2](https://github.com/Dav1dde/glad) - OpenGL loader
- [GLM](https://github.com/g-truc/glm) - Mathematics library
- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate mode GUI (docking branch)
- [spdlog](https://github.com/gabime/spdlog) - Fast logging
- [stb_image](https://github.com/nothings/stb) - Image loading
- [doctest](https://github.com/doctest/doctest) - Testing framework

## CI/CD

GitHub Actions automatically:

- Builds on Linux (GCC, Clang), Windows (MSVC), and **Web (Emscripten)**
- Runs all tests with sanitizers enabled
- Checks code formatting with clang-format
- Performs static analysis with clang-tidy
- Validates GLSL shaders

## Using as a Template

This project is designed to be used as a template for new OpenGL applications:

1. Click "Use this template" on GitHub (or clone the repository)
2. Update `CMakeLists.txt` with your project name
3. Replace the demo code in `src/main.cpp` with your application
4. Add your assets to `data/`
5. Write tests in `tests/`

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Contributing

This is a template project, but improvements and bug fixes are welcome! Please ensure:

- Code is formatted with clang-format
- All tests pass
- Sanitizers report no issues
- CI builds successfully

## Acknowledgments

- Dear ImGui for the excellent immediate mode GUI library
- The GLFW, GLAD, and GLM teams for their essential OpenGL tools
- The spdlog and doctest projects for development quality of life
