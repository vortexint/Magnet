cmake_minimum_required(VERSION 3.27)
project(Magnet LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(submodules)

option(STEAM_BUILD "Include Steam API in the final build" ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

### Magnet Framework ###

add_subdirectory(core) # magnet::core

add_subdirectory(samples)