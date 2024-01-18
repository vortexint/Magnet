#include "app.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Interface::init() {
  
  AssetManager assetMgr(ARCH_data, ARCH_data_KEY);
  WindowManager* windowMgr = ApplicationContext::getWindowManager();

  window = windowMgr->getWindow();
  ctx = Magnet::get_nk_context();

  // Load icon
  {
    std::vector<unsigned char> imageData;
    assetMgr.getAsset("icon.png", imageData);

    GLFWimage icons[1] = {};
    icons[0].pixels =
      stbi_load_from_memory(imageData.data(), imageData.size(), &icons[0].width,
                            &icons[0].height, nullptr, STBI_rgb_alpha);

    glfwSetWindowIcon(window, 1, icons);

    free(icons[0].pixels);
  }

}

void Interface::update() {
  Magnet::DevTools(ctx);

}