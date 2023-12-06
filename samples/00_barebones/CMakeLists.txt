project(barebones LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)

#target_precompile_headers(${PROJECT_NAME} PRIVATE app.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE magnet-core)

magnet_package(UNDEFINED) # no additional assets

# NOTE: UNDEFINED is a string that is unlikely to exist
# in the development environment. With this in mind, use anything.