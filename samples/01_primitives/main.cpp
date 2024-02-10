#include <magnet/Application.hpp>

using namespace Magnet;

const char* const TITLE = "Primitives Example";

class Interface : public ProjectInterface {
 public:
  void init(ApplicationContext& ctx) override {
    ArchiveManager archiveMgr(ARCH_assets);
  }

  void update(ApplicationContext& ctx) override {
    // Project-specific
  }
};

int main(void) {
  Interface appInterface;
  ApplicationContext appCtx(appInterface);

  appCtx.initialize(TITLE);
  return 0;
}