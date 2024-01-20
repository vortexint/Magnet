#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/Input.hpp>

#include "imgui_impl_glfw.h"

namespace Magnet::Input {

Observer::Observer() { addObserver(this); }

void Observer::onKeyEvent(int, int, int) {}
void Observer::onMouseButtonEvent(int, int, int) {}
void Observer::onMouseMoveEvent(double, double) {}
void Observer::onMouseScrollEvent(double, double) {}

std::vector<Observer*> observers;

/* Callbacks */

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

  if (ImGui::GetIO().WantCaptureKeyboard) return;

  for (Observer* observer : observers) {
    observer->onKeyEvent(key, action, mods);
  }
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

  if (ImGui::GetIO().WantCaptureMouse) return;

  for (Observer* observer : observers) {
    observer->onMouseButtonEvent(button, action, mods);
  }
}

static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {

  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

  if (ImGui::GetIO().WantCaptureMouse) return;

  for (Observer* observer : observers) {
    observer->onMouseMoveEvent(xpos, ypos);
  }
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

  if (ImGui::GetIO().WantCaptureMouse) return;

  for (Observer* observer : observers) {
    observer->onMouseScrollEvent(xoffset, yoffset);
  }
}

void init() {
  GLFWwindow* window = glfwGetCurrentContext();

  glfwSetKeyCallback(window, keyCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, cursorPositionCallback);
  glfwSetScrollCallback(window, scrollCallback);
}

void addObserver(Observer* observer) { observers.emplace_back(observer); }

}  // namespace Magnet::Input