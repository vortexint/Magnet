#include <magnet/WindowManager.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/Input.hpp>
#include <magnet/Renderer.hpp>

namespace Magnet {

void resizeCallback(GLFWwindow*, int width, int height);
void errorCallback(int error, const char* desc);

WindowManager::WindowManager(const char* windowTitle) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);

  window =
    glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, windowTitle, nullptr, nullptr);

  glfwMakeContextCurrent(window);
  //glfwSwapInterval(0);

  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  /* Setup callbacks */
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetErrorCallback(errorCallback);

  /* Initialize input management */
  Input::init();
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
}

bool WindowManager::shouldClose() const {
  return glfwWindowShouldClose(window);
}

void resizeCallback(GLFWwindow*, int width, int height) {
  Renderer::setSize(width, height);
}

void errorCallback(int error, const char* desc) {
  spdlog::error("GLFW Error {}: {}", error, desc);
}

}  // namespace Magnet