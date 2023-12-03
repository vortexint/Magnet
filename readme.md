# Magnet
A framework to simplify C++ game development

## Setup
|     | Packages                                                                        |
| --- | ------------------------------------------------------------------------------- |
| apt | cmake clang libgl1-mesa-dev x11proto-core-dev libx11-dev lz4 p7zip-full openexr |

```sh
git submodule update --init --recursive --progress
```

## Usage

### Project Setup

As this project extends beyond the scope of a single library, you will design your project within the framework itself.

#### 1. Create your project directory in `projects/` and add it to `projects/CMakeLists.txt`
<sup>

*Alternatively, you can duplicate an existing project.*

</sup>

Afterward, you can work on your project's code structure.

#### 2. Include the necessary core headers

```cpp
#include <magnet/ApplicationContext.hpp>
// ...
```

Note that the core's libraries are **SHARED** so you can simply add any of them to your project as needed.
```cpp
#include <GLFW/glfw3.h>

// ...
GLFWwindow* window = ApplicationContext::getWindowManager()->getWindow();
```

### Content management
By convention, the contents of the project's `assets/` are files to be packaged with the game to an archive that is loaded in run-time

The `core` directory includes default assets for rendering and scripting.

<h2 align="center">Projects</h2>

### Sandbox <img src="projects/sandbox/assets/icon.png" alt="icon" width="128" height="128" align="left" valign="middle">
This is a sample project to demonstrate basic capabilities of the engine