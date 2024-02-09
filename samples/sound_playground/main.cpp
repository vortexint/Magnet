#include "app.hpp"

/* ENTRY POINT */

#include <magnet/Components.hpp>
#include <cglm/cglm.h>

#include <iostream>

int main(void) {
  Interface appInterface;
  ApplicationContext appCtx(appInterface);

  appCtx.initialize(GAME_TITLE);

  return 0;
}