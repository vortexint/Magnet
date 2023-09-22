cmake_minimum_required(VERSION 3.27)
project(EtherealReach LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME} main.cpp)
