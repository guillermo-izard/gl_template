#pragma once

/// @file
/// Texture loading utilities using stb_image.

#include "../core/GLIncludes.hpp"
#include <string>

namespace vibegl {

/// Utilities for loading textures from image files.
///
/// TextureLoader uses stb_image to load various image formats (PNG, JPEG, etc.)
/// and creates OpenGL textures with appropriate settings.
class TextureLoader {
public:
    /// Load a texture from an image file.
    /// @param filepath Path to the image file
    /// @param flipVertically Whether to flip the image vertically (default: true)
    /// @return OpenGL texture ID, or 0 on failure
    static GLuint loadTexture(const std::string& filepath, bool flipVertically = true);

    /// Delete a texture.
    /// @param texture OpenGL texture ID to delete
    static void deleteTexture(GLuint texture);
};

} // namespace vibegl
