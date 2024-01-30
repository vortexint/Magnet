#include <magnet/ApplicationContext.hpp>

using namespace Magnet;

const char* const TITLE = "Primitives Example";

class AppInterface : public ProjectInterface {
 public:
  void init() override {
    ArchiveManager archiveMgr(ARCH_assets);

  }

  void update() override {
    // Project-specific
  }
};

int main(void) {
  auto& applicationContext = ApplicationContext::getInstance();
  applicationContext.registerInterface(new AppInterface());

  applicationContext.initialize(TITLE);
  return 0;
}