#include "app.hpp"

#include "io/input.hpp"

void resize_callback(GLFWwindow* window, int width, int height) {
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

  /* GLAD manages function pointers for OpenGL so we want to initialize GLAD
   * before we call any OpenGL function */
  gladLoadGL();

  glViewport(0, 0, INIT_WIDTH, INIT_HEIGHT);
  glfwSetFramebufferSizeCallback(window, resize_callback);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Rendering */
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);

    /* Input */

    processInput(window);
  }

  glfwTerminate();
  return 0;
}