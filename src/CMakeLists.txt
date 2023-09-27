cmake_minimum_required(VERSION 3.27)

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
  gfx/shader.cpp
  io/input.cpp
  glad.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
                      ${OpenGL_LIBRARIES}
                      glfw
                      lua::static
                      tweeny
                      )
                      
target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_SOURCE_DIR}/include
                           PUBLIC
                           ${OPENGL_INCLUDE_DIR}
                           )