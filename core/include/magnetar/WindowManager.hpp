#pragma once

#include <GLFW/glfw3.h>

class WindowManager {
  GLFWwindow* window;

 public:
  WindowManager(uint32_t width, uint32_t height);

  static void pollEvents() { glfwPollEvents(); }

  static void resize_callback(GLFWwindow* window, int width, int height);

  bool shouldClose() const { return glfwWindowShouldClose(window); }
};