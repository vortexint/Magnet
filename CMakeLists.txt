cmake_minimum_required(VERSION 3.27)
project(Magnetar LANGUAGES C CXX)

add_subdirectory(submodules)

option(STEAM_BUILD "Include Steam API in the final build" ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

### Magnetar Framework ###

set(MAGNETAR_CORE_ASSETS ${CMAKE_SOURCE_DIR}/core/assets)

add_subdirectory(core) # magnetar::core

# Projects using the Magnetar framework
add_subdirectory(projects)