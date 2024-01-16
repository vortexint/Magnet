#pragma once
#include <GLFW/glfw3.h>

namespace Magnet {
class InputManager {
 public:
  InputManager() = default;
  ~InputManager() = default;

  // Callbacks for GLFW input events
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods);
  static void mouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);
  static void cursorPositionCallback(GLFWwindow* window, double xpos,
                                     double ypos);
};

}  // namespace Magnet