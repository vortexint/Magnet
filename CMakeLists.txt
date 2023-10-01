cmake_minimum_required(VERSION 3.27)
project(EtherealReach LANGUAGES C CXX)

add_subdirectory(submodules/bullet3 SYSTEM)
add_subdirectory(submodules/flecs   SYSTEM)
add_subdirectory(submodules/glfw)
add_subdirectory(submodules/Lua)
add_subdirectory(submodules/tiny-AES-c)
add_subdirectory(submodules/tweeny)

set(CMAKE_CXX_STANDARD 23)

option(STEAM_BUILD "Include Steam API in the final build" ON)

add_subdirectory(src)