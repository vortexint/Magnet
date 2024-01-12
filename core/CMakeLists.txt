set(CMAKE_CXX_STANDARD 23)

add_library(magnet-core STATIC)
add_library(magnet::core ALIAS magnet-core)

target_sources(magnet-core PRIVATE
  ApplicationContext.cpp
  
  IO/AssetManager.cpp
  IO/Input.cpp
)

add_subdirectory(gfx)
if (MSVC)
    # These optimizations cause issues in msvc
else()
    target_compile_options(magnet-core PUBLIC
        -O2
        # optimize for Size
        -Os
        # dead code elimination
        -fdata-sections
        -Wl,--gc-sections
        -fomit-frame-pointer
        -mcmodel=small
    )
endif()

find_package(OpenGL REQUIRED)

target_include_directories(magnet-core PRIVATE
                           ${OPENGL_INCLUDE_DIR}
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
                           ${CMAKE_SOURCE_DIR}/submodules/libarchive/libarchive
)

target_link_libraries(magnet-core PUBLIC
                      ${OpenGL_LIBRARIES}
                      archive_static
                      cglm
                      flecs_static
                      glfw
                      lua_static
                      spdlog
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(magnet-core PRIVATE ${CMAKE_SOURCE_DIR}/lib/steam_api64.lib)
  message("Windows was detected, linked steam_api64.lib")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(magnet-core PRIVATE ${CMAKE_SOURCE_DIR}/lib/libsteam_api.so)
  message("Linux was detected, linked libsteam_api.so")
else()
  message(FATAL_ERROR "Unknown platform: \"" ${CMAKE_SYSTEM_NAME} "\"")
endif()

# Asset Packaging 
#
# Call this function with the following arguments:
# - ASSET_DIR: The directory to be archived.
# - ARCHIVE_NAME: The name of the archive file (without extension).
#
# The function will perform the following steps:
# - Generate a random key using OpenSSL.
# - Archive the specified directory.
# - Encrypt the archive using the generated key.
# - Write the encrypted data to the file $<TARGET_FILE_DIR:${TARGET}>/{ARCHIVE_NAME}.magnet".
# - Define the preprocessor macros {ARCHIVE_NAME}="{ARCHIVE_NAME}.magnet" and {ARCHIVE_NAME}_KEY="{KEY}" to the target.
function(magnet_package TARGET ASSET_DIR)
  set(options)
  set(oneValueArgs ARCHIVE_NAME)
  set(multiValueArgs)
  cmake_parse_arguments(
    PARSE_ARGV 2 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}"
  )

  if(NOT ARG_ARCHIVE_NAME)
    message(FATAL_ERROR "magnet-package: ARCHIVE_NAME not specified")
  endif()

  set(ARCHIVE_PATH $<TARGET_FILE_DIR:${TARGET}>/${ARG_ARCHIVE_NAME}.magnet)

  execute_process(
    COMMAND openssl rand -hex 32
    OUTPUT_VARIABLE KEY
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  add_custom_command(TARGET ${TARGET} PRE_BUILD
    COMMAND tar -cvf ${ARCHIVE_PATH} ${ASSET_DIR}
    # COMMAND tar cvf - ${ASSET_DIR} | openssl enc -chacha20 -K ${KEY} > ${ARCHIVE_PATH}
    COMMENT "Created \"${ARCHIVE_PATH}\" with key \"${KEY}\""
  )

  target_compile_definitions(${TARGET} PRIVATE ARCH_${ARG_ARCHIVE_NAME}="${ARG_ARCHIVE_NAME}.magnet" ARCH_${ARG_ARCHIVE_NAME}_KEY="${KEY}")
endfunction()

function(magnet_package_core TARGET)
  set(CORE_ASSETS_DIR ${CMAKE_SOURCE_DIR}/core/assets)
  magnet_package(${TARGET} ${CORE_ASSETS_DIR} ARCHIVE_NAME core)
endfunction()

magnet_package_core(magnet-core) # For ARCH_core and ARCH_core_KEY definitions