add_executable(sound_playground main.cpp)

target_sources(sound_playground PRIVATE 
  app.cpp
)

#target_precompile_headers(sound_playground PRIVATE app.hpp)

target_link_libraries(sound_playground PRIVATE magnet-core tweeny)


magnet_package_core(sound_playground)
magnet_package(sound_playground ${CMAKE_CURRENT_SOURCE_DIR}/assets data)