# VibeGL executable
add_executable(vibegl
    main.cpp
    VibeGLApp.cpp
    core/Application.cpp
    rendering/ShaderManager.cpp
    rendering/TextureLoader.cpp
    rendering/StbImage.cpp
)

# Link libraries
target_link_libraries(vibegl PRIVATE
    glfw
    glad
    glm::glm
    imgui
    spdlog::spdlog
    stb_image
)

# Include directories for project headers
target_include_directories(vibegl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Mark GLM includes as SYSTEM to suppress warnings from third-party library
target_include_directories(vibegl SYSTEM PRIVATE ${glm_SOURCE_DIR})

# Set compiler warnings
set_project_warnings(vibegl)

# Enable sanitizers if requested
enable_sanitizers(vibegl)

# Platform-specific settings
if(EMSCRIPTEN)
    # Emscripten-specific configuration
    set_target_properties(vibegl PROPERTIES
        SUFFIX ".html"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    target_link_options(vibegl PRIVATE
        -sUSE_GLFW=3
        -sFULL_ES3=1
        -sALLOW_MEMORY_GROWTH=1
        -sINITIAL_MEMORY=64MB
        --preload-file ${CMAKE_SOURCE_DIR}/data@/data
    )

    target_compile_definitions(vibegl PRIVATE
        GLFW_INCLUDE_ES3
    )
elseif(UNIX AND NOT APPLE)
    # Link required libraries on Linux
    find_package(X11 REQUIRED)
    target_link_libraries(vibegl PRIVATE
        ${CMAKE_DL_LIBS}
        pthread
        X11::X11
    )
    # Set output directory
    set_target_properties(vibegl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
else()
    # Set output directory for other platforms
    set_target_properties(vibegl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()
