#include "window.hpp"

WindowManager::WindowManager() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, GAME_TITLE, NULL, NULL);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Failed to create a GLFW window");
  }

  /* Set window icon
  size_t dataSize;
  //auto imageData = assetMgr.LoadAsset("icon.png", dataSize);

  GLFWimage icons[1];
  icons[0].pixels = stbi_load_from_memory(const stbi_uc *buffer, int len, int *x, int *y, int *comp, int req_comp) glfwSetWindowIcon(window, 1, icons);
  
  glfwSetWindowIcon(window, 1, icons);
  free(icons[0].pixels);
  */

  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, resize_callback);

  /* GLAD manages function pointers for OpenGL so we initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  renderer_ = std::make_unique<Renderer>();
}

WindowManager::~WindowManager() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void WindowManager::resize_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void WindowManager::draw() const {
  renderer_->Clear();

  renderer_->Render();

  /* Swap front and back buffers */
  glfwSwapBuffers(window_);
}