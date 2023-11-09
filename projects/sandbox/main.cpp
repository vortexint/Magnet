#include "app.hpp"

#include "gfx/window.hpp"

/* ENTRY POINT */

int main(void) {
  AssetManager asset_manager(SECURE_ASSETS_ARCHIVE);
  WindowManager window_manager(INIT_WIDTH, INIT_HEIGHT, GAME_TITLE, asset_manager);
  GameContext game_context(asset_manager);

  /* Loop until the user closes the window */
  while (!window_manager.shouldClose()) {

    game_context.Update();

    window_manager.draw();

    window_manager.pollEvents();
  }

  return 0;
}