#include "ShaderManager.hpp"

#include <spdlog/spdlog.h>

#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

#include "../core/Platform.hpp"

namespace vibegl
{

/// Get error message for errno. Cross-platform helper to avoid MSVC warnings about strerror.
/// @param err errno value
/// @return Error message string
static std::string getErrorMessage(int err)
{
#ifdef _WIN32
    // MSVC: Use strerror_s for safer error message retrieval
    char buffer[256] = {0};
    strerror_s(buffer, sizeof(buffer), err);
    return buffer;
#else
    // POSIX: Use standard strerror
    return std::strerror(err);
#endif
}

Result<GLuint> ShaderManager::loadProgram(const std::string& baseName, const std::string& directory)
{
    std::string vertPath = directory + baseName + kShaderSuffix + ".vert";
    std::string fragPath = directory + baseName + kShaderSuffix + ".frag";
    return loadProgramFromFiles(vertPath, fragPath);
}

Result<GLuint> ShaderManager::loadProgramFromFiles(const std::string& vertPath,
                                                   const std::string& fragPath)
{
    auto vertSource = readFile(vertPath);
    if (!vertSource)
    {
        return std::unexpected(vertSource.error());
    }

    auto fragSource = readFile(fragPath);
    if (!fragSource)
    {
        return std::unexpected(fragSource.error());
    }

    auto vertShader = compileShader(GL_VERTEX_SHADER, vertSource.value());
    if (!vertShader)
    {
        return std::unexpected(vertShader.error());
    }

    auto fragShader = compileShader(GL_FRAGMENT_SHADER, fragSource.value());
    if (!fragShader)
    {
        glDeleteShader(vertShader.value());
        return std::unexpected(fragShader.error());
    }

    auto program = linkProgram(vertShader.value(), fragShader.value());

    // Shaders can be deleted after linking
    glDeleteShader(vertShader.value());
    glDeleteShader(fragShader.value());

    return program;
}

void ShaderManager::deleteProgram(GLuint program)
{
    if (program != 0)
    {
        glDeleteProgram(program);
    }
}

Result<std::string> ShaderManager::readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        int err = errno;
        return std::unexpected(Error{.message = "Failed to open shader file",
                                     .context = path + " (errno: " + std::to_string(err) + " - " +
                                                getErrorMessage(err) + ")"});
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

Result<GLuint> ShaderManager::compileShader(GLenum type, const std::string& source)
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

        const char* typeName = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";

        glDeleteShader(shader);
        return std::unexpected(
            Error{.message = std::string(typeName) + " shader compilation failed",
                  .context = std::string(errorLog.data())});
    }

    return shader;
}

Result<GLuint> ShaderManager::linkProgram(GLuint vertShader, GLuint fragShader)
{
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

        glDeleteProgram(program);
        return std::unexpected(Error{.message = "Shader program linking failed",
                                     .context = std::string(errorLog.data())});
    }

    return program;
}

} // namespace vibegl
