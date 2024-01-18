#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/Time.hpp>
#include <magnet/UserInterface.hpp>
#include <magnet/InputManager.hpp>

extern nk_font* firacode;
extern nk_font* roboto;

namespace Magnet {

static bool show_console = false;
static bool show_debug_info = false;

class dt_observer : public Observer {
 public:
  void onKeyEvent(int key, int action, int) override {
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
      show_debug_info = !show_debug_info;
    }
  }
};

void DevTools(nk_context* ctx) {
  static dt_observer observer;
  /* Debug Info */
  if (show_debug_info) {
    nk_style_push_style_item(ctx, &ctx->style.window.fixed_background,
                             nk_style_item_color(nk_rgba(0, 0, 0, 0)));
    nk_style_push_font(ctx, &firacode->handle);
    nk_begin(ctx, "DebugInfo", nk_rect(0, 0, 300, 200),
             NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_NO_INPUT);
    nk_layout_row_dynamic(ctx, 16, 1);
    nk_label(ctx, "Debug Info - F3", NK_TEXT_CENTERED);

    static char infoText[256];

    /* Performance Section */

    // FPS counter
    double deltaTime = Time::getDelta();
    double fps = 1.0 / deltaTime;

    sprintf(infoText, "%.2f FPS", fps);

    nk_label(ctx, infoText, NK_TEXT_LEFT);

    /* Info Section */

    // Vendor
    static const GLubyte* renderer = glGetString(GL_RENDERER);
    static const GLubyte* version = glGetString(GL_VERSION);

    sprintf(infoText, "%s", renderer);

    nk_label(ctx, infoText, NK_TEXT_LEFT);

    sprintf(infoText, "GL %s", version);

    nk_label(ctx, infoText, NK_TEXT_LEFT);

    nk_end(ctx);
    nk_style_pop_style_item(ctx);
    nk_style_pop_font(ctx);
  }

  /* Console */
  if (show_console) {
    nk_begin(ctx, "Console", nk_rect(0, 0, 200, 200),
             NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_NO_INPUT);
    nk_layout_row_dynamic(ctx, 30, 1);

    //static char command[1024];

    nk_end(ctx);
  }
}
}  // namespace Magnet