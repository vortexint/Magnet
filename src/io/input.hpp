#pragma once

#include "../app.hpp"

void processInput(GLFWwindow *window);

class InputService {
public:
  static InputService &instance() {
    static InputService instance;
    return instance;
  }
  // input handling methods here ...

private:
  InputService() {} // Private so no other instance can be created manually

  static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);

  static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};