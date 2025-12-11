#pragma once

/// @file VibeGLApp.hpp
/// @brief Demo application showing a rotating textured cube

#include "core/Application.hpp"
#include <array>

namespace vibegl {

/// @brief Demo application with rotating textured cube and ImGui controls
class VibeGLApp : public Application {
public:
    VibeGLApp();
    ~VibeGLApp() override;

    // Non-copyable, non-movable
    VibeGLApp(const VibeGLApp&) = delete;
    VibeGLApp& operator=(const VibeGLApp&) = delete;
    VibeGLApp(VibeGLApp&&) = delete;
    VibeGLApp& operator=(VibeGLApp&&) = delete;

protected:
    /// @brief Initialize OpenGL resources (shaders, textures, geometry)
    void onInit() override;

    /// @brief Update and render the rotating cube each frame
    /// @param deltaTime Time elapsed since last frame in seconds
    void onTick(float deltaTime) override;

    /// @brief Clean up OpenGL resources
    void onShutdown() override;

private:
    /// @brief Set up cube vertex and index buffers
    void setupCubeGeometry();

    /// @brief Render the textured cube with current transform
    void renderCube();

    /// @brief Render ImGui control panel
    void renderUI();

    // OpenGL resources
    GLuint shaderProgram_ = 0;     ///< Compiled shader program
    GLuint texture_ = 0;            ///< Loaded texture
    GLuint vao_ = 0;                ///< Vertex Array Object
    GLuint vbo_ = 0;                ///< Vertex Buffer Object
    GLuint ebo_ = 0;                ///< Element Buffer Object

    // Animation state
    float rotationAngle_ = 0.0f;                           ///< Current rotation angle in degrees
    float rotationVelocity_ = 45.0f;                       ///< Rotation speed in degrees per second
    std::array<float, 3> rotationAxis_ = {0.5f, 1.0f, 0.0f};  ///< Rotation axis (X, Y, Z)
    std::array<float, 3> cubeColor_ = {1.0f, 1.0f, 1.0f};     ///< Cube tint color (RGB)
};

} // namespace vibegl
