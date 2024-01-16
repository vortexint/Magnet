#include "app.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void Interface::init() {
  ctx = Magnet::get_nk_context();

}

void Interface::update() {
  Magnet::DevTools(ctx);

  static float slider_value = 0.6f;
  static nk_size progress_value = 0;

  if (progress_value == 100)
    progress_value = 0;

  progress_value++;
    

  if (nk_begin(ctx, "Nuklear Demo", nk_rect(50, 50, 300, 200),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                 NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE |
                 NK_WINDOW_NO_SCROLLBAR)) {
    nk_layout_row_dynamic(ctx, 0, 2);

    nk_button_label(ctx, "No-op");
    if (nk_button_label(ctx, "Button")) {
      spdlog::info("Button clicked");
    }

    nk_layout_row(ctx, NK_DYNAMIC, 0, 2, (float[]){.15f, .85f});
    nk_label(ctx, "Value:", NK_TEXT_LEFT);
    nk_slider_float(ctx, 0, &slider_value, 100.0f, 1.f);

    nk_layout_row_dynamic(ctx, 0, 1);
    nk_label(ctx, "Animated Progress bar", NK_TEXT_LEFT);
    nk_progress(ctx, &progress_value, 100, NK_FIXED);
  }
  nk_end(ctx);
}