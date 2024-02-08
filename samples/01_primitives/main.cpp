#include <magnet/Application.hpp>

using namespace Magnet;

const char* const TITLE = "Primitives Example";

class Interface : public ProjectInterface {
 public:
  void init() override {
    ArchiveManager archiveMgr(ARCH_assets);

  }

  void update() override {
    // Project-specific
  }
};

int main(void) {
  Interface appInterface;
  ApplicationContext appCtx(appInterface);

  appCtx.initialize(TITLE);
  return 0;
}