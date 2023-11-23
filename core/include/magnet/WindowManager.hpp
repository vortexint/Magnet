#pragma once

#include <cstdint>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

// forward declarations
class Renderer;
class WindowManager {
  GLFWwindow* window;
  Renderer* renderer;

 public:
  WindowManager(Renderer* renderer, const char* windowTitle);
  ~WindowManager();

  void swapBuffers();
  static void pollEvents();

  /* Callbacks */
  static void resizeCallback(GLFWwindow* window, int width, int height);

  /* Getters */
  GLFWwindow* getWindow() { return window; }

  bool shouldClose() const;
};