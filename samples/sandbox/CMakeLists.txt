project(sandbox LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE 
  app.cpp
)

#target_precompile_headers(${PROJECT_NAME} PRIVATE app.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE magnet-core tweeny)


magnet_package_core(${PROJECT_NAME})
magnet_package(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/assets data)