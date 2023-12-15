#include "app.hpp"

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Interface::init() {
  

  AssetManager* assetMgr = ApplicationContext::getAssetManager();
  WindowManager* windowMgr = ApplicationContext::getWindowManager();

  window = windowMgr->getWindow();
  ctx = Magnet::UI::getContext();

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

}

void Interface::update() {
  enum { EASY, HARD };
  static int op = EASY;
  static float value = 0.6f;
  static int i = 20;

  if (nk_begin(ctx, "Panel", nk_rect(50, 50, 230, 250),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE |
                 NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
    // first row with height: 30 composed of two widgets
    nk_layout_row_dynamic(ctx, 30, 2);

    if (nk_button_label(ctx, "button")) {
      /* event handling */
    }
    if (nk_button_label(ctx, "button")) {
      /* event handling */
    }

    if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
    if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

    /* custom widget pixel width */
    nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
    {
      nk_layout_row_push(ctx, 50);
      nk_label(ctx, "Volume:", NK_TEXT_LEFT);
      nk_layout_row_push(ctx, 110);
      nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
    }
    nk_layout_row_end(ctx);
  }
  nk_end(ctx);
}