#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/InputManager.hpp>

#include "imgui_impl_glfw.h"

namespace Magnet {

Observer::Observer() { InputManager::getInstance().addObserver(this); }

void Observer::onKeyEvent(int, int, int) {}
void Observer::onMouseButtonEvent(int, int, int) {}
void Observer::onMouseMoveEvent(double, double) {}
void Observer::onMouseScrollEvent(double, double) {}

std::vector<Observer*> Magnet::InputManager::observers;

/* Callbacks */

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode,
                             int action, int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
  
  if (ImGui::GetIO().WantCaptureKeyboard) return;

  for (Observer* observer : observers) {
    observer->onKeyEvent(key, action, mods);
  }
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button,
                                       int action, int mods) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

  if (ImGui::GetIO().WantCaptureMouse) return;

  for (Observer* observer : observers) {
    observer->onMouseButtonEvent(button, action, mods);
  }
}

void InputManager::cursorPositionCallback(GLFWwindow* window, double xpos,
                                          double ypos) {
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

  if (ImGui::GetIO().WantCaptureMouse) return;

  for (Observer* observer : observers) {
    observer->onMouseMoveEvent(xpos, ypos);
  }
}

void InputManager::scrollCallback(GLFWwindow* window, double xoffset,
                                  double yoffset) {
  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

  if (ImGui::GetIO().WantCaptureMouse) return;

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

void InputManager::removeObserver(Observer *observer) {
  observers.erase(std::remove(observers.begin(), observers.end(), observer),
                  observers.end());
}

}  // namespace Magnet