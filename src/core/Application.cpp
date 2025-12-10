#include "Application.hpp"

#include "Platform.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace vibegl
{

Application::Application(const WindowConfig& config)
{
    if (!initWindow(config))
    {
        throw std::runtime_error("Failed to initialize window");
    }

    if (!initOpenGL())
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize OpenGL");
    }

    initImGui();
    initialized_ = true;
}

Application::~Application()
{
    if (initialized_)
    {
        shutdownImGui();
    }
    if (window_ != nullptr)
    {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

bool Application::initWindow(const WindowConfig& config)
{
    if (glfwInit() == GLFW_FALSE)
    {
        spdlog::error("Failed to initialize GLFW");
        return false;
    }

#ifdef __EMSCRIPTEN__
    // WebGL 2.0 context (OpenGL ES 3.0)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
    // Desktop OpenGL 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#endif

    window_ = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr)
    {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);

    // Set up callbacks
    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* /*win*/, int width, int height)
                                   { glViewport(0, 0, width, height); });

    glfwSetKeyCallback(window_,
                       [](GLFWwindow* win, int key, int /*scancode*/, int action, int /*mods*/)
                       {
                           if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                           {
                               glfwSetWindowShouldClose(win, GLFW_TRUE);
                           }
                       });

    if (config.vsync)
    {
        glfwSwapInterval(1);
    }

    return true;
}

bool Application::initOpenGL()
{
#ifndef __EMSCRIPTEN__
    // GLAD is only needed on desktop - Emscripten provides OpenGL ES directly
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
    {
        spdlog::error("Failed to initialize GLAD");
        return false;
    }
#endif

    spdlog::info("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::info("GLSL Version: {}",
                 reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    return true;
}

void Application::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(kGLSLVersionString);
}

void Application::shutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::run()
{
    spdlog::info("Entering main loop");
    onInit();

#ifdef __EMSCRIPTEN__
    // Emscripten: browser controls the main loop via requestAnimationFrame
    emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, 1);
    // Note: emscripten_set_main_loop_arg does not return when simulate_infinite_loop=1
#else
    // Desktop: traditional main loop
    while (!shouldQuit())
    {
        tick();
    }

    spdlog::info("Shutting down...");
    onShutdown();
#endif
}

void Application::tick()
{
    auto currentTime = static_cast<float>(glfwGetTime());
    float deltaTime = currentTime - lastFrameTime_;
    lastFrameTime_ = currentTime;

    glfwPollEvents();
    onTick(deltaTime);
}

void Application::emscriptenMainLoop(void* arg)
{
    auto* app = static_cast<Application*>(arg);
    app->tick();
}

bool Application::shouldQuit() const
{
    return glfwWindowShouldClose(window_) != 0;
}

int Application::getWindowWidth() const
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    return width;
}

int Application::getWindowHeight() const
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    return height;
}

float Application::getAspectRatio() const
{
    int width = getWindowWidth();
    int height = getWindowHeight();
    if (height == 0)
    {
        return 1.0f;
    }
    return static_cast<float>(width) / static_cast<float>(height);
}

void Application::endFrame()
{
    glfwSwapBuffers(window_);
}

} // namespace vibegl
