#include "app.hpp"

/* ENTRY POINT */

#include <cglm/cglm.h>
#include <iostream>
#include <iomanip>

int main(void) {
  ApplicationContext& appCtx = ApplicationContext::getInstance();
  appCtx.registerInterface(new Interface());

  appCtx.initialize(GAME_TITLE);

  return 0;
}