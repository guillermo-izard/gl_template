#pragma once

/// @file
/// Demo application showing a rotating textured cube.

#include "core/Application.hpp"
#include <array>

namespace vibegl {

/// Demo application with rotating textured cube and ImGui controls.
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
    void onInit() override;
    void onTick(float deltaTime) override;
    void onShutdown() override;

private:
    void setupCubeGeometry();
    void renderCube();
    void renderUI();

    // OpenGL resources
    GLuint shaderProgram_ = 0;
    GLuint texture_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;

    // Animation state
    float rotationAngle_ = 0.0f;
    float rotationVelocity_ = 45.0f;
    std::array<float, 3> rotationAxis_ = {0.5f, 1.0f, 0.0f};
    std::array<float, 3> cubeColor_ = {1.0f, 1.0f, 1.0f};
};

} // namespace vibegl
