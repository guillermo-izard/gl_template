#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Window configuration
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr const char* WINDOW_TITLE = "VibeGL";

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

std::string readFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        spdlog::error("Failed to open file: {}", filepath);
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> errorLog(static_cast<size_t>(logLength));
        glGetShaderInfoLog(shader, logLength, &logLength, errorLog.data());
        spdlog::error("Shader compilation failed: {}", errorLog.data());
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath)
{
    std::string vertSource = readFile(vertPath);
    std::string fragSource = readFile(fragPath);

    if (vertSource.empty() || fragSource.empty())
    {
        return 0;
    }

    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertSource);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragSource);

    if (vertShader == 0 || fragShader == 0)
    {
        if (vertShader != 0)
        {
            glDeleteShader(vertShader);
        }
        if (fragShader != 0)
        {
            glDeleteShader(fragShader);
        }
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> errorLog(static_cast<size_t>(logLength));
        glGetProgramInfoLog(program, logLength, &logLength, errorLog.data());
        spdlog::error("Shader program linking failed: {}", errorLog.data());
        glDeleteProgram(program);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        return 0;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

GLuint loadTexture(const std::string& filepath)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

    if (data == nullptr)
    {
        spdlog::error("Failed to load texture: {}", filepath);
        return 0;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    spdlog::info("Loaded texture: {} ({}x{})", filepath, width, height);
    return texture;
}

void framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main()
{
    // Initialize spdlog
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Starting VibeGL...");

    // Initialize GLFW
    if (glfwInit() == GLFW_FALSE)
    {
        spdlog::error("Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window =
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr)
    {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLAD
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
    {
        spdlog::error("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    spdlog::info("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::info("GLSL Version: {}",
                 reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // Create shader program
    GLuint shaderProgram = createShaderProgram("shaders/cube.vert", "shaders/cube.frag");
    if (shaderProgram == 0)
    {
        spdlog::error("Failed to create shader program");
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Load texture
    GLuint texture = loadTexture("textures/sample.png");
    if (texture == 0)
    {
        spdlog::error("Failed to load texture");
        glDeleteProgram(shaderProgram);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Create VAO, VBO, and EBO
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * CUBE_VERTICES.size(), CUBE_VERTICES.data(),
                 GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * CUBE_INDICES.size(), CUBE_INDICES.data(),
                 GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Application state
    float rotationAngle = 0.0F;
    float rotationVelocity = 45.0F; // degrees per second
    std::array<float, 3> rotationAxis = {0.5F, 1.0F, 0.0F};
    std::array<float, 3> cubeColor = {1.0F, 1.0F, 1.0F};

    float lastFrameTime = 0.0F;

    spdlog::info("Entering main loop");

    // Main loop
    while (glfwWindowShouldClose(window) == 0)
    {
        // Calculate delta time
        auto currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        // Update rotation
        rotationAngle += rotationVelocity * deltaTime;
        if (rotationAngle >= 360.0F)
        {
            rotationAngle -= 360.0F;
        }

        // Poll events
        glfwPollEvents();

        // Rendering (done before ImGui so UI is on top)
        glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);

        // Set uniforms
        glm::mat4 model = glm::mat4(1.0F);
        glm::vec3 axis(rotationAxis[0], rotationAxis[1], rotationAxis[2]);
        if (glm::length(axis) > 0.0F)
        {
            axis = glm::normalize(axis);
            model = glm::rotate(model, glm::radians(rotationAngle), axis);
        }

        glm::mat4 view = glm::mat4(1.0F);
        view = glm::translate(view, glm::vec3(0.0F, 0.0F, -3.0F));

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        glm::mat4 projection = glm::perspective(glm::radians(45.0F), aspect, 0.1F, 100.0F);

        glm::mat4 mvp = projection * view * model;

        GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
        glUniform3fv(colorLoc, 1, cubeColor.data());

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        GLint texLoc = glGetUniformLocation(shaderProgram, "uTexture");
        glUniform1i(texLoc, 0);

        // Draw cube using indexed rendering
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(CUBE_INDICES.size()), GL_UNSIGNED_INT,
                       nullptr);
        glBindVertexArray(0);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Control panel - floating window on the right
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(width) - 300.0F, 20.0F),
                                ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(280.0F, 200.0F), ImGuiCond_FirstUseEver);
        ImGui::Begin("Controls");
        ImGui::Text("FPS: %.1f", static_cast<double>(io.Framerate));
        ImGui::Separator();
        ImGui::SliderFloat3("Rotation Axis", rotationAxis.data(), -1.0F, 1.0F, "%.2f");
        ImGui::SliderFloat("Rotation Velocity", &rotationVelocity, -180.0F, 180.0F, "%.1f deg/s");
        ImGui::ColorEdit3("Cube Color", cubeColor.data());
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    spdlog::info("Shutting down...");
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
