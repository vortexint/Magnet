#pragma once

#include <memory>
#include <vector>

#define GLFW_INCLUDE_NONE // Don't include OpenGL headers
#include <GLFW/glfw3.h>

namespace Magnet {

class Observer {
 public:
  Observer();
  virtual void onKeyEvent(int key, int action, int mods);
  virtual void onMouseButtonEvent(int button, int action, int mods);
  virtual void onMouseMoveEvent(double xpos, double ypos);
  virtual void onMouseScrollEvent(double xoffset, double yoffset);
};

class InputManager {
  static std::vector<Observer*> observers;

  InputManager();
  InputManager(const InputManager&) = delete;
  void operator=(const InputManager&) = delete;

  static void keyCallback(GLFWwindow*, int, int, int, int);
  static void mouseButtonCallback(GLFWwindow*, int, int, int);
  static void cursorPositionCallback(GLFWwindow*, double, double);
  static void scrollCallback(GLFWwindow*, double, double);

  friend class Observer; // Allow observers to register themselves
  static void addObserver(Observer* observer);
 public:
  static InputManager& getInstance() {
    static InputManager instance;
    return instance;
  }

  static void removeObserver(Observer* observer);
};

}  // namespace Magnet