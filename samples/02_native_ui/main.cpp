#include "app.hpp"

const char* const TITLE = "Native UI Example";

int main(void) {
  ApplicationContext::registerInterface(new Interface());

  ApplicationContext::initialize(TITLE);

  return 0;
}