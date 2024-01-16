project(01_primitives LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE magnet-core)

magnet_package_core(${PROJECT_NAME})
magnet_package(01_primitives ${CMAKE_CURRENT_SOURCE_DIR}/assets assets)