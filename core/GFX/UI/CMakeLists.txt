target_sources(magnet-core PRIVATE
  imgui_draw.cpp
  imgui_freetype.cpp
  imgui_impl_glfw.cpp
  imgui_impl_opengl3.cpp
  imgui_tables.cpp
  imgui_widgets.cpp
  imgui.cpp
  UserInterface.cpp
)

add_subdirectory(widgets)