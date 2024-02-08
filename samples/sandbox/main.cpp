#include "app.hpp"

/* ENTRY POINT */

int main(void) {
  Interface appInterface;
  ApplicationContext appCtx(appInterface);

  appCtx.initialize(GAME_TITLE);

  return 0;
}