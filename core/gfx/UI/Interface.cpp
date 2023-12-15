/**
 * @file Interface.cpp
 * @author Igor Alexey (igor@vortex-dev.com)
 * @brief Not to be confused with ProjectInterface, manages Graphical User
 * Interface
 */

#include "magnet/ApplicationContext.hpp"
#include "magnet/UI.hpp"

#include <spdlog/spdlog.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

namespace Magnet::UI {
nk_context* ctx;

void initialize() {
  WindowManager* windowManager = ApplicationContext::getWindowManager();
  AssetManager* assetMgr = ApplicationContext::getAssetManager();

  ctx = nk_glfw3_init(windowManager->getWindow(), NK_GLFW3_INSTALL_CALLBACKS,
                      MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

  /* Load default fonts */
  {
    nk_font_atlas* atlas;
    std::vector<unsigned char> fontData;

    struct nk_font_config config = nk_font_config(24);
    config.oversample_h = 5;
    config.oversample_v = 5;

    nk_glfw3_font_stash_begin(&atlas);

    assetMgr->getAsset("fonts/Kanit-Regular.ttf", fontData);

    nk_font* kanit = nk_font_atlas_add_from_memory(
      atlas, fontData.data(), fontData.size(), 22, &config);

    assetMgr->getAsset("fonts/FiraCode-Regular.ttf", fontData);

    nk_font* firacd = nk_font_atlas_add_from_memory(atlas, fontData.data(),
                                              fontData.size(), 16, &config);

    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &firacd->handle);
  }

  /* Set theme */
  {
    struct nk_color table[NK_COLOR_COUNT];
    table[NK_COLOR_TEXT] = nk_rgb_hex("d8d8d8");
    table[NK_COLOR_WINDOW] = nk_rgb_hex("282828");
    table[NK_COLOR_HEADER] = nk_rgb_hex("212121");
    table[NK_COLOR_BORDER] = nk_rgb_hex("404040");
    table[NK_COLOR_BUTTON] = nk_rgb_hex("4a4a4a");
    table[NK_COLOR_BUTTON_HOVER] = nk_rgb_hex("5a5a5a");
    table[NK_COLOR_BUTTON_ACTIVE] = nk_rgb_hex("7a7a7a");
    table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
    table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
    table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
    table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
    table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
    table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
    table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
    table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
    table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
    table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
    table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
    table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
    table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
    table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
    table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
    table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
    nk_style_from_table(ctx, table);

    ctx->style.window.border = 1;
    ctx->style.button.rounding = 2;
    ctx->style.window.padding = {5, 5};
    ctx->style.window.spacing = {5, 5};
    ctx->style.window.header.spacing = {1,1};
  }
}

struct nk_context* getContext() { return ctx; }

}  // namespace Magnet::UI