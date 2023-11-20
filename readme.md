# Magnet

## Setup
| Packages |
|-|
| cmake clang libgl1-mesa-dev x11proto-core-dev libx11-dev lz4 p7zip-full openexr |

```sh
git submodule update --init --recursive --progress
```

## Content management
By convention, the contents of the project's `assets/` are files to be packaged with the game to an archive that is loaded in run-time

The `core` includes default assets for rendering and scripting.

<h2 align="center">Projects</h2>

### Sandbox <img src="projects/sandbox/assets/icon.png" alt="icon" width="128" height="128" align="left" valign="middle">
This is a sample project to demonstrate basic capabilities of the engine

---

<!--
 Developer notes:

 - headers only include what they need
 - source files should use stdafx.hpp before including headers
-->