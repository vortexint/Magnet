#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/InputManager.hpp>
#include <spdlog/spdlog.h>

namespace Magnet {

Observer::Observer() {
  InputManager* inputManager = ApplicationContext::getInputManager();
  inputManager->addObserver(this);
}

static void keyCallback(GLFWwindow*, int, int, int, int);
static void mouseButtonCallback(GLFWwindow*, int, int, int);
static void cursorPositionCallback(GLFWwindow*, double, double);
static void scrollCallback(GLFWwindow*, double, double);

InputManager::InputManager() {
  GLFWwindow* window = glfwGetCurrentContext();
  glfwSetWindowUserPointer(window, this);

  glfwSetKeyCallback(window, keyCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, cursorPositionCallback);
  glfwSetScrollCallback(window, scrollCallback);
}

void InputManager::addObserver(Observer* observer) {
  observers.push_back(observer);
}

void InputManager::notifyKeyEvent(int key, int action, int mods) {
  for (auto& observer : observers) {
    observer->onKeyEvent(key, action, mods);
  }
}

void InputManager::notifyMouseButtonEvent(int button, int action, int mods) {
  for (auto& observer : observers) {
    observer->onMouseButtonEvent(button, action, mods);
  }
}

void InputManager::notifyMouseMoveEvent(double xpos, double ypos) {
  for (auto& observer : observers) {
    observer->onMouseMoveEvent(xpos, ypos);
  }
}

void InputManager::notifyMouseScrollEvent(double xoffset, double yoffset) {
  for (auto& observer : observers) {
    observer->onMouseScrollEvent(xoffset, yoffset);
  }
}

void keyCallback(GLFWwindow* window, int key, int, int action, int mods) {
  // parameter 3 (scancode) is not used
  InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
  inputManager->notifyKeyEvent(key, action, mods);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
  inputManager->notifyMouseButtonEvent(button, action, mods);
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
  InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
  inputManager->notifyMouseMoveEvent(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
  inputManager->notifyMouseScrollEvent(xoffset, yoffset);
}

}  // namespace Magnet