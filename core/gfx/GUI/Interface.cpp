/**
 * @file Interface.cpp
 * @author Igor Alexey (igor@vortex-dev.com)
 * @brief Not to be confused with ProjectInterface, manages Graphical User
 * Interface
 */

#include "magnet/ApplicationContext.hpp"
#include "magnet/GUI.hpp"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

namespace Magnet::GUI {
nk_context* ctx;

void initialize() {
  WindowManager* windowManager = ApplicationContext::getWindowManager();

  ctx = nk_glfw3_init(windowManager->getWindow(), NK_GLFW3_INSTALL_CALLBACKS,
                      MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

struct nk_context* getContext() { return ctx; }

}  // namespace Magnet::GUI