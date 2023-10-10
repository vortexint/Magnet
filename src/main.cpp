#include "app/app.hpp"

#include "core/io/assets.hpp"
#include "core/io/input.hpp"
#include "core/gfx/window.hpp"

class GameContext {
  AssetManager assetMgr_;

public:
  GameContext() {

  }

};

/* ENTRY POINT */

int main(void) {
  AssetManager asset_manager;
  WindowManager window_manager(INIT_WIDTH, INIT_HEIGHT, GAME_TITLE, asset_manager);
  GameContext game_context;

  /* Loop until the user closes the window */
  while (!window_manager.shouldClose()) {

    window_manager.draw();

    window_manager.pollEvents();
  }

  return 0;
}