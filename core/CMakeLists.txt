project(Magnetar LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)

add_library(magnetar-core STATIC)
add_library(magnetar::core ALIAS magnetar-core)

target_sources(magnetar-core PRIVATE
# GFX
  gfx/glad.c
  gfx/renderer.cpp
  gfx/shader_manager.cpp
  gfx/window.cpp
# I/O
  io/asset_manager.cpp
  io/input.cpp
)

#target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.hpp)

target_compile_options(magnetar-core PUBLIC
    -O2
    # Optimize for Size
    -Os
    # Dead Code Elimination
    -fdata-sections
    -Wl,--gc-sections
    -fomit-frame-pointer
    -mcmodel=small
)

find_package(OpenGL REQUIRED)

target_include_directories(magnetar-core PUBLIC
                           # libraries
                           ${CMAKE_SOURCE_DIR}/submodules/libarchive/libarchive
                           ${OPENGL_INCLUDE_DIR}
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(magnetar-core PUBLIC
                      ${OpenGL_LIBRARIES}
                      glfw
                      archive_static
                      flecs_static
                      lua_static
                      tiny-aes
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(magnetar-core PRIVATE ${CMAKE_SOURCE_DIR}/lib/steam_api64.lib)
  message("Windows was detected, linked steam_api64.lib")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(magnetar-core PRIVATE ${CMAKE_SOURCE_DIR}/lib/libsteam_api.so)
  message("Linux was detected, linked libsteam_api.so")
else()
  message(FATAL_ERROR "Unknown platform: \"" ${CMAKE_SYSTEM_NAME} "\"")
endif()