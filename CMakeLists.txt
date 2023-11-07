cmake_minimum_required(VERSION 3.27)
project(Magnetar LANGUAGES C CXX)

add_subdirectory(submodules)

option(STEAM_BUILD "Include Steam API in the final build" ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

### Magnetar Framework ###

add_subdirectory(core) # magnetar::core
set(MAGNETAR_CORE_ASSETS ${CMAKE_SOURCE_DIR}/core/assets) # workaround

add_subdirectory(projects)