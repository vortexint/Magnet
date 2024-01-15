#include <magnet/ApplicationContext.hpp>

const char* const TITLE = "Primitives Example";

class AppInterface : public Magnet::ProjectInterface {
 public:
  void init() override {
    AssetManager assetMgr(ARCH_assets);

  }

  void update() override {
    // Project-specific
  }
};

int main(void) {
  Magnet::ApplicationContext::registerInterface(new AppInterface());

  Magnet::ApplicationContext::initialize(TITLE);
  return 0;
}