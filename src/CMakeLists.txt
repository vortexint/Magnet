cmake_minimum_required(VERSION 3.27)

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
  gfx/glad.c
  gfx/shader.cpp
  io/input.cpp
  asset_manager.cpp
  game_state.cpp
)

#target_precompile_headers(${PROJECT_NAME} PRIVATE app.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
                      ${OpenGL_LIBRARIES}
                      glfw
                      archive_static
                      lua::static
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
                           ${CMAKE_SOURCE_DIR}/submodules/libarchive/libarchive
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
  COMMAND tar -cvf - ${ASSETS_DIR}/secure | lz4 > $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)

# decompress example:
# lz4 -d folderABC.tar.lz4 -c | tar xvf -