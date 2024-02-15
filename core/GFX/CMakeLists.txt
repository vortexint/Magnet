target_sources(magnet-core PRIVATE
  glad.c
  Pipeline.cpp
  Shaders.cpp
  Viewport.cpp
  WindowManager.cpp
)

add_subdirectory(UI)