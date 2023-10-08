#pragma once

#include "renderer.hpp"

class WindowManager {
  
  GLFWwindow *window_;
  std::unique_ptr<Renderer> renderer_;

public:
  WindowManager();
  ~WindowManager();

  static void resize_callback(GLFWwindow *window, int width, int height);

  void draw() const;

  void pollEvents() const {glfwPollEvents();}

  bool shouldClose() const { return glfwWindowShouldClose(window_); }
};