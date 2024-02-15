#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <spdlog/spdlog.h>

#include <magnet/Application.hpp>
#include <magnet/Input.hpp>

namespace Magnet::Input {

std::vector<Observer*>& getObservers() {
  static std::vector<Observer*> observers;
  return observers;
}

Observer::Observer() { addObserver(this); }

void Observer::onKeyEvent(int, int, int, int) {}
void Observer::onMouseButtonEvent(int, int, int) {}
void Observer::onMouseMoveEvent(double, double) {}
void Observer::onMouseScrollEvent(double, double) {}

/* Callbacks */

void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) {
  for (Observer* observer : getObservers()) {
    observer->onKeyEvent(key, scancode, action, mods);
  }
}

void mouseButtonCallback(GLFWwindow*, int button, int action, int mods) {
  for (Observer* observer : getObservers()) {
    observer->onMouseButtonEvent(button, action, mods);
  }
}

void cursorPositionCallback(GLFWwindow*, double xpos, double ypos) {
  for (Observer* observer : getObservers()) {
    observer->onMouseMoveEvent(xpos, ypos);
  }
}

void scrollCallback(GLFWwindow*, double xoffset, double yoffset) {
  for (Observer* observer : getObservers()) {
    observer->onMouseScrollEvent(xoffset, yoffset);
  }
}

void initialize(GLFWwindow* window) {
  glfwSetKeyCallback(window, keyCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, cursorPositionCallback);
  glfwSetScrollCallback(window, scrollCallback);
}

void addObserver(Observer* observer) { getObservers().emplace_back(observer); }

void removeObserver(Observer* observer) {
  std::vector<Observer*> observers = getObservers();
  observers.erase(std::remove(observers.begin(), observers.end(), observer),
                  observers.end());
}

}  // namespace Magnet::Input