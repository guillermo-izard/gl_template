# Emscripten WebAssembly Support - Implementation Plan

This document outlines the phased approach to add WebAssembly/WebGL support to VibeGL using Emscripten.

## Goals

- **Cross-platform code**: Single codebase for desktop and web
- **Minimal preprocessor directives**: Isolated to 3-4 infrastructure files
- **Clean abstractions**: Application code never sees platform differences
- **Full feature parity**: ImGui, textures, shaders all working on web
- **Maintainability**: Easy to work with day-to-day

## Architecture Overview

```
src/
├── core/
│   ├── Platform.hpp        # Compile-time platform detection (constexpr)
│   ├── Application.hpp     # Main loop abstraction
│   └── Application.cpp     # Platform-specific run() implementation
├── rendering/
│   ├── ShaderManager.hpp   # Shader loading with platform variants
│   ├── ShaderManager.cpp
│   ├── TextureLoader.hpp   # Texture loading (stb_image wrapper)
│   └── TextureLoader.cpp
├── VibeGLApp.hpp           # Concrete application
├── VibeGLApp.cpp
└── main.cpp                # Entry point only (~20 lines)
```

**#ifdef locations (total: 4 files):**
1. `Platform.hpp` - Define `kIsWeb` constexpr bool
2. `Application.cpp` - Main loop mechanism
3. `ShaderManager.cpp` - Shader path selection
4. `main.cpp` - OpenGL loader initialization (GLAD)

---

## Phase 0: Foundation & Preparation

**Goal**: Prepare the codebase without any Emscripten changes yet.

### 0.1 Enable LTO for Release Builds

Add to `CMakeLists.txt`:
```cmake
# Enable Link-Time Optimization for Release builds
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
if(lto_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    message(STATUS "LTO enabled for Release builds")
else()
    message(STATUS "LTO not supported: ${lto_error}")
endif()
```

### 0.2 Create Feature Branch

```bash
git checkout -b feature/emscripten-support
```

### 0.3 Reorganize Source Structure

Create the directory structure:
```
src/
├── core/           # Platform abstractions
├── rendering/      # Graphics utilities
└── CMakeLists.txt  # Updated for new structure
```

### 0.4 Create Platform Detection Header

**`src/core/Platform.hpp`**:
```cpp
#pragma once

namespace vibegl {

#ifdef __EMSCRIPTEN__
    inline constexpr bool kIsWeb = true;
    inline constexpr bool kIsDesktop = false;
#else
    inline constexpr bool kIsWeb = false;
    inline constexpr bool kIsDesktop = true;
#endif

// OpenGL version info for shader selection
#ifdef __EMSCRIPTEN__
    inline constexpr const char* kGLSLVersion = "#version 300 es";
    inline constexpr const char* kShaderSuffix = "_es3";
#else
    inline constexpr const char* kGLSLVersion = "#version 460 core";
    inline constexpr const char* kShaderSuffix = "_gl46";
#endif

} // namespace vibegl
```

### 0.5 Validation

- [ ] Native debug build compiles and runs
- [ ] Native release build compiles with LTO enabled
- [ ] All existing tests pass
- [ ] CI pipeline passes

---

## Phase 1: Core Abstractions

**Goal**: Refactor to use Application pattern, still native-only.

### 1.1 Create Application Base Class

**`src/core/Application.hpp`**:
```cpp
#pragma once

#include <string>

struct GLFWwindow;

namespace vibegl {

class Application {
public:
    struct Config {
        std::string title = "VibeGL";
        int width = 1280;
        int height = 720;
    };

    explicit Application(const Config& config);
    virtual ~Application();

    // Non-copyable, non-movable
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Starts the main loop (platform-specific implementation)
    void run();

protected:
    // Override these in derived class
    virtual void onInit() {}
    virtual void onTick(float deltaTime) = 0;
    virtual void onShutdown() {}
    virtual bool shouldQuit() const;

    // Accessors
    GLFWwindow* getWindow() const { return window_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    void tick();  // Called each frame by run()

    GLFWwindow* window_ = nullptr;
    int width_;
    int height_;
    float lastFrameTime_ = 0.0f;
};

} // namespace vibegl
```

**`src/core/Application.cpp`**:
```cpp
#include "Application.hpp"
#include "Platform.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace vibegl {

// ... constructor, destructor, initialization ...

void Application::run() {
    onInit();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(
        [](void* arg) { static_cast<Application*>(arg)->tick(); },
        this, 0, 1
    );
#else
    while (!shouldQuit()) {
        tick();
    }
    onShutdown();
#endif
}

void Application::tick() {
    float currentTime = static_cast<float>(glfwGetTime());
    float deltaTime = currentTime - lastFrameTime_;
    lastFrameTime_ = currentTime;

    glfwPollEvents();
    onTick(deltaTime);
    glfwSwapBuffers(window_);
}

bool Application::shouldQuit() const {
    return glfwWindowShouldClose(window_) != 0;
}

} // namespace vibegl
```

### 1.2 Create ShaderManager

**`src/rendering/ShaderManager.hpp`**:
```cpp
#pragma once

#include <string>
#include <glad/glad.h>

namespace vibegl {

class ShaderManager {
public:
    // Load shader pair with automatic platform suffix
    // e.g., "cube" loads "cube_gl46.vert" on desktop, "cube_es3.vert" on web
    static GLuint loadProgram(const std::string& baseName);

    // Load with explicit paths (for special cases)
    static GLuint loadProgramFromFiles(
        const std::string& vertPath,
        const std::string& fragPath
    );

private:
    static std::string readFile(const std::string& path);
    static GLuint compileShader(GLenum type, const std::string& source);
    static GLuint linkProgram(GLuint vertShader, GLuint fragShader);
};

} // namespace vibegl
```

### 1.3 Create TextureLoader

**`src/rendering/TextureLoader.hpp`**:
```cpp
#pragma once

#include <string>
#include <glad/glad.h>

namespace vibegl {

class TextureLoader {
public:
    struct TextureInfo {
        GLuint id = 0;
        int width = 0;
        int height = 0;
    };

    static TextureInfo load(const std::string& path);
    static void destroy(GLuint textureId);
};

} // namespace vibegl
```

### 1.4 Create VibeGLApp (Concrete Application)

**`src/VibeGLApp.hpp`**:
```cpp
#pragma once

#include "core/Application.hpp"
#include <array>

namespace vibegl {

class VibeGLApp : public Application {
public:
    VibeGLApp();

protected:
    void onInit() override;
    void onTick(float deltaTime) override;
    void onShutdown() override;

private:
    void renderScene();
    void renderUI();

    // OpenGL resources
    GLuint shaderProgram_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLuint texture_ = 0;

    // Scene state
    float rotationAngle_ = 0.0f;
    float rotationVelocity_ = 45.0f;
    std::array<float, 3> rotationAxis_ = {0.5f, 1.0f, 0.0f};
    std::array<float, 3> cubeColor_ = {1.0f, 1.0f, 1.0f};
};

} // namespace vibegl
```

### 1.5 Simplify main.cpp

**`src/main.cpp`** (~25 lines):
```cpp
#include "core/Platform.hpp"
#include "VibeGLApp.hpp"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

int main() {
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Starting VibeGL...");

    try {
        vibegl::VibeGLApp app;
        app.run();
    } catch (const std::exception& e) {
        spdlog::error("Fatal error: {}", e.what());
        return 1;
    }

    spdlog::info("Shutdown complete");
    return 0;
}
```

### 1.6 Update CMakeLists.txt for New Structure

Update `src/CMakeLists.txt` to compile all new source files.

### 1.7 Validation

- [ ] Native build works identically to before refactor
- [ ] All tests pass
- [ ] No functional changes, just reorganization
- [ ] Code compiles without warnings

---

## Phase 2: CMake Emscripten Configuration

**Goal**: Configure build system for Emscripten without code changes.

### 2.1 Add fmt as External Dependency

**Update `cmake/Dependencies.cmake`**:
```cmake
# fmt library (required for spdlog with Emscripten)
message(STATUS "Fetching fmt...")
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        11.1.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(fmt)

# spdlog with external fmt (fixes Emscripten consteval issues)
message(STATUS "Fetching spdlog...")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.0
    GIT_SHALLOW    TRUE
)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)
```

### 2.2 Conditional Dependencies for Emscripten

**Update `cmake/Dependencies.cmake`**:
```cmake
# GLFW - Emscripten provides its own via -sUSE_GLFW=3
if(NOT EMSCRIPTEN)
    message(STATUS "Fetching GLFW...")
    FetchContent_Declare(glfw ...)
    FetchContent_MakeAvailable(glfw)
else()
    # Create interface target for compatibility
    add_library(glfw INTERFACE)
endif()

# GLAD - Not needed for Emscripten (OpenGL ES provided directly)
if(NOT EMSCRIPTEN)
    message(STATUS "Fetching GLAD...")
    FetchContent_Declare(glad ...)
    FetchContent_MakeAvailable(glad)
else()
    add_library(glad INTERFACE)
endif()
```

### 2.3 Add Emscripten CMake Preset

**Update `CMakePresets.json`**:
```json
{
    "name": "emscripten",
    "displayName": "Emscripten (WebAssembly)",
    "description": "Build for web browsers using Emscripten",
    "generator": "Ninja",
    "binaryDir": "${sourceDir}/build/emscripten",
    "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "CMAKE_TOOLCHAIN_FILE": "$env{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake",
        "CMAKE_CXX_STANDARD": "20",
        "BUILD_TESTS": "OFF"
    }
}
```

Note: C++20 for Emscripten (C++23 has some compatibility issues).

### 2.4 Update src/CMakeLists.txt for Emscripten

```cmake
if(EMSCRIPTEN)
    set_target_properties(vibegl PROPERTIES
        SUFFIX ".html"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    target_link_options(vibegl PRIVATE
        -sUSE_GLFW=3
        -sFULL_ES3=1
        -sALLOW_MEMORY_GROWTH=1
        -sINITIAL_MEMORY=64MB
        --preload-file ${CMAKE_SOURCE_DIR}/data@/data
    )

    target_compile_definitions(vibegl PRIVATE
        GLFW_INCLUDE_ES3
    )
endif()
```

### 2.5 Platform-Specific Link Libraries

```cmake
if(NOT EMSCRIPTEN)
    # Native: X11, pthread, dl
    if(UNIX AND NOT APPLE)
        find_package(X11 REQUIRED)
        target_link_libraries(vibegl PRIVATE X11::X11 pthread ${CMAKE_DL_LIBS})
    endif()
endif()
```

### 2.6 Validation

- [ ] Native build still works
- [ ] `cmake --preset emscripten` configures without errors
- [ ] Dependencies resolve correctly for both platforms

---

## Phase 3: Web-Specific Code Adaptations

**Goal**: Make the code compile and run on Emscripten.

### 3.1 Create WebGL Shader Variants

Rename existing shaders and create ES3 versions:
```
data/shaders/
├── cube_gl46.vert    # Desktop OpenGL 4.6
├── cube_gl46.frag
├── cube_es3.vert     # WebGL 2.0 (GLSL ES 3.0)
└── cube_es3.frag
```

**`cube_es3.frag`** (requires precision qualifier):
```glsl
#version 300 es
precision highp float;

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uColor;

void main() {
    vec4 texColor = texture(uTexture, vTexCoord);
    FragColor = texColor * vec4(uColor, 1.0);
}
```

### 3.2 Update ShaderManager for Platform Variants

```cpp
GLuint ShaderManager::loadProgram(const std::string& baseName) {
    std::string vertPath = "shaders/" + baseName + kShaderSuffix + ".vert";
    std::string fragPath = "shaders/" + baseName + kShaderSuffix + ".frag";
    return loadProgramFromFiles(vertPath, fragPath);
}
```

### 3.3 Conditional OpenGL Includes

**`src/core/GLIncludes.hpp`**:
```cpp
#pragma once

#ifdef __EMSCRIPTEN__
    #define GLFW_INCLUDE_ES3
    #include <GLFW/glfw3.h>
#else
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
#endif
```

### 3.4 Conditional GLAD Initialization

In `Application.cpp` or `main.cpp`:
```cpp
#ifndef __EMSCRIPTEN__
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
#endif
```

### 3.5 ImGui Backend Configuration

```cpp
#ifdef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_Init("#version 300 es");
#else
    ImGui_ImplOpenGL3_Init("#version 460 core");
#endif
```

Or use the `kGLSLVersion` constant from `Platform.hpp`.

### 3.6 Validation

- [ ] `cmake --build build/emscripten` compiles successfully
- [ ] Output files generated: `vibegl.html`, `vibegl.js`, `vibegl.wasm`, `vibegl.data`
- [ ] Native build still works
- [ ] All native tests pass

---

## Phase 4: Testing & Documentation

**Goal**: Verify everything works and document the process.

### 4.1 Manual Browser Testing

```bash
# Build
source ~/dev/emsdk/emsdk_env.sh
cmake --preset emscripten
cmake --build build/emscripten

# Serve locally (Emscripten provides a server)
cd build/emscripten/bin
python3 -m http.server 8080
# Open http://localhost:8080/vibegl.html
```

**Test checklist:**
- [ ] Page loads without console errors
- [ ] Cube renders with texture
- [ ] Cube rotates
- [ ] ImGui panel appears
- [ ] Sliders work (rotation, color)
- [ ] FPS counter updates
- [ ] No WebGL errors in console

### 4.2 Add Web Build Script

**`scripts/build-web.sh`**:
```bash
#!/bin/bash
set -e

if [ -z "$EMSDK" ]; then
    echo "Error: EMSDK environment not set"
    echo "Run: source /path/to/emsdk/emsdk_env.sh"
    exit 1
fi

cmake --preset emscripten
cmake --build build/emscripten --parallel

echo "Build complete: build/emscripten/bin/vibegl.html"

# Optional: start local server
if [[ "$1" == "--serve" ]]; then
    PORT="${2:-8080}"
    echo ""
    echo "Starting server at http://localhost:$PORT/vibegl.html"
    echo "Press Ctrl+C to stop"
    cd build/emscripten/bin
    python3 -m http.server "$PORT"
fi
```

Usage:
- `./scripts/build-web.sh` - Build only
- `./scripts/build-web.sh --serve` - Build and serve on port 8080
- `./scripts/build-web.sh --serve 3000` - Build and serve on custom port

### 4.3 Update README

Add section on web builds:
```markdown
## Web Build (Emscripten)

### Prerequisites
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)

### Building
source /path/to/emsdk/emsdk_env.sh
./scripts/build-web.sh

### Running
cd build/emscripten/bin
python3 -m http.server 8080
# Open http://localhost:8080/vibegl.html
```

### 4.4 Optional: CI for Web Build

Add GitHub Actions workflow for Emscripten build (compile-only, no browser tests):
```yaml
emscripten:
  runs-on: ubuntu-latest
  steps:
    - uses: mymindstorm/setup-emsdk@v14
    - run: cmake --preset emscripten
    - run: cmake --build build/emscripten
```

### 4.5 Final Validation

- [ ] Fresh clone + build works (native)
- [ ] Fresh clone + build works (emscripten)
- [ ] README instructions are complete
- [ ] CI passes for both platforms

---

## Summary

| Phase | Focus | Risk Level | Estimated Effort |
|-------|-------|------------|------------------|
| 0 | Foundation (LTO, structure) | Low | Small |
| 1 | Abstractions (Application, ShaderManager) | Medium | Medium |
| 2 | CMake Emscripten config | Medium | Small |
| 3 | Web-specific code | Medium | Small |
| 4 | Testing & docs | Low | Small |

**Total #ifdef locations: 4 files**
- `Platform.hpp` - constexpr platform flags
- `Application.cpp` - main loop
- `GLIncludes.hpp` - OpenGL headers
- `main.cpp` or `Application.cpp` - GLAD init

**Key insight**: The spdlog issue is solved by using external fmt 11.1.4+, requiring zero code changes for logging.

---

## Decisions Made

- **`--serve` option**: Yes, added to build script with optional custom port
- **Visual regression testing**: No, overkill for this template
- **ImGui docking**: Keep enabled for web builds
