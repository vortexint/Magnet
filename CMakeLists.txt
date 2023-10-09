cmake_minimum_required(VERSION 3.27)
project(Uniforge LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(SECURE_DIR ${ASSETS_DIR}/secure)

add_subdirectory(submodules/bullet3 SYSTEM)
add_subdirectory(submodules/flecs   SYSTEM)
add_subdirectory(submodules/glfw)
add_subdirectory(submodules/libarchive)
add_subdirectory(submodules/Lua)
add_subdirectory(submodules/tiny-AES-c)
add_subdirectory(submodules/tweeny)

option(STEAM_BUILD "Include Steam API in the final build" ON)

add_subdirectory(src)