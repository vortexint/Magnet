add_executable(02_native_ui main.cpp)

target_sources(02_native_ui PRIVATE
  app.cpp
)

target_link_libraries(02_native_ui PRIVATE magnet-core)


magnet_package_core(02_native_ui)