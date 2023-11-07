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
    # optimize for Size
    -Os
    # dead code elimination
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
                      cglm
                      archive_static
                      mustache
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

### Utilities ###

function(magnetar_package)
  if(NOT DEFINED ASSETS_DIR)
    message(FATAL_ERROR "Please define ASSETS_DIR it before calling magnetar_package()")
  endif()

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # it's best to remove tempdata to avoid shipping old assets
    COMMAND rm -r ${CMAKE_BINARY_DIR}/tempdata || true

    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/tempdata
    # merge core and project assets to new temp. dir
    COMMAND cp -a ${MAGNETAR_CORE_ASSETS}/. ${CMAKE_BINARY_DIR}/tempdata
    COMMAND cp -a ${ASSETS_DIR}/. ${CMAKE_BINARY_DIR}/tempdata

    COMMAND tar -C ${CMAKE_BINARY_DIR}/tempdata -cvf - . | lz4 > $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
  )

  # decompress example:
  # lz4 -d folderABC.tar.lz4 -c | tar xvf -
endfunction()