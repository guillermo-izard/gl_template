include(FetchContent)

# Set FetchContent options
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# ============================================================================
# GLFW - Windowing and input
# ============================================================================
if(NOT EMSCRIPTEN)
    message(STATUS "Fetching GLFW...")
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
        GIT_SHALLOW    TRUE
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
else()
    # Emscripten provides GLFW via -sUSE_GLFW=3
    message(STATUS "Using Emscripten's built-in GLFW")
    add_library(glfw INTERFACE)
endif()

# ============================================================================
# glad - OpenGL loader (not needed for Emscripten)
# ============================================================================
if(NOT EMSCRIPTEN)
    message(STATUS "Fetching glad...")
    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG        v0.1.36
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(glad)
else()
    # Emscripten provides OpenGL ES directly
    message(STATUS "Using Emscripten's built-in OpenGL ES")
    add_library(glad INTERFACE)
endif()

# ============================================================================
# GLM - Mathematics library
# ============================================================================
message(STATUS "Fetching GLM...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)
set(GLM_ENABLE_CXX_23 ON CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

# Mark GLM as SYSTEM to suppress warnings (glm::glm is an alias to glm)
get_target_property(GLM_IID glm INTERFACE_INCLUDE_DIRECTORIES)
set_target_properties(glm PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${GLM_IID}"
)

# ============================================================================
# Dear ImGui - UI library (docking branch)
# ============================================================================
message(STATUS "Fetching Dear ImGui (docking branch)...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

# Create ImGui library target with OpenGL3 and GLFW backends
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui SYSTEM PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
if(NOT EMSCRIPTEN)
    target_link_libraries(imgui PUBLIC glfw glad)
else()
    # Emscripten: link only interface targets
    target_link_libraries(imgui PUBLIC glfw)
endif()

# ============================================================================
# fmt - Formatting library (required for spdlog with Emscripten)
# ============================================================================
if(EMSCRIPTEN)
    message(STATUS "Fetching fmt (external for Emscripten)...")
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        11.1.4
        GIT_SHALLOW    TRUE
    )
    set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(fmt)
    set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
endif()

# ============================================================================
# spdlog - Fast logging library
# ============================================================================
message(STATUS "Fetching spdlog...")
if(EMSCRIPTEN)
    # Use spdlog 1.15.1 for Emscripten (proper fmt 11 support)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.15.1
        GIT_SHALLOW    TRUE
    )
else()
    # Use spdlog 1.14.1 for native builds (bundled fmt)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.14.1
        GIT_SHALLOW    TRUE
    )
endif()
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# ============================================================================
# stb_image - Image loading library (header-only)
# ============================================================================
message(STATUS "Fetching stb...")
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        f1c79c02822848a9bed4315b12c8c8f3761e1296  # 2025-01-08
)
FetchContent_MakeAvailable(stb)

# Create interface library for stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE ${stb_SOURCE_DIR})

# ============================================================================
# doctest - Testing framework
# ============================================================================
if(BUILD_TESTS)
    message(STATUS "Fetching doctest...")
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.11
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(doctest)
endif()

message(STATUS "All dependencies fetched successfully")
