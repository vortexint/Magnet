cmake_minimum_required(VERSION 3.27)

add_subdirectory(submodules)

option(STEAM_BUILD "Include Steam API in the final build" ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(core) # magnetar::core
# Projects using the Magnetar framework
add_subdirectory(projects)