#include "magnet/WindowManager.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "magnet/Renderer.hpp"

#include "UI.hpp"

WindowManager::WindowManager(Renderer* renderer, const char* windowTitle) : renderer(renderer)  {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);

  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW");
  }

  window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, windowTitle, nullptr, nullptr);

  glfwMakeContextCurrent(window);

  glfwSetWindowUserPointer(window, this);

  /* Setup callbacks */
  glfwSetFramebufferSizeCallback(window, resizeCallback);
}

WindowManager::~WindowManager() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void WindowManager::swapBuffers() { glfwSwapBuffers(window); }

void WindowManager::pollEvents() { 
  glfwPollEvents();
  nk_glfw3_new_frame();
}

void WindowManager::resizeCallback(GLFWwindow* window, int width, int height) {
  static_cast<WindowManager*>(glfwGetWindowUserPointer(window))
    ->renderer->resize(width, height);
}

bool WindowManager::shouldClose() const {
  return glfwWindowShouldClose(window);
}