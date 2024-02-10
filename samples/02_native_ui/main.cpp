#include "app.hpp"

const char* const TITLE = "Native UI Example";

int main(void) {
  Interface appInterface;
  ApplicationContext appCtx(appInterface);

  appCtx.initialize(TITLE);

  return 0;
}
