/// @file main.cpp
/// @brief Application entry point

#include <spdlog/spdlog.h>

#include <exception>

#include "VibeGLApp.hpp"

int main()
{
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Starting VibeGL...");

    try
    {
        vibegl::VibeGLApp app;
        app.run();
    }
    catch (const std::exception& e)
    {
        spdlog::error("Fatal error: {}", e.what());
        return 1;
    }

    return 0;
}
