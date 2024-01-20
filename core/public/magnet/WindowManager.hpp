#pragma once

#include <cstdint>

// forward declarations
struct GLFWwindow;
namespace Magnet {

const uint32_t INIT_WIDTH = 1280;
const uint32_t INIT_HEIGHT = 720;
class WindowManager {
  GLFWwindow* window;

 public:
  WindowManager(const char* windowTitle);
  ~WindowManager();

  void swapBuffers();
  static void pollEvents();

  /* Getters */
  GLFWwindow* getWindow() const { return window; }

  bool shouldClose() const;
};

} // namespace Magnet