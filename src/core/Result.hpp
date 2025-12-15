#pragma once

/// @file
/// Result type for error handling using C++23 std::expected.

#include <expected>
#include <string>

namespace vibegl {

/// Error information for fallible operations.
///
/// Provides contextual information about what went wrong and where.
struct Error {
    /// Human-readable error message
    std::string message;

    /// Additional context (e.g., file path, shader name, errno details)
    std::string context;
};

/// Result type for operations that can fail.
///
/// Uses C++23 std::expected to provide type-safe error handling.
/// Template users should use this pattern for fallible operations.
///
/// Example:
/// ```cpp
/// Result<GLuint> shader = ShaderManager::loadProgram("cube");
/// if (!shader) {
///     spdlog::error("Shader failed: {} ({})", shader.error().message, shader.error().context);
///     return;
/// }
/// GLuint program = shader.value();
/// ```
template<typename T>
using Result = std::expected<T, Error>;

} // namespace vibegl
