# VibeGL executable
add_executable(vibegl main.cpp)

# Link libraries
target_link_libraries(vibegl PRIVATE
    glfw
    glad
    glm::glm
    imgui
    spdlog::spdlog
    stb_image
)

# Mark GLM includes as SYSTEM to suppress warnings from third-party library
target_include_directories(vibegl SYSTEM PRIVATE ${glm_SOURCE_DIR})

# Set compiler warnings
set_project_warnings(vibegl)

# Enable sanitizers if requested
enable_sanitizers(vibegl)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    # Link required libraries on Linux
    find_package(X11 REQUIRED)
    target_link_libraries(vibegl PRIVATE
        ${CMAKE_DL_LIBS}
        pthread
        X11::X11
    )
endif()

# Set output directory
set_target_properties(vibegl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
