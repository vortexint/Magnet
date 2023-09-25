#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main(void) {
  GLFWwindow* window;

  if (!glfwInit()) return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "Game", NULL, NULL);
  if (!window) {

    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  gladLoadGL();

  glViewport(0, 0, INIT_WIDTH, INIT_HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}