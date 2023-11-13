#include <GLFW/glfw3.h>

#include <magnetar/Renderer.hpp>
#include <magnetar/WindowManager.hpp>

WindowManager::WindowManager(const char* title, uint32_t width, uint32_t height) {

}

WindowManager::~WindowManager() {
  glfwDestroyWindow(this->window);
  glfwTerminate();
}

void WindowManager::pollEvents() {
  glfwPollEvents();
}

void WindowManager::setResizeCallback(Renderer* renderer) {
  glfwSetFramebufferSizeCallback(window, renderer->resizeCallback);
}