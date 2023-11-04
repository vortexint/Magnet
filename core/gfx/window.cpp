#include "window.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

WindowManager::WindowManager(int width, int height, const char *title,
                             AssetManager &asset_manager)
    : asset_manager_(asset_manager) {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window_) {
    const char *description;
    glfwGetError(&description);

    throw std::runtime_error(description);
  }

  /* Setting the window's icon */
  {
    asset_manager_.openArchive();

    size_t dataSize;
    unsigned char *iconData = asset_manager.getAsset("icon.png", dataSize);
    int comp;

    GLFWimage icons[1];
    icons[0].pixels =
        stbi_load_from_memory(iconData, dataSize, &icons[0].width,
                              &icons[0].height, &comp, STBI_rgb_alpha);

    glfwSetWindowIcon(window_, 1, icons);

    free(icons[0].pixels);

    asset_manager_.closeArchive(); // Close the archive after we're done with it
  }

  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, resize_callback);

  /* GLAD manages function pointers for OpenGL so we initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  renderer_ = std::make_unique<Renderer>(asset_manager);
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