cmake_minimum_required(VERSION 3.27)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
                      ${SDL2_LIBRARIES}
                      ${OpenGL_LIBRARIES}
                      lua::static)
                      
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS})