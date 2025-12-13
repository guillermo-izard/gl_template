#pragma once

/// @file
/// Base application class with platform-abstracted main loop.

#include "GLIncludes.hpp"
#include <string>

namespace vibegl {

/// Configuration for creating an Application window.
struct WindowConfig {
    std::string title = "VibeGL";  ///< Window title displayed in title bar
    int width = 1280;               ///< Initial window width in pixels
    int height = 720;               ///< Initial window height in pixels
    bool vsync = true;              ///< Enable vertical synchronization
};

/// Base class for applications with platform-abstracted main loop.
///
/// Derive from this class and implement onInit(), onTick(), and onShutdown().
/// The run() method handles the platform-specific main loop:
/// - Desktop: Traditional while loop
/// - Web: emscripten_set_main_loop callback
///
/// Example:
/// ```cpp
/// class MyApp : public Application {
/// protected:
///     void onInit() override { /* load resources */ }
///     void onTick(float dt) override { /* update & render */ }
///     void onShutdown() override { /* cleanup */ }
/// };
///
/// int main() {
///     MyApp app;
///     app.run();
/// }
/// ```
class Application {
public:
    /// Construct application with given window configuration.
    explicit Application(const WindowConfig& config = {});

    /// Virtual destructor for proper cleanup in derived classes.
    virtual ~Application();

    // Non-copyable, non-movable
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    /// Start the main loop (blocking on desktop, returns on web).
    void run();

protected:
    /// Called once after window and OpenGL context are ready.
    virtual void onInit() {}

    /// Called every frame with delta time in seconds.
    /// @param deltaTime Time elapsed since last frame
    virtual void onTick(float deltaTime) = 0;

    /// Called once before application exits (desktop only).
    virtual void onShutdown() {}

    /// Check if application should quit.
    /// @return true if window close was requested
    virtual bool shouldQuit() const;

    // Accessors for derived classes
    GLFWwindow* getWindow() const { return window_; }
    int getWindowWidth() const;
    int getWindowHeight() const;
    float getAspectRatio() const;

    /// Swap buffers and poll events (call at end of onTick).
    void endFrame();

private:
    /// Initialize GLFW and create window.
    bool initWindow(const WindowConfig& config);

    /// Initialize OpenGL loader (GLAD on desktop).
    bool initOpenGL();

    /// Initialize ImGui with GLFW and OpenGL backends.
    void initImGui();

    /// Shutdown ImGui.
    void shutdownImGui();

    /// Internal tick function called by main loop.
    void tick();

    /// Static callback for Emscripten main loop.
    static void emscriptenMainLoop(void* arg);

    GLFWwindow* window_ = nullptr;
    float lastFrameTime_ = 0.0f;
    bool initialized_ = false;
};

} // namespace vibegl
