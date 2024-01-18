#pragma once
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

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

  static void keyCallback(GLFWwindow*, int, int, int, int);
  static void mouseButtonCallback(GLFWwindow*, int, int, int);
  static void cursorPositionCallback(GLFWwindow*, double, double);
  static void scrollCallback(GLFWwindow*, double, double);

 public:
  InputManager();

  static void addObserver(Observer* observer);
};

}  // namespace Magnet