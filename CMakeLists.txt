cmake_minimum_required(VERSION 3.27)

add_subdirectory(submodules/bullet3)
add_subdirectory(submodules/flecs)
add_subdirectory(submodules/glfw)
add_subdirectory(submodules/glm)
add_subdirectory(submodules/libarchive)
add_subdirectory(submodules/Lua)
add_subdirectory(submodules/tiny-AES-c)
add_subdirectory(submodules/tweeny)

option(STEAM_BUILD "Include Steam API in the final build" ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(core) # magnetar::core

add_subdirectory(projects)