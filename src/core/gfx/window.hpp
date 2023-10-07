#pragma once

#include "app.hpp"

class WindowManager {
  GLFWwindow *window_;

public:
  WindowManager();
  ~WindowManager();

  static void resize_callback(GLFWwindow *window, int width, int height);

  void draw() const;

  void pollEvents() const {glfwPollEvents();}

  bool shouldClose() const { return glfwWindowShouldClose(window_); }
};