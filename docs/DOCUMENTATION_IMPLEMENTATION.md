# API Documentation Implementation

## Overview

This document describes the implementation of automatic API documentation generation for the VibeGL project using Doxygen.

## Implementation Summary

**Goal:** Set up automatic C++ API documentation generation with GitHub Pages deployment.

**Solution:** Doxygen with GitHub Actions automation.

**Live Documentation:** https://guillermo-izard.github.io/gl_template/docs/

## What Was Implemented

### 1. Documentation Generator: Doxygen

**Why Doxygen:**
- Industry-standard C++ documentation tool (25+ years of development)
- Direct HTML generation (no intermediate steps)
- Excellent C++23 support
- Widely available in all package managers
- Extensive feature set and community support

**Note:** Initially attempted to use Doxide (modern alternative), but version 0.9.0 had a critical bug causing it to generate zero output despite correct configuration. Switched to Doxygen for reliability.

### 2. Configuration Files

**Doxyfile** - Main Doxygen configuration:
- Project name: VibeGL
- Version: 0.1.0
- Input: `src/` directory (recursive)
- Output: `docs/` directory
- Extracts documentation from `.hpp` and `.cpp` files
- Excludes third-party code (`StbImage.cpp`)
- Generates HTML with tree view and search functionality

### 3. Build System Integration

**CMakeLists.txt:**
- Added `docs` target using Doxygen
- Auto-detects if Doxygen is installed
- Usage: `cmake --build build/debug --target docs`

**GitHub Actions Workflow (.github/workflows/deploy-pages.yml):**
- Installs Doxygen on Ubuntu 24.04
- Generates documentation automatically on every push to master
- Combines documentation with WebAssembly demo in single deployment
- Documentation served at `/docs/` subdirectory
- Demo served at root

### 4. Documentation Standard

**Comment Style:**
```cpp
/// Brief description of the class/function.
///
/// Detailed description with more information.
///
/// @param paramName Description of the parameter
/// @return Description of return value
```

**Example from Application.hpp:**
```cpp
/// Base class for applications with platform-abstracted main loop.
///
/// Derive from this class and implement onInit(), onTick(), and onShutdown().
/// The run() method handles the platform-specific main loop:
/// - Desktop: Traditional while loop
/// - Web: emscripten_set_main_loop callback
class Application {
    /// Called every frame with delta time in seconds.
    /// @param deltaTime Time elapsed since last frame
    virtual void onTick(float deltaTime) = 0;
};
```

### 5. Documented Components

All major components now have complete API documentation:

- **Core Classes:**
  - `vibegl::Application` - Application framework
  - `vibegl::WindowConfig` - Window configuration
  - `vibegl::Platform` - Platform detection constants

- **Rendering Classes:**
  - `vibegl::ShaderManager` - Shader loading and compilation
  - `vibegl::TextureLoader` - Texture loading from images

- **Demo:**
  - `vibegl::VibeGLApp` - Example application

## Usage

### Viewing Documentation

**Online:** https://guillermo-izard.github.io/gl_template/docs/

The documentation is automatically generated and deployed on every push to master.

### Generating Documentation Locally

**Prerequisites:**
```bash
# Ubuntu/Debian
sudo apt-get install doxygen

# macOS
brew install doxygen
```

**Generate:**
```bash
# Direct Doxygen invocation
doxygen Doxyfile

# Or via CMake
cmake --build build/debug --target docs

# Open the documentation
xdg-open docs/index.html  # Linux
open docs/index.html      # macOS
```

### Updating Documentation

Documentation comments use standard Doxygen format:

1. Add `///` comments above classes, functions, and members
2. Use `@param` for parameters, `@return` for return values
3. First sentence becomes the brief description
4. Additional paragraphs provide detailed description
5. Documentation auto-generates on push to master

## Features

- ✅ **Automatic Generation:** Runs on every push to master via GitHub Actions
- ✅ **Class Hierarchy:** Shows inheritance relationships
- ✅ **Function Documentation:** Parameters, return values, descriptions
- ✅ **Source Browser:** Links to source code with line numbers
- ✅ **Search Functionality:** Fast client-side search
- ✅ **Tree Navigation:** Organized file and class structure
- ✅ **Modern UI:** Clean, responsive design
- ✅ **Zero Maintenance:** No manual updates required

## Files Modified/Created

### Created:
- `Doxyfile` - Doxygen configuration

### Modified:
- `.github/workflows/deploy-pages.yml` - Added Doxygen build step
- `CMakeLists.txt` - Added `docs` target
- `README.md` - Updated with Doxygen instructions and feature list
- All header files in `src/` - Enhanced documentation comments

### Removed (from previous Doxide attempt):
- `doxide.yaml`
- `mkdocs.yml`
- `src/TestDoc.hpp`

## Maintenance

Documentation is **maintenance-free** once set up:

1. Write documentation comments in code
2. Push to master
3. GitHub Actions automatically generates and deploys

No manual documentation updates needed - it stays in sync with code automatically.

## Technical Details

**Build Time:** ~5 seconds for full documentation generation

**Output Size:** ~100 HTML files (one per class/file/namespace)

**Deployment:** Combined with WebAssembly demo in unified GitHub Pages deployment

**Cache:** None required - full rebuild on each push is fast

## Future Enhancements (Optional)

Possible improvements if needed in the future:

- Add Graphviz for class diagrams (set `HAVE_DOT = YES`)
- Add call/caller graphs
- Add example code snippets with `@code` blocks
- Add `@see` cross-references between related classes
- Add custom CSS for additional styling

## Conclusion

The VibeGL project now has comprehensive, automatically-generated API documentation that:
- Provides complete reference for all public APIs
- Stays synchronized with code automatically
- Deploys alongside the WebAssembly demo
- Requires zero maintenance effort

Documentation is accessible at: https://guillermo-izard.github.io/gl_template/docs/
