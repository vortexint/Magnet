#pragma once

#include "../app.hpp"

class WindowManager {
  GLFWwindow *window_;

public:
  WindowManager();
  ~WindowManager();

  static void resize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
  }

  void update();

  bool shouldClose() const { return glfwWindowShouldClose(window_); }
};