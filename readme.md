
<h1 align="center">
<img src="https://github.com/vortexdevsoftware/Magnet/assets/18470725/add58edd-4713-4795-a4cb-bd7e59c8e96e" alt="3D horseshoe magnet icon" width="128"/><br>
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

### Setup
<sup>

Assuming you have CMake & Git...

</sup>
in your working directory, run:

```bash
git clone https://github.com/vortexdevsoftware/Magnet.git

# Or if a submodule is preferred:

git submodule add https://github.com/vortexdevsoftware/Magnet.git
git submodule update --init --recursive --progress
```
After this, you should be ready to configure the project with CMake...

## Learning Material

There are several [samples](samples/) to learn from:

| Project | Description |
|-|-|
|[Barebones](samples/00_barebones/)| As raw as it gets, establishes a minimal project |
|[Primitives](samples/01_primitives/)| Populating the scenes with simple models |
|[Native UI](samples/02_native_ui/)| Using Magnet's native UI |
|[Sandbox](samples/sandbox/)| Physics sandbox |
