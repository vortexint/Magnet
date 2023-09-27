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

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/steam_api64.lib)
  message("Windows was detected, linked steam_api64.lib")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/libsteam_api.so)
  message("Linux was detected, linked libsteam_api.so")
else()
  message(FATAL_ERROR "Unknown platform: \"" ${CMAKE_SYSTEM_NAME} "\"")
endif()
                      
target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_SOURCE_DIR}/include
                           ${OPENGL_INCLUDE_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    # Optimization Level
    -O2

    # Optimize for Size
    -Os

    # Strip Debug Information
    -s

    # Dead Code Elimination
    -fdata-sections
    -Wl,--gc-sections

    # Minimize Code Generation
    -fomit-frame-pointer

    # Reduce Code Model
    -mcmodel=small
)