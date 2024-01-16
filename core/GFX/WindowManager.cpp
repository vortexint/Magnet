#include <magnet/WindowManager.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/Renderer.hpp>
#include <magnet/InputManager.hpp>
#include <magnet/UserInterface.hpp>

namespace Magnet {

WindowManager::WindowManager(const char* windowTitle) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);

  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW");
  }

  window =
    glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, windowTitle, nullptr, nullptr);

  glfwMakeContextCurrent(window);

  /* Setup callbacks */
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetKeyCallback(window, InputManager::keyCallback);
  glfwSetMouseButtonCallback(window, InputManager::mouseButtonCallback);
  glfwSetCursorPosCallback(window, InputManager::cursorPositionCallback);
}

WindowManager::~WindowManager() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void WindowManager::swapBuffers() {
  glfwSwapBuffers(window);
}

void WindowManager::pollEvents() {
  glfwPollEvents();
  nk_impl_new_frame();
}

void WindowManager::resizeCallback(GLFWwindow*, int width, int height) {
  ApplicationContext::getRenderer()->setSize(width, height);
}

bool WindowManager::shouldClose() const {
  return glfwWindowShouldClose(window);
}

}  // namespace Magnet