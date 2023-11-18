#include <magnetar/ApplicationContext.hpp>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "app.hpp"

/* ENTRY POINT */

int main(void) {
  ApplicationContext::initialize(GAME_TITLE);

  /* Project code */
  AssetManager*  assetMgr  = ApplicationContext::getAssetManager();
  WindowManager* windowMgr = ApplicationContext::getWindowManager();

  GLFWwindow* window = windowMgr->getWindow();

  assetMgr->openArchive(SECURE_ASSETS_ARCHIVE);
  // Load icon
  {

    std::vector<unsigned char> icon = assetMgr->getAsset(
      "icon.png"
      );

    GLFWimage icons[1];
    icons[0].pixels =
      stbi_load_from_memory(icon.data(), icon.size(), &icons[0].width,
                            &icons[0].height, nullptr, STBI_rgb_alpha);

    glfwSetWindowIcon(window, 1, icons);

    free(icons[0].pixels);
  }

  assetMgr->closeArchive();

  ApplicationContext::run();
  return 0;
}