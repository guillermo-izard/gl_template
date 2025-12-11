cmake_minimum_required(VERSION 3.28)

project(VibeGL
    VERSION 0.1.0
    DESCRIPTION "Modern OpenGL Graphics Template"
    LANGUAGES CXX
)

# Suppress deprecation warnings from third-party dependencies
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.27")
    cmake_policy(SET CMP0148 OLD)  # Suppress FindPython deprecation
endif()
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "Suppress deprecation warnings" FORCE)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable Link-Time Optimization for Release builds
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
if(lto_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
else()
    message(STATUS "LTO not supported: ${lto_error}")
endif()

# Options
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)
option(BUILD_TESTS "Build unit tests" ON)

# Include CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)
include(Coverage)
include(Dependencies)

# Add subdirectories
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation target with hdoc
find_program(HDOC_EXECUTABLE hdoc)
if(HDOC_EXECUTABLE)
    add_custom_target(docs
        COMMAND ${HDOC_EXECUTABLE} --verbose
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with hdoc..."
        VERBATIM
    )
    set(HDOC_FOUND TRUE)
    message(STATUS "hdoc found: ${HDOC_EXECUTABLE}")
    message(STATUS "Run 'cmake --build . --target docs' to generate documentation")
else()
    set(HDOC_FOUND FALSE)
    message(STATUS "hdoc not found - documentation target not available")
    message(STATUS "Install hdoc from: https://github.com/hdoc/hdoc/releases")
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "VibeGL Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "  Coverage: ${ENABLE_COVERAGE}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  LTO (Release): ${lto_supported}")
message(STATUS "  Documentation (hdoc): ${HDOC_FOUND}")
message(STATUS "")
