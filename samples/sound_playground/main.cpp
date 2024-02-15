#include "app.hpp"

/* ENTRY POINT */

#include <magnet/Components.hpp>
#include <cglm/cglm.h>

#include <iostream>

int main(void) {
  App application;

  Application::registerContext(application, GAME_TITLE);
  Application::initialize();

  return 0;
}