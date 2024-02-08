#include "app.hpp"

/* ENTRY POINT */

int main(void) {
  ApplicationContext appCtx;
  appCtx.registerInterface(new Interface());

  appCtx.initialize(GAME_TITLE);

  return 0;
}