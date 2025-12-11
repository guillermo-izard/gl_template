#include "TextureLoader.hpp"

#include <spdlog/spdlog.h>

#include <stb_image.h>

namespace vibegl
{

GLuint TextureLoader::loadTexture(const std::string& filepath, bool flipVertically)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);
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

void TextureLoader::deleteTexture(GLuint texture)
{
    if (texture != 0)
    {
        glDeleteTextures(1, &texture);
    }
}

} // namespace vibegl
