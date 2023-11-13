#include "app.hpp"

#include <magnetar/ApplicationContext.hpp>

/* ENTRY POINT */

int main(void) {
  ApplicationContext::initialize(SECURE_ASSETS_ARCHIVE, GAME_TITLE, INIT_WIDTH,
                                 INIT_HEIGHT);

  ApplicationContext::run();
  return 0;
}