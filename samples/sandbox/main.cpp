#include "app.hpp"

/* ENTRY POINT */

int main(void) {
  App appInterface;
  
  Application::registerContext(appInterface, GAME_TITLE);
  Application::initialize();

  return 0;
}