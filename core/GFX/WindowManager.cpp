#include "WindowManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <magnet/Application.hpp>
#include <magnet/InputManager.hpp>

#include "Viewport.hpp"

namespace Magnet::Window {

void errorCallback(int error, const char* desc);

void initialize(const char* title) {
  GLFWwindow* window;

  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "Application", nullptr, nullptr);

  glfwSetWindowTitle(window, title);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  /* Setup callbacks */
  glfwSetFramebufferSizeCallback(window, Viewport::setSize);
  glfwSetErrorCallback(errorCallback);

  /* Initialize InputManager */
  InputManager::getInstance();
}

void shutdown() {
  glfwDestroyWindow(glfwGetCurrentContext());
  glfwTerminate();
}

void swapBuffers() {
  glfwSwapBuffers(glfwGetCurrentContext());
}

void pollEvents() {
  glfwPollEvents();
}

bool shouldClose() {
  return glfwWindowShouldClose(glfwGetCurrentContext());
}

void errorCallback(int error, const char* desc) {
  spdlog::error("GLFW Error {}: {}", error, desc);
}

}  // namespace Magnet::Window