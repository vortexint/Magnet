project(01_primitives LANGUAGES C CXX)

add_executable(01_primitives main.cpp)

target_link_libraries(01_primitives PRIVATE magnet-core)