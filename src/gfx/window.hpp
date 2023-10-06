#pragma once

class WindowManager {
private:
  GLFWwindow *window_;

public:
  WindowManager();
  ~WindowManager();

  static void resize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
  }

  bool shouldClose() const { return glfwWindowShouldClose(window_); }

  void swapBuffers() const { glfwSwapBuffers(window_); }

  void pollEvents() const { glfwPollEvents(); }

  GLFWwindow *getWindow() const { return window_; }
};