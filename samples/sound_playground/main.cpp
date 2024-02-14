#include "app.hpp"

/* ENTRY POINT */

int main(void) {
  App application;

  Application::registerContext(application, GAME_TITLE);
  Application::initialize();

  return 0;
}