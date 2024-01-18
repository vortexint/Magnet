#include <magnet/WindowManager.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/Renderer.hpp>
#include <magnet/InputManager.hpp>
#include <magnet/UserInterface.hpp>

namespace Magnet {

void resizeCallback(GLFWwindow*, int width, int height);
void errorCallback(int error, const char* desc);

WindowManager::WindowManager(const char* windowTitle) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);

  window =
    glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, windowTitle, nullptr, nullptr);

  glfwMakeContextCurrent(window);

  /* Setup callbacks */
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetErrorCallback(errorCallback);
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

bool WindowManager::shouldClose() const {
  return glfwWindowShouldClose(window);
}

void resizeCallback(GLFWwindow*, int width, int height) {
  ApplicationContext::getRenderer()->setSize(width, height);
}

void errorCallback(int error, const char* desc) {
  spdlog::error("GLFW Error {}: {}", error, desc);
}

}  // namespace Magnet