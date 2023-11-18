set(CMAKE_CXX_STANDARD 23)

add_library(magnetar-core STATIC)
add_library(magnetar::core ALIAS magnetar-core)

target_sources(magnetar-core PRIVATE
  ApplicationContext.cpp
  gfx/glad.c
  gfx/Renderer.cpp
  gfx/ShaderManager.cpp
  gfx/WindowManager.cpp
  io/AssetManager.cpp
)
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

target_include_directories(magnetar-core PRIVATE
                           ${OPENGL_INCLUDE_DIR}
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
                           ${CMAKE_SOURCE_DIR}/submodules/libarchive/libarchive
)

target_link_libraries(magnetar-core PUBLIC
                      ${OpenGL_LIBRARIES}
                      cglm
                      flecs_static
                      glfw
                      lua_static
                      spdlog
                      tiny-aes
                      zip
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

### Archive ###

function(magnetar_package)
  if(NOT DEFINED ASSETS_DIR)
    message(FATAL_ERROR "Please define ASSETS_DIR it before calling magnetar_package()")
  endif()

  string(TIMESTAMP TODAY "%Y%m%d") # Used as password for archive
  target_compile_definitions(magnetar-core PUBLIC SECURE_ASSETS_ARCHIVE="data.zip" SECURE_ASSETS_PASSWORD="${TODAY}")

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  # remove tempdata so we
  # 1. avoid shipping old assets
  # 2. avoid merging unrelated assets from different projects
  COMMAND rm -r ${CMAKE_BINARY_DIR}/tempdata || true

  COMMAND mkdir -p ${CMAKE_BINARY_DIR}/tempdata
  # merge core and project assets to new temp. dir
  COMMAND cp -a ${MAGNETAR_CORE_ASSETS}/. ${CMAKE_BINARY_DIR}/tempdata
  COMMAND cp -a ${ASSETS_DIR}/. ${CMAKE_BINARY_DIR}/tempdata

  # archive
  COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/tempdata
          zip --encrypt --password "${TODAY}" -r $<TARGET_FILE_DIR:${PROJECT_NAME}>/data .
  )
endfunction()