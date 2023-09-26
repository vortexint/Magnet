#include "../app.hpp"

void processInput(GLFWwindow *window) {
  glfwPollEvents();
  
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
