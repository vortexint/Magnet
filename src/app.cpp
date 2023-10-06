#include "app.hpp"

#include "io/assets.hpp"
#include "io/input.hpp"
#include "gfx/shader.hpp"
#include "gfx/window.hpp"

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
    
  }

  return 0;
}