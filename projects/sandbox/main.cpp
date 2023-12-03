#include "app.hpp"

/* ENTRY POINT */

int main(void) {
  ApplicationContext::registerInterface(new Interface());
  
  ApplicationContext::initialize(GAME_TITLE);

  return 0;
}