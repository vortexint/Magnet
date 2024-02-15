#include "app.hpp"

const char* const TITLE = "Native UI Example";

int main(void) {
  App application;

  Application::registerContext(application, TITLE);
  Application::initialize();
  
  return 0;
}
