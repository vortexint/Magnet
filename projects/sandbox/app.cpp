#include "app.hpp"

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Interface::init() {
  AssetManager* assetMgr = ApplicationContext::getAssetManager();
  WindowManager* windowMgr = ApplicationContext::getWindowManager();

  GLFWwindow* window = windowMgr->getWindow();

  assetMgr->openArchive();
  // Load icon
  {
    std::vector<unsigned char> imageData;
    assetMgr->getAsset("icon.png", imageData);

    GLFWimage icons[1];
    icons[0].pixels =
      stbi_load_from_memory(imageData.data(), imageData.size(), &icons[0].width,
                            &icons[0].height, nullptr, STBI_rgb_alpha);

    glfwSetWindowIcon(window, 1, icons);

    free(icons[0].pixels);
  }

  assetMgr->closeArchive();
}
void Interface::update() {}