# Test executable
add_executable(vibegl_tests test_main.cpp)

# Link libraries
target_link_libraries(vibegl_tests PRIVATE
    doctest::doctest
    glm::glm
)

# Mark GLM includes as SYSTEM to suppress warnings from third-party library
target_include_directories(vibegl_tests SYSTEM PRIVATE ${glm_SOURCE_DIR})

# Set compiler warnings
set_project_warnings(vibegl_tests)

# Force enable sanitizers for tests (unless coverage is enabled)
if(NOT ENABLE_COVERAGE)
    set(ENABLE_SANITIZERS ON CACHE BOOL "" FORCE)
    enable_sanitizers(vibegl_tests)
else()
    # Enable coverage for tests
    enable_coverage(vibegl_tests)
endif()

# Add tests to CTest
add_test(NAME vibegl_tests COMMAND vibegl_tests)

# Set output directory
set_target_properties(vibegl_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
