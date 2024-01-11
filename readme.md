
<h1 align="center">
<img src="https://vortex-dev.com/assets/img/magnet.png" alt="3D horseshoe magnet icon" width="128"/><br>
Magnet<br>
<sup><sup><sub>A robust C++ framework for 3D software</sub></sup></sup></h1>

**[Official webpage](https://vortex-dev.com/?p=magnet) · [License](license.md)**

## Dependencies
**Linux**

<sup>

Choose the appropriate package manager for your system.

</sup>

| Manager| Packages |
| - | - |
| apt | cmake clang libgl1-mesa-dev x11proto-core-dev libx11-dev lz4 p7zip-full openexr |
| pacman | cmake clang mesa xorgproto libx11 lz4 p7zip openexr |
| dnf | clang mesa-libGL-devel libX11-devel lz4 p7zip openexr |
| zypper | cmake clang Mesa-libGL-devel libX11-devel lz4 p7zip openexr |

<sup>

*Most packages are likely already installed on your system.*

</sup>


**Windows**
- [CMake](https://cmake.org/)
- [Git](https://git-scm.com/)
- [OpenEXR](https://openexr.com/en/latest/install.html)
- [Python](https://www.python.org/downloads/)

## Usage
This project uses CMake as it's build system, prefer an IDE that manages CMake configuration automatically on save, such as:

- [VSCode](https://code.visualstudio.com/) + [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
- [Visual Studio](https://visualstudio.microsoft.com/)
- [CLion](https://www.jetbrains.com/clion/)

### Setup example
<sup>

Assuming your project uses CMake & Git and you have all the dependencies.

</sup>
in your working directory, run:

```sh
mkdir submodules
cd submodules
git submodule add https://github.com/vortexdevsoftware/Magnet.git
git submodule update --init --recursive --progress
```

#### Configure CMakeLists.txt

You can create a directory within your project's directory for assets to ship with the executable; By convention, "assets/" is used.  

The `core` directory includes default assets for rendering and scripting.

Always make sure your project's CMakeLists.txt calls `magnet_package()`, otherwise no assets will be packaged.

```cmake
# ...
target_link_libraries(myApp PRIVATE magnet-core)

magnet_package(${CMAKE_CURRENT_SOURCE_DIR}/assets) 

# Necessary. If not using any custom assets, pass an empty string.
magnet_package("")
```

Next, let's work on the project's code structure...

#### Project Interface

```cpp
#include <magnet/ApplicationContext.hpp>

// Note: the core's libraries are SHARED
// So you can use any of them as needed.
#include <GLFW/glfw3.h>

// Register Interface for init and update callbacks
class AppInterface : public ProjectInterface {
 public:
  void init() override {
    AssetManager*  assetMgr  = ApplicationContext::getAssetManager();
    WindowManager* windowMgr = ApplicationContext::getWindowManager();

    GLFWwindow* window = windowMgr->getWindow();

    // Use AssetManager to load essential assets, etc...
  }

  void update() override {
    // Project-specific update logic
  }
};
```

## Learning Material

There are several [samples](samples/) to learn from:

| Tutorial | Description |
|-|-|
|[Barebones](samples/00_barebones/)| As raw as it gets, establishes a minimal project |
|[Primitives](samples/01_primitives/)| Use native features and prefabs to populate the scene |

| Project | Description |
|-|-|
|[Sandbox](samples/sandbox/)| Garry's Mod clone |
