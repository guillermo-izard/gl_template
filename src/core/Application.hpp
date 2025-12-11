#pragma once

/// @file Application.hpp
/// @brief Base application class with platform-abstracted main loop

#include "GLIncludes.hpp"
#include <string>

namespace vibegl {

/// @brief Configuration for creating an Application window
struct WindowConfig {
    std::string title = "VibeGL";
    int width = 1280;
    int height = 720;
    bool vsync = true;
};

/// @brief Base class for applications with platform-abstracted main loop
///
/// Derive from this class and implement onInit(), onTick(), and onShutdown().
/// The run() method handles the platform-specific main loop:
/// - Desktop: Traditional while loop
/// - Web: emscripten_set_main_loop callback
///
/// Example:
/// @code
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
/// @endcode
class Application {
public:
    /// @brief Construct application with given window configuration
    explicit Application(const WindowConfig& config = {});

    /// @brief Virtual destructor for proper cleanup in derived classes
    virtual ~Application();

    // Non-copyable, non-movable
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    /// @brief Start the main loop (blocking on desktop, returns on web)
    void run();

protected:
    /// @brief Called once after window and OpenGL context are ready
    virtual void onInit() {}

    /// @brief Called every frame with delta time in seconds
    /// @param deltaTime Time elapsed since last frame
    virtual void onTick(float deltaTime) = 0;

    /// @brief Called once before application exits (desktop only)
    virtual void onShutdown() {}

    /// @brief Check if application should quit
    /// @return true if window close was requested
    virtual bool shouldQuit() const;

    // Accessors for derived classes
    GLFWwindow* getWindow() const { return window_; }
    int getWindowWidth() const;
    int getWindowHeight() const;
    float getAspectRatio() const;

    /// @brief Swap buffers and poll events (call at end of onTick)
    void endFrame();

private:
    /// @brief Initialize GLFW and create window
    bool initWindow(const WindowConfig& config);

    /// @brief Initialize OpenGL loader (GLAD on desktop)
    bool initOpenGL();

    /// @brief Initialize ImGui with GLFW and OpenGL backends
    void initImGui();

    /// @brief Shutdown ImGui
    void shutdownImGui();

    /// @brief Internal tick function called by main loop
    void tick();

    /// @brief Static callback for Emscripten main loop
    static void emscriptenMainLoop(void* arg);

    GLFWwindow* window_ = nullptr;
    float lastFrameTime_ = 0.0f;
    bool initialized_ = false;
};

} // namespace vibegl
