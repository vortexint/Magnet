#include "app.hpp"

const char* const TITLE = "Native UI Example";

int main(void) {
  ApplicationContext& appCtx = ApplicationContext::getInstance();
  appCtx.registerInterface(new Interface());

  appCtx.initialize(TITLE);

  return 0;
}