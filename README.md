# VibeGL

A modern OpenGL graphics programming template with C++23, featuring a clean CMake build system, comprehensive testing, and cross-platform support.

## Features

- **Modern C++23** with strict compiler warnings and static analysis
- **Cross-platform** support for Windows and Linux
- **OpenGL 4.6 Core Profile** with GLFW and GLAD2
- **Dear ImGui** with docking support for interactive UI
- **GLM** for mathematics operations
- **spdlog** for fast, structured logging
- **stb_image** for texture loading
- **Comprehensive testing** with doctest and sanitizers
- **CMake Presets** for easy build configuration
- **GitHub Actions CI/CD** with multiple compiler support
- **Code quality tools**: clang-format, clang-tidy

## Demo Application

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
# Ubuntu/Debian
sudo apt-get install ninja-build libxrandr-dev libxinerama-dev \
                     libxcursor-dev libxi-dev libgl1-mesa-dev

# Arch Linux
sudo pacman -S ninja libxrandr libxinerama libxcursor libxi mesa
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

## Development

### VS Code

The project includes VS Code configurations for building and debugging:

- **Tasks**: `Ctrl+Shift+B` to build (default: debug configuration)
- **Debug**: `F5` to build and launch with debugger
  - Automatically sets working directory to `data/`
  - Includes configurations for both main app and tests

### Code Formatting

```bash
# Format all source files
find src tests -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Check formatting
find src tests -name "*.cpp" -o -name "*.h" | xargs clang-format --dry-run --Werror
```

### Static Analysis

```bash
# Run clang-tidy
find src -name "*.cpp" | xargs clang-tidy -p build/debug
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
│   ├── main.cpp
│   └── CMakeLists.txt
├── tests/               # Unit tests
│   ├── test_main.cpp
│   └── CMakeLists.txt
├── data/                # Runtime assets
│   ├── shaders/        # GLSL shaders
│   └── textures/       # Texture files
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

- Builds on Linux (GCC, Clang) and Windows (MSVC)
- Runs all tests with sanitizers enabled
- Checks code formatting with clang-format
- Performs static analysis with clang-tidy

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
