#include "VibeGLApp.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include <array>

#include "rendering/ShaderManager.hpp"
#include "rendering/TextureLoader.hpp"

namespace vibegl
{

// Cube vertices using indexed drawing (position: x,y,z, texcoord: u,v)
// 24 unique vertices (4 per face, needed for proper texture coordinates per face)
// clang-format off
constexpr std::array<float, 120> CUBE_VERTICES = {
    // Front face (0-3)
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    // Back face (4-7)
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    // Top face (8-11)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    // Bottom face (12-15)
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
    // Right face (16-19)
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    // Left face (20-23)
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

// Cube indices (6 faces × 2 triangles × 3 vertices = 36 indices)
constexpr std::array<GLuint, 36> CUBE_INDICES = {
    // Front face
    0, 1, 2,  2, 3, 0,
    // Back face
    4, 5, 6,  6, 7, 4,
    // Top face
    8, 9, 10,  10, 11, 8,
    // Bottom face
    12, 13, 14,  14, 15, 12,
    // Right face
    16, 17, 18,  18, 19, 16,
    // Left face
    20, 21, 22,  22, 23, 20
};
// clang-format on

VibeGLApp::VibeGLApp() : Application(WindowConfig{"VibeGL", 1280, 720, true}) {}

VibeGLApp::~VibeGLApp() = default;

void VibeGLApp::onInit()
{
    // Load shader program with automatic platform suffix
    auto shaderResult = ShaderManager::loadProgram("cube", resolvePath("data/shaders/"));
    if (!shaderResult)
    {
        spdlog::error("Failed to create shader program: {} - {}",
                      shaderResult.error().message,
                      shaderResult.error().context);
        return;
    }
    shaderProgram_ = shaderResult.value();

    // Cache shader uniform locations for efficiency (avoid glGetUniformLocation per frame)
    shaderLocations_.mvp = glGetUniformLocation(shaderProgram_, "uMVP");
    shaderLocations_.color = glGetUniformLocation(shaderProgram_, "uColor");
    shaderLocations_.texture = glGetUniformLocation(shaderProgram_, "uTexture");

    // Load texture
    auto textureResult = TextureLoader::loadTexture(resolvePath("data/textures/sample.png"));
    if (!textureResult)
    {
        spdlog::error("Failed to load texture: {} - {}",
                      textureResult.error().message,
                      textureResult.error().context);
        return;
    }
    texture_ = textureResult.value();

    setupCubeGeometry();
    glEnable(GL_DEPTH_TEST);
}

void VibeGLApp::onTick(float deltaTime)
{
    // Update rotation
    rotationAngle_ += rotationVelocity_ * deltaTime;
    if (rotationAngle_ >= 360.0f)
    {
        rotationAngle_ -= 360.0f;
    }

    // Clear
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderCube();
    renderUI();

    endFrame();
}

void VibeGLApp::onShutdown()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
    TextureLoader::deleteTexture(texture_);
    ShaderManager::deleteProgram(shaderProgram_);
}

void VibeGLApp::setupCubeGeometry()
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * CUBE_VERTICES.size(), CUBE_VERTICES.data(),
                 GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * CUBE_INDICES.size(), CUBE_INDICES.data(),
                 GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float))); // NOLINT(performance-no-int-to-ptr)
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void VibeGLApp::renderCube()
{
    glUseProgram(shaderProgram_);

    // Build model matrix
    auto model = glm::mat4(1.0f);
    glm::vec3 axis(rotationAxis_[0], rotationAxis_[1], rotationAxis_[2]);
    if (glm::length(axis) > 0.0f)
    {
        axis = glm::normalize(axis);
        model = glm::rotate(model, glm::radians(rotationAngle_), axis);
    }

    // View matrix
    auto view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // Projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), getAspectRatio(), 0.1f, 100.0f);

    // MVP
    glm::mat4 mvp = projection * view * model;

    // Use cached uniform locations (queried once during initialization)
    glUniformMatrix4fv(shaderLocations_.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(shaderLocations_.color, 1, cubeColor_.data());

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glUniform1i(shaderLocations_.texture, 0);

    // Draw
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(CUBE_INDICES.size()), GL_UNSIGNED_INT,
                   nullptr);
    glBindVertexArray(0);
}

void VibeGLApp::renderUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Control panel
    int width = getWindowWidth();
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(width) - 300.0f, 20.0f),
                            ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280.0f, 200.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls");

    ImGuiIO& io = ImGui::GetIO();
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    ImGui::Text("FPS: %.1f", static_cast<double>(io.Framerate));
    ImGui::Separator();
    ImGui::SliderFloat3("Rotation Axis", rotationAxis_.data(), -1.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Rotation Velocity", &rotationVelocity_, -180.0f, 180.0f, "%.1f deg/s");
    ImGui::ColorEdit3("Cube Color", cubeColor_.data());

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace vibegl
