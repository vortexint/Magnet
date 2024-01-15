#include <magnet/ApplicationContext.hpp>
#include <magnet/UserInterface.hpp>
#include <magnet/Time.hpp>

namespace Magnet {

void DevTools(nk_context* ctx) {
  static bool show_console = false;
  static bool show_debug_info = false;

  //static GLFWwindow* window = ApplicationContext::getWindowManager()->getWindow();

  /* Debug Info */
  if(show_debug_info) {
    nk_begin(ctx, "DebugInfo", nk_rect(0, 0, 200, 200),
             NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_NO_INPUT);
    nk_layout_row_static(ctx, 30, 200, 1);
    nk_label(ctx, "Debug Info", NK_TEXT_LEFT);
    // FPS counter
    
    nk_end(ctx);
  }

  /* Console */
  if(show_console) {
    nk_begin(ctx, "Console", nk_rect(0, 0, 200, 200),
             NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_NO_INPUT);
    nk_layout_row_dynamic(ctx, 30, 1);
    nk_label(ctx, "Console", NK_TEXT_LEFT);
    
    nk_end(ctx);
  }
}
}  // namespace Magnet