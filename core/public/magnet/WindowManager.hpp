#pragma once

#include <cstdint>

// forward declarations
struct GLFWwindow;
namespace Magnet {
class Renderer;
class WindowManager {
  GLFWwindow* window;

 public:
  WindowManager(const char* windowTitle);
  ~WindowManager();

  void swapBuffers();
  static void pollEvents();

  /* Callbacks */
  static void resizeCallback(GLFWwindow* window, int width, int height);

  /* Getters */
  GLFWwindow* getWindow() const { return window; }

  bool shouldClose() const;
};

} // namespace Magnet