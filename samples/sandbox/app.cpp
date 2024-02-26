#include "app.hpp"

void App::init() {
  window = glfwGetCurrentContext();

  // Load icon
  {
    ID iconID = enqueueLoad(Mimetype::PNG, archiveMgr, "icon.png");
    Asset* iconAsset = nullptr;

    //while (!isAssetLoaded(iconID)) {
    //  std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //}

    iconAsset = getAsset(iconID);

    Magnet::Library::Texture* iconTexture =
      dynamic_cast<Magnet::Library::Texture*>(iconAsset);

    GLFWimage windowIcon;
    windowIcon.width = iconTexture->width;
    windowIcon.height = iconTexture->height;
    windowIcon.pixels = iconTexture->data;
    glfwSetWindowIcon(window, 1, &windowIcon);
  }
}

void App::update() {}