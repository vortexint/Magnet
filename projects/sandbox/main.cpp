#include <magnetar/ApplicationContext.hpp>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "app.hpp"

/* ENTRY POINT */

int main(void) {
  ApplicationContext::initialize(SECURE_ASSETS_ARCHIVE, GAME_TITLE);

  /* Project code */
  AssetManager*  assetMgr  = ApplicationContext::getAssetManager();
  WindowManager* windowMgr = ApplicationContext::getWindowManager();

  GLFWwindow* window = windowMgr->getWindow();

  // Load icon
  {
    assetMgr->openArchive();
    size_t dataSize;
    unsigned char *iconData = assetMgr->getAsset("icon.png", dataSize);

    GLFWimage icons[1];
    icons[0].pixels =
      stbi_load_from_memory(iconData, dataSize, &icons[0].width,
                            &icons[0].height, nullptr, STBI_rgb_alpha);

    glfwSetWindowIcon(window, 1, icons);

    free(icons[0].pixels);

    assetMgr->closeArchive();
  }

  ApplicationContext::run();
  return 0;
}