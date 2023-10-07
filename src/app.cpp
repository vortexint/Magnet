#include "app.hpp"

#include "core/io/assets.hpp"
#include "core/io/input.hpp"
#include "core/gfx/shader.hpp"
#include "core/gfx/window.hpp"

class GameContext {
  AssetManager assetMgr_;

public:
  GameContext() {

  }

};

/* ENTRY POINT */

int main(void) {

  GameContext game_context;
  WindowManager window_manager;

  /* Loop until the user closes the window */
  while (!window_manager.shouldClose()) {

    window_manager.draw();

    window_manager.pollEvents();
  }

  return 0;
}