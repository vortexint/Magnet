#pragma once

#include "gfx/renderer.hpp"

class WindowManager {

  AssetManager &asset_manager_;

  GLFWwindow *window_;
  std::unique_ptr<Renderer> renderer_;

public:
  WindowManager(int width, int height, const char *title, AssetManager &asset_manager);
  ~WindowManager();

  static void resize_callback(GLFWwindow *window, int width, int height);

  void draw() const;

  void pollEvents() const {glfwPollEvents();}

  bool shouldClose() const { return glfwWindowShouldClose(window_); }
};