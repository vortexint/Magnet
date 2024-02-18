#include "Window.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/Input.hpp>

#include "Viewport.hpp"

namespace Magnet::Window {

void errorCallback(int error, const char* desc) {
  spdlog::error("GLFW Error {}: {}", error, desc);
}

void initialize(Context& context, const char* title) {
  GLFWwindow* window;
  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, title, nullptr, nullptr);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  /* Setup callbacks */
  glfwSetFramebufferSizeCallback(window, Viewport::setSize);
  glfwSetErrorCallback(errorCallback);

  /* Initialize Input */
  Input::initialize(window);
  context.setWindow(window);
}

}  // namespace Magnet::Window