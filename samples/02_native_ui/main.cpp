#include "app.hpp"

const char* const TITLE = "Native UI Example";

int main(void) {
  auto& appCtx = ApplicationContext::getInstance();
  appCtx.registerInterface(new Interface());

  appCtx.initialize(TITLE);

  return 0;
}