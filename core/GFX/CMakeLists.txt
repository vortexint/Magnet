target_sources(magnet-core PRIVATE
  glad.c
  Renderer.cpp
  ShaderManager.cpp
  Viewport.cpp
  WindowManager.cpp
)

add_subdirectory(UI)