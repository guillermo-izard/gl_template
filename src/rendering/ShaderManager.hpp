#pragma once

/// @file ShaderManager.hpp
/// @brief Shader loading and compilation utilities

#include "../core/GLIncludes.hpp"
#include <string>

namespace vibegl {

/// @brief Utilities for loading and compiling OpenGL shaders
///
/// ShaderManager handles platform-specific shader variants automatically.
/// When loading a shader by base name, it appends the appropriate suffix
/// (_gl46 for desktop, _es3 for web) based on the current platform.
///
/// Example:
/// @code
/// // Loads "shaders/cube_gl46.vert" on desktop, "shaders/cube_es3.vert" on web
/// GLuint program = ShaderManager::loadProgram("cube");
/// @endcode
class ShaderManager {
public:
    /// @brief Load shader program with automatic platform suffix
    /// @param baseName Base name without suffix (e.g., "cube" loads cube_gl46 or cube_es3)
    /// @param directory Directory containing shaders (default: "shaders/")
    /// @return OpenGL program ID, or 0 on failure
    static GLuint loadProgram(const std::string& baseName, const std::string& directory = "shaders/");

    /// @brief Load shader program from explicit file paths
    /// @param vertPath Path to vertex shader file
    /// @param fragPath Path to fragment shader file
    /// @return OpenGL program ID, or 0 on failure
    static GLuint loadProgramFromFiles(const std::string& vertPath, const std::string& fragPath);

    /// @brief Delete a shader program
    /// @param program OpenGL program ID to delete
    static void deleteProgram(GLuint program);

private:
    /// @brief Read entire file contents into string
    /// @param path Path to file to read
    /// @returns File contents as string
    static std::string readFile(const std::string& path);

    /// @brief Compile a shader from source
    /// @param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
    /// @param source GLSL source code
    /// @returns Shader ID, or 0 on failure
    static GLuint compileShader(GLenum type, const std::string& source);

    /// @brief Link vertex and fragment shaders into program
    /// @param vertShader Compiled vertex shader ID
    /// @param fragShader Compiled fragment shader ID
    /// @returns Program ID, or 0 on failure
    static GLuint linkProgram(GLuint vertShader, GLuint fragShader);
};

} // namespace vibegl
