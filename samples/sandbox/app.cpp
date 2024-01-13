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
    assetMgr.getAsset("./icon.png", imageData);

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

  static float value = 0.6f;
  
  if(nk_begin(ctx, "Hello World", nk_rect(50, 50, 200, 200),
              NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
    nk_layout_row_dynamic(ctx, 30, 2);
    nk_button_label(ctx, "Modal");

    if(nk_button_label(ctx, "button")) {
      spdlog::info("Button clicked");
    }
    
    nk_layout_row_begin(ctx, NK_STATIC, 30, 20);

    nk_layout_row_push(ctx, 50);
    nk_label(ctx, "Value:", NK_TEXT_LEFT);
    nk_layout_row_push(ctx, 110);
    nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);

    nk_layout_row_end(ctx);
  }
  nk_end(ctx);

}