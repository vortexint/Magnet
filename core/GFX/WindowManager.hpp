#pragma once

#include <cstdint>

// Forward declarations
struct GLFWwindow;
namespace Magnet {

const uint32_t INIT_WIDTH = 1280;
const uint32_t INIT_HEIGHT = 720;
class WindowManager {
  GLFWwindow* window;

  WindowManager();
  WindowManager(const WindowManager&) = delete;
  void operator=(const WindowManager&) = delete;

 public:
  static WindowManager& getInstance() {
    static WindowManager instance;
    return instance;
  }
  ~WindowManager();

  void setTitle(const char* title);
  void swapBuffers();
  void pollEvents();

  /* Getters */
  GLFWwindow* getWindow() const { return window; }

  bool shouldClose() const;
};

} // namespace Magnet