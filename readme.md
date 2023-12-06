
<h1 align="center">
<img src="magnet icon.png" alt="drawing" width="128"/><br>
Magnet<br>
<sup><sup><sub>A robust C++ framework for 3D software</sub></sup></sup></h2>

## Dependencies
**Linux**
<sup>

Choose the right one according to your system.

</sup>

| Manager| Packages |
| - | - |
| apt | cmake clang libgl1-mesa-dev x11proto-core-dev libx11-dev lz4 p7zip-full openexr |
| pacman | cmake clang mesa xorgproto libx11 lz4 p7zip openexr |
| dnf | clang mesa-libGL-devel libX11-devel lz4 p7zip openexr |
| zypper | cmake clang Mesa-libGL-devel libX11-devel lz4 p7zip openexr |

<sup>Note: some packages may already be shipped with your system</sup>

**Windows**
- [Git](https://git-scm.com/)
- [CMake](https://cmake.org/)
- [OpenEXR](https://openexr.com/en/latest/install.html)

```sh
git submodule update --init --recursive --progress
```

## Usage
Setup is CMake-oriented, Prefer an IDE that can manage CMake configuration automatically every time that a CMakeLists.txt file is saved such as:

- [VSCode](https://code.visualstudio.com/) + [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
- [Visual Studio](https://visualstudio.microsoft.com/)
- [CLion](https://www.jetbrains.com/clion/)

### Add Project
As this project extends beyond the scope of a single library, you will design your project within the framework itself.

Create your project directory in `projects/` and add it to `projects/CMakeLists.txt`
<sup>

*Alternatively, you can duplicate an existing project.*

</sup>

Afterward, you can work on your project's code structure.

### Implementing Logic

Next, register your Project Interface for init, update callback.
```cpp
#include <magnet/ApplicationContext.hpp>

// Note: the core's libraries are **SHARED**
// so you can use any of them to your project as needed.
#include <GLFW/glfw3.h>

class AppInterface : public ProjectInterface {
 public:
  void init() override {
    AssetManager*  assetMgr  = ApplicationContext::getAssetManager();
    WindowManager* windowMgr = ApplicationContext::getWindowManager();

    // Here you would use the AssetManager to load
    // essential assets of your project, including textures, sounds, etc.

    GLFWwindow* window = windowMgr->getWindow();
    // ...
  }

  void update() override {
    // Project-specific update logic
  }
};
```

### Content management
You can create a directory within your project's directory for assets to ship with the executable; By convention, "assets/" is used.  

The `core` directory includes default assets for rendering and scripting.

Always make sure your project's CMakeLists.txt calls `magnet_package()`, otherwise no assets will be packaged.

```cmake
# ...
magnet_package(${CMAKE_CURRENT_SOURCE_DIR}/assets) 

# if not using any asset, pass a directory that doesn't exist anywhere instead:
magnet_package(UNDEFINED)
```

## Learning Material

Seek the tutorials and samples in the [samples](samples/) directory.

<h2 align="center">Projects</h2>

### Sandbox <img src="projects/sandbox/assets/icon.png" alt="icon" width="128" height="128" align="left" valign="middle">
This is a sample project to demonstrate basic capabilities of the engine