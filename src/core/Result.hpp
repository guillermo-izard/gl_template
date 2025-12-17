#pragma once

/// @file
/// Result type for error handling using C++23 std::expected.

#include <expected>
#include <string>

// Feature detection for std::expected availability
#ifndef __cpp_lib_expected
#error "std::expected is not available in this standard library implementation. " \
        "This typically occurs with Clang + libstdc++ (GCC's standard library). " \
        "Supported compilers/configurations: " \
        "GCC 13+, Clang 16+ with libc++, or MSVC 2022 17.4+. " \
        "On Linux with Clang, consider using libc++ or GCC instead."
#endif

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
