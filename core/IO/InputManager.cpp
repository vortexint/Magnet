#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/InputManager.hpp>

namespace Magnet {

Observer::Observer() { InputManager::getInstance().addObserver(this); }

void Observer::onKeyEvent(int, int, int, int) {}
void Observer::onMouseButtonEvent(int, int, int) {}
void Observer::onMouseMoveEvent(double, double) {}
void Observer::onMouseScrollEvent(double, double) {}

std::vector<Observer*> Magnet::InputManager::observers;

/* Callbacks */

void InputManager::keyCallback(GLFWwindow*, int key, int scancode, int action,
                               int mods) {
  for (Observer* observer : observers) {
    observer->onKeyEvent(key, scancode, action, mods);
  }
}

void InputManager::mouseButtonCallback(GLFWwindow*, int button, int action,
                                       int mods) {
  for (Observer* observer : observers) {
    observer->onMouseButtonEvent(button, action, mods);
  }
}

void InputManager::cursorPositionCallback(GLFWwindow*, double xpos,
                                          double ypos) {
  for (Observer* observer : observers) {
    observer->onMouseMoveEvent(xpos, ypos);
  }
}

void InputManager::scrollCallback(GLFWwindow*, double xoffset, double yoffset) {
  for (Observer* observer : observers) {
    observer->onMouseScrollEvent(xoffset, yoffset);
  }
}

InputManager::InputManager() {
  GLFWwindow* window = glfwGetCurrentContext();

  glfwSetKeyCallback(window, keyCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, cursorPositionCallback);
  glfwSetScrollCallback(window, scrollCallback);
}

void InputManager::addObserver(Observer* observer) {
  observers.emplace_back(observer);
}

void InputManager::removeObserver(Observer* observer) {
  observers.erase(std::remove(observers.begin(), observers.end(), observer),
                  observers.end());
}

}  // namespace Magnet