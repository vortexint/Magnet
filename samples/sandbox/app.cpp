#include "app.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void App::init() {
  ArchiveManager archiveMgr(ARCH_data, ARCH_data_KEY);

  window = glfwGetCurrentContext();

  // Load icon
  {
    std::vector<unsigned char> imageData;
    archiveMgr.loadFile("icon.png", imageData);

    GLFWimage icons[1] = {};
    icons[0].pixels =
      stbi_load_from_memory(imageData.data(), imageData.size(), &icons[0].width,
                            &icons[0].height, nullptr, STBI_rgb_alpha);

    glfwSetWindowIcon(window, 1, icons);

    free(icons[0].pixels);
  }

}

void App::update() {
  
}