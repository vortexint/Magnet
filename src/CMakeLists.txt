cmake_minimum_required(VERSION 3.27)

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
  gfx/shader.cpp
  io/input.cpp
  glad.c
)

target_precompile_headers(${PROJECT_NAME} PRIVATE app.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
                      ${OpenGL_LIBRARIES}
                      glfw
                      lua::static
                      miniz
                      tiny-aes
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
    -O2
    # Optimize for Size
    -Os
    # Strip Debug Information
    -s
    # Dead Code Elimination
    -fdata-sections
    -Wl,--gc-sections
    -fomit-frame-pointer
    -mcmodel=small
)

# Copy assets to the output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${ASSETS_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Generate a random hexadecimal string using OpenSSL
# NOTE: This isn't ideal, AFAIK this only runs every time the CMake is configured
# But it's a fair number of times
execute_process(
  COMMAND openssl rand -hex 16
  OUTPUT_VARIABLE AES_ASSETS_KEY
  RESULT_VARIABLE RAND_RESULT
  ERROR_VARIABLE RAND_ERROR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT RAND_RESULT EQUAL 0)
  message(FATAL_ERROR "Failed to generate a random key: ${RAND_ERROR}")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_ASSETS_AES_KEY="${AES_ASSETS_KEY}")

message("AES Assets Key: ${AES_ASSETS_KEY}")