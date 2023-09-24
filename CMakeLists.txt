cmake_minimum_required(VERSION 3.27)
project(EtherealReach LANGUAGES C CXX)

add_subdirectory(submodules/bullet3 SYSTEM)
add_subdirectory(submodules/SDL)
add_subdirectory(submodules/Lua)

#

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(game)