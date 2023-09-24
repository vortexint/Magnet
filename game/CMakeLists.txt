cmake_minimum_required(VERSION 3.27)

add_executable(${PROJECT_NAME} main.cpp)

find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
                      SDL3::SDL3-static
                      lua::static)
                      
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)