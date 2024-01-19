add_executable(00_barebones main.cpp)

target_link_libraries(00_barebones PRIVATE magnet-core)

magnet_package_core(00_barebones)