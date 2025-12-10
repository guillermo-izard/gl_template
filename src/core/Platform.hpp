#pragma once

/// @file Platform.hpp
/// @brief Compile-time platform detection and constants
///
/// This header provides constexpr values for platform-specific behavior.
/// All platform differences should be resolved at compile time where possible.

namespace vibegl {

/// @name Platform Detection
/// @{

#ifdef __EMSCRIPTEN__
inline constexpr bool kIsWeb = true;
inline constexpr bool kIsDesktop = false;
#else
inline constexpr bool kIsWeb = false;
inline constexpr bool kIsDesktop = true;
#endif

/// @}

/// @name OpenGL/GLSL Configuration
/// @{

#ifdef __EMSCRIPTEN__
/// GLSL version string for shader compilation and ImGui
inline constexpr const char* kGLSLVersionString = "#version 300 es";
/// Shader file suffix for platform-specific variants
inline constexpr const char* kShaderSuffix = "_es3";
#else
/// GLSL version string for shader compilation and ImGui
inline constexpr const char* kGLSLVersionString = "#version 460 core";
/// Shader file suffix for platform-specific variants
inline constexpr const char* kShaderSuffix = "_gl46";
#endif

/// @}

} // namespace vibegl
