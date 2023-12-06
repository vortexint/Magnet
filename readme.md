
<h1 align="center">
<img src="https://vortex-dev.com/assets/img/magnet.png" alt="3D horseshoe magnet icon" width="128"/><br>
Magnet<br>
<sup><sup><sub>A robust C++ framework for 3D software</sub></sup></sup></h2>

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

*Some packages may already be installed on your system.*

</sup>


**Windows**
- [Git](https://git-scm.com/)
- [CMake](https://cmake.org/)
- [OpenEXR](https://openexr.com/en/latest/install.html)

After getting the dependencies, run the commands:

```sh
git clone https://github.com/vortexdevsoftware/Magnet.git
cd Magnet
git submodule update --init --recursive --progress
```

## Usage
The setup is CMake-oriented. Prefer an IDE that manages CMake configuration automatically on save, such as::

- [VSCode](https://code.visualstudio.com/) + [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
- [Visual Studio](https://visualstudio.microsoft.com/)
- [CLion](https://www.jetbrains.com/clion/)

### Adding a Project
As this project extends beyond a single library, you design your project within the framework. Create a directory in `projects/` and add it to `projects/CMakeLists.txt`.

<sup>

*Alternatively, you can duplicate an existing project.*

</sup>

Next, let's work on the project's code structure...

### Implementing Logic

Register your Project Interface for init and update callbacks.
```cpp
#include <magnet/ApplicationContext.hpp>

// Note: the core's libraries are SHARED
// So you can use any of them as needed.
#include <GLFW/glfw3.h>

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

### Content management
You can create a directory within your project's directory for assets to ship with the executable; By convention, "assets/" is used.  

The `core` directory includes default assets for rendering and scripting.

Always make sure your project's CMakeLists.txt calls `magnet_package()`, otherwise no assets will be packaged.

```cmake
# ...
magnet_package(${CMAKE_CURRENT_SOURCE_DIR}/assets) 

# If not using any assets, pass a non-existent directory:
magnet_package(UNDEFINED)
```

## Learning Material

Seek the tutorials and samples in the [samples](samples/) directory.

<h2 align="center">Projects</h2>

### Sandbox <img src="samples/sandbox/assets/icon.png" alt="icon" width="128" height="128" align="left" valign="middle">
This is a sample project to demonstrate basic capabilities of the engine