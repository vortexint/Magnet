#pragma once

#include <GLFW/glfw3.h>

class Renderer; // Forward declaration
class WindowManager {
  GLFWwindow* window;

 public:
  WindowManager(const char* title, uint32_t width, uint32_t height);
  ~WindowManager();

  static void pollEvents();

  void setResizeCallback(Renderer* renderer);

  bool shouldClose() const { return glfwWindowShouldClose(window); }
};