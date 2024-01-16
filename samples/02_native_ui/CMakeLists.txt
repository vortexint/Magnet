project(02_native_ui LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
  app.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE magnet-core)


magnet_package_core(${PROJECT_NAME})