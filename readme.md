# Magnetar

## Setup
| Packages |
|-|
| cmake clang lz4 p7zip-full |

```sh
git submodule update --init --recursive --progress
```

## Content management
By convention, the contents of the project's `assets/` are files to be packaged with the game to an archive that is loaded in run-time

The `core` includes default assets for rendering and scripting.

## Projects

### Sandbox
This is a sample project to demonstrate basic capabilities of the engine

<!--
 Developer notes:

 - headers only include what they need
 - source files should use stdafx.hpp before including headers
-->