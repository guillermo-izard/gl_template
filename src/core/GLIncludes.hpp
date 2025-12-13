#pragma once

/// @file
/// Platform-specific OpenGL header includes.
///
/// This header handles the differences between desktop OpenGL (GLAD loader)
/// and WebGL/OpenGL ES (provided directly by Emscripten).

#ifdef __EMSCRIPTEN__
    #ifndef GLFW_INCLUDE_ES3
        #define GLFW_INCLUDE_ES3
    #endif
    #include <GLFW/glfw3.h>
#else
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
#endif
