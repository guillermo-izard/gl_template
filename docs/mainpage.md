# VibeGL API Documentation {#mainpage}

**Modern OpenGL graphics programming template with C++23**

[🚀 Try Live Demo](https://guillermo-izard.github.io/gl_template/) | [📦 GitHub Repository](https://github.com/guillermo-izard/gl_template)

---

## Overview

VibeGL is a production-ready OpenGL template that supports **desktop** (OpenGL 4.6) and **web** (WebGL 2.0/OpenGL ES 3.0) platforms. It demonstrates modern C++23 practices, clean architecture, and comprehensive tooling for graphics application development.

### Key Features

- 🎯 **Modern C++23** with strict compiler warnings and static analysis
- 🌐 **Cross-platform**: Windows, Linux, macOS, and **WebAssembly**
- 🎨 **OpenGL 4.6** (desktop) / **WebGL 2.0** (web)
- 🖼️ **Dear ImGui** for interactive UI with docking support
- 📐 **GLM** for mathematics
- 📝 **spdlog** for structured logging
- 🧪 **Comprehensive testing** with doctest and sanitizers
- ⚙️ **CMake Presets** for streamlined builds
- 🔄 **CI/CD** with GitHub Actions
- 📊 **Code quality**: clang-format, clang-tidy, coverage, Valgrind

---

## Quick Start

### Building

```bash
# Configure and build
cmake --preset debug
cmake --build build/debug --parallel

# Run application (from project root)
./build/debug/bin/vibegl

# Run tests
ctest --test-dir build/debug --output-on-failure
```

### Web Build

```bash
# Activate Emscripten
source /path/to/emsdk/emsdk_env.sh

# Build for web
./scripts/build-web.sh --serve
```

---

## Architecture

### Platform Abstraction Layer

VibeGL uses **compile-time platform detection** to eliminate runtime overhead:

#### Core Platform Module

- @ref vibegl::kIsWeb - Compile-time web platform detection
- @ref vibegl::kIsDesktop - Compile-time desktop platform detection
- @ref vibegl::kGLSLVersionString - Platform-specific GLSL version string
- @ref vibegl::kShaderSuffix - Shader file suffix (`_gl46` or `_es3`)

#### Application Framework

The @ref vibegl::Application class provides a platform-agnostic base:

```cpp
class MyApp : public vibegl::Application {
protected:
    void onInit() override {
        // Initialize resources
    }

    void onTick(float deltaTime) override {
        // Update and render
    }

    void onShutdown() override {
        // Cleanup
    }
};
```

**Platform-specific main loops:**
- **Desktop**: Traditional `while(!shouldQuit())` loop
- **Web**: `emscripten_set_main_loop` for browser integration

---

## Core API

### Rendering Subsystem

#### Shader Management

The @ref vibegl::ShaderManager automatically loads platform-specific shaders:

```cpp
// Loads "cube_gl46.vert/frag" on desktop, "cube_es3.vert/frag" on web
auto program = ShaderManager::loadProgram("cube");
if (!program) {
    spdlog::error("Shader load failed: {}", program.error());
}
```

**Key Features:**
- Automatic platform suffix selection
- Compile error reporting with line numbers
- RAII-friendly Result<T> return type

See: @ref vibegl::ShaderManager

#### Texture Loading

The @ref vibegl::TextureLoader handles platform differences automatically:

```cpp
auto texture = TextureLoader::load("textures/crate.png");
if (!texture) {
    spdlog::error("Texture load failed: {}", texture.error());
}
```

**Features:**
- Automatic format detection (RGB/RGBA)
- Mipmap generation
- Platform-specific texture parameters

See: @ref vibegl::TextureLoader

---

## Error Handling

VibeGL uses the @ref vibegl::Result<T> type for expressive error handling:

```cpp
Result<GLuint> loadShader() {
    auto source = readFile("shader.vert");
    if (!source) {
        return Error("Failed to read shader file");
    }
    return compileShader(source.value());
}

// Usage
auto shader = loadShader();
if (shader) {
    glUseProgram(shader.value());
} else {
    spdlog::error("Shader error: {}", shader.error());
}
```

When `std::expected` is available (C++23), Result is an alias. Otherwise, a minimal compatible implementation is provided.

See: @ref vibegl::Result, @ref vibegl::Error

---

## Platform-Specific Code

Use `if constexpr` for compile-time platform branching:

```cpp
#include "core/Platform.hpp"

void setup() {
    if constexpr (vibegl::kIsWeb) {
        // Web-specific initialization
        emscripten_set_canvas_element_size("#canvas", 1920, 1080);
    } else {
        // Desktop-specific initialization
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}
```

**Benefits:**
- Zero runtime overhead
- Dead code elimination by compiler
- Type-safe platform detection

---

## Shader Development

Maintain two variants for each shader:

- **Desktop**: `name_gl46.{vert,frag}` - GLSL 4.6 Core (`#version 460 core`)
- **Web**: `name_es3.{vert,frag}` - GLSL ES 3.0 (`#version 300 es`)

**Example structure:**
```
data/shaders/
├── cube_gl46.vert    # Desktop vertex shader
├── cube_gl46.frag    # Desktop fragment shader
├── cube_es3.vert     # Web vertex shader
└── cube_es3.frag     # Web fragment shader
```

The @ref vibegl::ShaderManager automatically selects the correct variant based on @ref vibegl::kShaderSuffix.

**Validation:**
```bash
./scripts/validate-shaders.sh  # Validates all shaders with glslangValidator
```

---

## Class Hierarchy

### Core Classes

- @ref vibegl::Application - Base application class with platform-abstracted main loop
- @ref vibegl::WindowConfig - Window configuration structure

### Rendering Classes

- @ref vibegl::ShaderManager - Shader loading and compilation utilities
- @ref vibegl::TextureLoader - Texture loading with stb_image

### Utility Classes

- @ref vibegl::Result - Monadic error handling wrapper
- @ref vibegl::Error - Error message container

### Demo Application

- @ref vibegl::VibeGLApp - Example application demonstrating the API

---

## Development Tools

### Testing

```bash
# Run all tests
ctest --test-dir build/debug --output-on-failure

# With sanitizers
cmake --preset sanitizers
cmake --build build/sanitizers --parallel
ctest --test-dir build/sanitizers --output-on-failure
```

### Code Quality

```bash
# Format check/fix
./scripts/format-check.sh
./scripts/format-fix.sh

# Static analysis
./scripts/static-analysis.sh

# Code coverage
./scripts/generate-coverage.sh
# Open build/coverage/coverage-report/index.html

# Memory profiling
./scripts/profile-memory.sh
./scripts/profile-memory.sh --mode massif  # Heap profiling
```

### CI Validation

```bash
# Run all CI checks locally
./scripts/ci-local.sh
```

---

## Configuration Files

- `CMakeLists.txt` - Root CMake configuration
- `CMakePresets.json` - Build presets (debug, release, sanitizers, coverage, emscripten)
- `Doxyfile` - Documentation generation configuration
- `.clang-format` - Code formatting rules
- `.clang-tidy` - Static analysis configuration
- `.lsan_suppressions` - LeakSanitizer suppressions for known false positives

---

## Directory Structure

```
src/
├── core/                      # Platform abstractions
│   ├── Application.hpp/cpp    # Main loop, window, ImGui setup
│   ├── Platform.hpp           # Compile-time platform detection
│   ├── GLIncludes.hpp         # Platform-specific GL headers
│   └── Result.hpp             # Error handling utilities
├── rendering/                 # Graphics utilities
│   ├── ShaderManager.hpp/cpp  # Shader loading
│   └── TextureLoader.hpp/cpp  # Texture loading
├── VibeGLApp.hpp/cpp          # Demo application
└── main.cpp                   # Entry point

data/
├── shaders/                   # Platform-specific GLSL shaders
│   ├── *_gl46.{vert,frag}     # Desktop (OpenGL 4.6)
│   └── *_es3.{vert,frag}      # Web (OpenGL ES 3.0)
└── textures/                  # Image assets

tests/                         # Unit tests with doctest
```

---

## Dependencies

All dependencies are automatically fetched via CMake FetchContent:

| Library | Purpose | Version |
|---------|---------|---------|
| [GLFW](https://github.com/glfw/glfw) | Windowing and input | Latest |
| [GLAD2](https://github.com/Dav1dde/glad) | OpenGL loader (desktop) | GL 4.6 Core |
| [GLM](https://github.com/g-truc/glm) | Mathematics | Latest |
| [Dear ImGui](https://github.com/ocornut/imgui) | Immediate mode GUI | Docking branch |
| [spdlog](https://github.com/gabime/spdlog) | Logging | Platform-specific |
| [stb_image](https://github.com/nothings/stb) | Image loading | Latest |
| [doctest](https://github.com/doctest/doctest) | Testing | Latest |

---

## CMake Presets

| Preset | Description |
|--------|-------------|
| `debug` | Debug build with symbols, tests enabled |
| `release` | Optimized release build with LTO |
| `sanitizers` | Debug + AddressSanitizer + UndefinedBehaviorSanitizer |
| `coverage` | Debug + code coverage instrumentation |
| `emscripten` | WebAssembly build (C++20 due to Emscripten limitations) |

---

## Example Application

See @ref vibegl::VibeGLApp for a complete example demonstrating:

- Custom @ref vibegl::Application subclass
- Shader and texture loading
- ImGui integration
- Platform-specific rendering
- Input handling

---

## Resources

- [GitHub Repository](https://github.com/guillermo-izard/gl_template)
- [Live WebAssembly Demo](https://guillermo-izard.github.io/gl_template/)
- [Issue Tracker](https://github.com/guillermo-izard/gl_template/issues)
- [OpenGL 4.6 Reference](https://www.khronos.org/registry/OpenGL-Refpages/gl4/)
- [WebGL 2.0 Reference](https://www.khronos.org/registry/webgl/specs/latest/2.0/)

---

## License

MIT License - See LICENSE file for details.

---

*Generated with Doxygen*
