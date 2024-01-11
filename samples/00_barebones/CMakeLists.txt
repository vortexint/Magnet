project(00_barebones LANGUAGES C CXX)

add_executable(00_barebones main.cpp)

target_link_libraries(00_barebones PRIVATE magnet-core)