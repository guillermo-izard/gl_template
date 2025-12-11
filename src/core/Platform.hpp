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
/// @brief True when compiling for WebAssembly/Emscripten
inline constexpr bool kIsWeb = true;
/// @brief True when compiling for desktop platforms (Windows/Linux/macOS)
inline constexpr bool kIsDesktop = false;
#else
/// @brief True when compiling for WebAssembly/Emscripten
inline constexpr bool kIsWeb = false;
/// @brief True when compiling for desktop platforms (Windows/Linux/macOS)
inline constexpr bool kIsDesktop = true;
#endif

/// @}

/// @name OpenGL/GLSL Configuration
/// @{

#ifdef __EMSCRIPTEN__
/// @brief GLSL version string for shader compilation and ImGui (WebGL 2.0 / ES 3.0)
inline constexpr const char* kGLSLVersionString = "#version 300 es";
/// @brief Shader file suffix for platform-specific variants (_es3 for web)
inline constexpr const char* kShaderSuffix = "_es3";
#else
/// @brief GLSL version string for shader compilation and ImGui (OpenGL 4.6 Core)
inline constexpr const char* kGLSLVersionString = "#version 460 core";
/// @brief Shader file suffix for platform-specific variants (_gl46 for desktop)
inline constexpr const char* kShaderSuffix = "_gl46";
#endif

/// @}

} // namespace vibegl
