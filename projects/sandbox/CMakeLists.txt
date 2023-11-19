project(sandbox LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)

#target_precompile_headers(${PROJECT_NAME} PRIVATE app.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE magnet-core tweeny)
                      
# Package project assets
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
magnet_package()