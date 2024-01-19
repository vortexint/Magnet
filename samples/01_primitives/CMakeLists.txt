add_executable(01_primitives main.cpp)

target_link_libraries(01_primitives PRIVATE magnet-core)

magnet_package_core(01_primitives)
magnet_package(01_primitives ${CMAKE_CURRENT_SOURCE_DIR}/assets assets)