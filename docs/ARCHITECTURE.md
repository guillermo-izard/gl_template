# VibeGL Architecture Documentation

This document explains the design decisions behind VibeGL and how to customize it for your own projects.

## Design Philosophy

### Starter Template Approach

VibeGL is designed as a **starter template**, not a library. You are expected to:
- Fork or copy this repository
- Modify the code directly for your needs
- Delete demo code and replace it with your application
- Use the patterns established here as a foundation

This approach prioritizes **clarity and transparency** over API stability or backwards compatibility.

### Inheritance-Based Application Pattern

The `Application` base class uses inheritance to provide lifecycle hooks:

```cpp
class MyApp : public Application {
protected:
    void onInit() override { /* load resources */ }
    void onTick(float dt) override { /* update & render */ }
    void onShutdown() override { /* cleanup */ }
};
```

**Why inheritance?**
- Clear separation of concerns (framework vs. application logic)
- Familiar pattern for most developers
- Simple override mechanism for lifecycle events
- Platform abstraction handled automatically by base class

**Alternatives considered:**
- Composition with callbacks: More flexible but requires more boilerplate
- ECS architecture: Powerful for games but adds complexity

### Manual Resource Management

VibeGL uses raw `GLuint` handles for OpenGL resources (shaders, textures, buffers) with explicit cleanup:

```cpp
GLuint texture = textureResult.value();  // Raw handle
// ... use texture ...
TextureLoader::deleteTexture(texture);   // Explicit cleanup
```

**Why manual management?**
- **Transparency**: You see exactly what OpenGL calls are being made
- **Simplicity**: No wrapper abstractions to learn
- **Template flexibility**: Easy to replace with your own resource management system
- **Educational value**: Demonstrates direct OpenGL usage

**Future extensibility:**
- You can add RAII wrappers later when your project scales
- You can integrate a resource manager system when needed
- The current design doesn't prevent these additions

### Error Handling with std::expected

VibeGL uses C++23 `std::expected` for type-safe error handling:

```cpp
Result<GLuint> shader = ShaderManager::loadProgram("cube");
if (!shader) {
    spdlog::error("Shader failed: {} - {}",
                  shader.error().message,
                  shader.error().context);
    return;
}
GLuint program = shader.value();
```

**Why std::expected?**
- **Type-safe**: Compiler enforces error checking
- **Modern C++23**: Demonstrates current best practices
- **Contextual errors**: Error objects carry message + context (file paths, errno)
- **Clear pattern**: Easy for template users to follow in their own code

See `src/core/Result.hpp` for the type definitions.

---

## Platform Abstraction

VibeGL targets **desktop (OpenGL 4.6)** and **web (WebGL 2.0/ES 3.0)** through compile-time platform detection.

### Three-Layer Abstraction

#### 1. Platform.hpp - Compile-Time Constants

`src/core/Platform.hpp` defines `constexpr` values that are resolved at compile time:

```cpp
#ifdef __EMSCRIPTEN__
    constexpr bool kIsWeb = true;
    constexpr bool kIsDesktop = false;
    constexpr const char* kGLSLVersionString = "#version 300 es";
    constexpr const char* kShaderSuffix = "_es3";
#else
    constexpr bool kIsWeb = false;
    constexpr bool kIsDesktop = true;
    constexpr const char* kGLSLVersionString = "#version 460 core";
    constexpr const char* kShaderSuffix = "_gl46";
#endif
```

**Benefits:**
- Zero runtime overhead - dead code is eliminated by compiler
- Use `if constexpr (kIsWeb)` for platform-specific logic
- Shader suffix automatically selects correct GLSL version

#### 2. GLIncludes.hpp - Header Abstraction

`src/core/GLIncludes.hpp` handles OpenGL header differences:

```cpp
#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>      // Emscripten provides ES3
#else
    #include <glad/glad.h>      // GLAD loads desktop OpenGL
#endif
#include <GLFW/glfw3.h>
```

**Why separate file?**
- All OpenGL-dependent files include this ONE header
- Platform differences isolated to single location
- Easy to add new platforms (e.g., Vulkan backend)

#### 3. Application.cpp - Main Loop Abstraction

`src/core/Application.cpp` handles the platform-specific main loop:

**Desktop:**
```cpp
while (!shouldQuit()) {
    tick();  // Update and render
}
```

**Web:**
```cpp
emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, true);
// Returns immediately, browser calls emscriptenMainLoop each frame
```

**Why different?**
- Web browsers require returning control to the event loop
- Desktop can use traditional blocking loops
- `Application::run()` hides this complexity from derived classes

---

## Shader System

### Platform-Specific Shader Files

VibeGL maintains **two versions** of each shader:

```
data/shaders/
├── cube_gl46.vert   # Desktop: OpenGL 4.6 Core
├── cube_gl46.frag
├── cube_es3.vert    # Web: OpenGL ES 3.0
└── cube_es3.frag
```

**Why separate files?**
- **Clarity**: Easy to see which version is for which platform
- **Flexibility**: Shaders may diverge as you add features
- **Simplicity**: No preprocessor magic or build-time transpilation

**Shader differences:**
```glsl
// Desktop: cube_gl46.vert
#version 460 core
in vec3 aPosition;
// ...

// Web: cube_es3.vert
#version 300 es
precision highp float;  // ES requires precision qualifiers
in vec3 aPosition;
// ...
```

### Automatic Shader Loading

`ShaderManager::loadProgram("cube")` automatically appends the platform suffix:

```cpp
// Desktop loads: cube_gl46.vert, cube_gl46.frag
// Web loads: cube_es3.vert, cube_es3.frag
auto shader = ShaderManager::loadProgram("cube", "data/shaders/");
```

### Adding New Shaders

1. Create both versions: `myshader_gl46.{vert,frag}` and `myshader_es3.{vert,frag}`
2. Load with: `ShaderManager::loadProgram("myshader", resolvePath("data/shaders/"))`
3. Validate with: `./scripts/validate-shaders.sh`

**Note:** Always use `resolvePath()` when loading assets to respect the configured asset base path.

---

## Extension Points

### Where to Add Your Own Code

#### 1. Create Your Application Class

Replace `VibeGLApp` with your own derived class:

```cpp
// MyApp.hpp
class MyApp : public vibegl::Application {
public:
    MyApp();
    ~MyApp() override;

protected:
    void onInit() override;
    void onTick(float deltaTime) override;
    void onShutdown() override;

private:
    // Your member variables
    GLuint myShader_ = 0;
    GLuint myTexture_ = 0;
};
```

#### 2. Load Your Resources in onInit()

```cpp
void MyApp::onInit() {
    auto shaderResult = ShaderManager::loadProgram("myshader", resolvePath("data/shaders/"));
    if (!shaderResult) {
        spdlog::error("Failed to load shader: {} - {}",
                      shaderResult.error().message,
                      shaderResult.error().context);
        return;
    }
    myShader_ = shaderResult.value();

    // Cache uniform locations immediately after shader load
    myUniforms_.mvp = glGetUniformLocation(myShader_, "uMVP");

    // Load textures, create buffers, etc.
}
```

#### 3. Implement Your Render Loop in onTick()

```cpp
void MyApp::onTick(float deltaTime) {
    // Update logic
    updateGameState(deltaTime);

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render your scene
    renderScene();

    // Optional: ImGui for debugging
    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
    ImGui::End();

    endFrame();  // Swap buffers
}
```

#### 4. Clean Up in onShutdown()

```cpp
void MyApp::onShutdown() {
    if (myShader_ != 0) {
        ShaderManager::deleteProgram(myShader_);
    }
    if (myTexture_ != 0) {
        TextureLoader::deleteTexture(myTexture_);
    }
    // Delete buffers, VAOs, etc.
}
```

#### 5. Configure Your Window

```cpp
int main() {
    try {
        MyApp app(vibegl::WindowConfig{
            .title = "My Application",
            .width = 1920,
            .height = 1080,
            .vsync = true,
            .assetBasePath = ""  // Empty = current directory
        });
        app.run();
    } catch (const std::exception& e) {
        spdlog::error("Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}
```

---

## What to Delete (Demo Code)

When starting your own project, you can safely **delete** these demo-specific files:

### Application Code
- `src/VibeGLApp.hpp` - Demo application header
- `src/VibeGLApp.cpp` - Demo application implementation

### Demo Assets
- `data/shaders/cube_gl46.vert` - Demo vertex shader (desktop)
- `data/shaders/cube_gl46.frag` - Demo fragment shader (desktop)
- `data/shaders/cube_es3.vert` - Demo vertex shader (web)
- `data/shaders/cube_es3.frag` - Demo fragment shader (web)
- `data/textures/sample.png` - Demo texture

### Keep These (Core Framework)
- `src/core/` - **Keep** all files (Application, Platform, GLIncludes, Result)
- `src/rendering/` - **Keep** ShaderManager and TextureLoader utilities
- `src/main.cpp` - **Modify** to instantiate your application instead of VibeGLApp
- `cmake/` - **Keep** all build configuration
- `tests/` - **Keep** and expand with your own tests
- `.clang-format`, `.clang-tidy` - **Keep** for code quality

---

## Best Practices for Template Users

### 1. Error Handling Pattern

Always check `Result<T>` return values:

```cpp
auto resource = loadSomething();
if (!resource) {
    spdlog::error("Load failed: {} - {}",
                  resource.error().message,
                  resource.error().context);
    // Handle error appropriately
    return;
}
// Use resource.value()
```

### 2. Asset Path Resolution

Always use `resolvePath()` for asset loading:

```cpp
// Good
auto shader = ShaderManager::loadProgram("cube", resolvePath("data/shaders/"));
auto texture = TextureLoader::loadTexture(resolvePath("data/textures/image.png"));

// Bad - hardcoded paths break asset base path configuration
auto shader = ShaderManager::loadProgram("cube", "data/shaders/");
```

### 3. Uniform Location Caching

Cache uniform locations once during initialization:

```cpp
// In onInit(), after shader load:
myLocations_.mvp = glGetUniformLocation(shader, "uMVP");

// In onTick(), use cached location:
glUniformMatrix4fv(myLocations_.mvp, 1, GL_FALSE, glm::value_ptr(mvp));

// Don't do this every frame:
// glUniformMatrix4fv(glGetUniformLocation(shader, "uMVP"), ...);
```

### 4. Platform-Specific Code

Use `if constexpr` for platform branching:

```cpp
if constexpr (vibegl::kIsWeb) {
    // Web-specific code
    emscripten_run_script("console.log('Hello from WebAssembly')");
} else {
    // Desktop-specific code
    glfwSetWindowIcon(window, ...);
}
```

### 5. Resource Cleanup

Always clean up resources in `onShutdown()`:

```cpp
void MyApp::onShutdown() {
    // Shaders
    if (shader_ != 0) {
        ShaderManager::deleteProgram(shader_);
    }

    // Textures
    if (texture_ != 0) {
        TextureLoader::deleteTexture(texture_);
    }

    // Buffers and VAOs
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
}
```

---

## Build System Notes

### CMake Presets

Use presets for consistent builds:

```bash
cmake --preset debug           # Debug build with tests
cmake --preset release         # Optimized release build
cmake --preset sanitizers      # ASan + UBSan for bug detection
cmake --preset emscripten      # WebAssembly build
```

### Adding New Source Files

Edit `src/CMakeLists.txt` and add your files to the `vibegl` target:

```cmake
add_executable(vibegl
    main.cpp
    MyApp.cpp              # Add your files here
    MyOtherClass.cpp
    core/Application.cpp
    # ...
)
```

### Web Build Specifics

- Uses C++20 (Emscripten limitation) vs C++23 for desktop
- ImGui automatically uses correct GLSL version via `kGLSLVersionString`
- Build with `./scripts/build-web.sh` for convenience
- Output: `build/emscripten/bin/vibegl.html`

---

## Future Directions

As your project grows, consider these enhancements:

### Resource Management System
When you have many assets, implement:
- RAII wrappers (`UniqueShader`, `UniqueTexture`) with move semantics
- Resource manager with handle-based access
- Automatic reference counting

### Configuration System
Replace hardcoded values with:
- JSON/TOML configuration files
- Command-line argument parsing
- Runtime settings hot-reloading

### Build Improvements
- Shader hot-reloading in debug mode
- Embedded resources (no runtime file I/O)
- Shader compilation validation in CI

### Cross-Platform Expansion
- Add Vulkan backend alongside OpenGL
- Support mobile platforms (iOS/Android)
- Investigate Metal for macOS

---

## Summary

VibeGL provides a solid foundation for cross-platform OpenGL development:

- **Clean architecture** with platform abstraction
- **Modern C++23** patterns (std::expected, constexpr)
- **Starter template** design for easy customization
- **Best practices** demonstrated throughout

Fork it, customize it, and build something awesome!
