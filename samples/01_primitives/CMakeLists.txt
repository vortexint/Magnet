project(01_primitives LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE magnet-core)

magnet_package(UNDEFINED)