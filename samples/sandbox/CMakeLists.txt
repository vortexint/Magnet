add_executable(sandbox main.cpp)

target_sources(sandbox PRIVATE 
  app.cpp
)

#target_precompile_headers(sandbox PRIVATE app.hpp)

target_link_libraries(sandbox PRIVATE magnet-core tweeny)


magnet_package_core(sandbox)
magnet_package(sandbox ${CMAKE_CURRENT_SOURCE_DIR}/assets data)