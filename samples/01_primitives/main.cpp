#include <magnet/ApplicationContext.hpp>

using namespace Magnet;

const char* const TITLE = "Primitives Example";

class AppInterface : public ProjectInterface {
 public:
  void init() override {
    AssetManager assetMgr(ARCH_assets);

  }

  void update() override {
    // Project-specific
  }
};

int main(void) {
  ApplicationContext::registerInterface(new AppInterface());

  ApplicationContext::initialize(TITLE);
  return 0;
}