#include "ShaderManager.hpp"

#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>
#include <vector>

#include "../core/Platform.hpp"

namespace vibegl
{

GLuint ShaderManager::loadProgram(const std::string& baseName, const std::string& directory)
{
    std::string vertPath = directory + baseName + kShaderSuffix + ".vert";
    std::string fragPath = directory + baseName + kShaderSuffix + ".frag";
    return loadProgramFromFiles(vertPath, fragPath);
}

GLuint ShaderManager::loadProgramFromFiles(const std::string& vertPath, const std::string& fragPath)
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

    GLuint program = linkProgram(vertShader, fragShader);

    // Shaders can be deleted after linking
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

void ShaderManager::deleteProgram(GLuint program)
{
    if (program != 0)
    {
        glDeleteProgram(program);
    }
}

std::string ShaderManager::readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        spdlog::error("Failed to open shader file: {}", path);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint ShaderManager::compileShader(GLenum type, const std::string& source)
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
        spdlog::error("{} shader compilation failed: {}", typeName, errorLog.data());

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint ShaderManager::linkProgram(GLuint vertShader, GLuint fragShader)
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

        spdlog::error("Shader program linking failed: {}", errorLog.data());

        glDeleteProgram(program);
        return 0;
    }

    return program;
}

} // namespace vibegl
