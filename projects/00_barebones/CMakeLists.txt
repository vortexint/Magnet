project(barebones LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)

#target_precompile_headers(${PROJECT_NAME} PRIVATE app.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE magnet-core)
                      
# Normally you would package assets here, but it goes beyond the scope of this example.
# set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
magnet_package()