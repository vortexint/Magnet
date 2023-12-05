#include <magnet/ApplicationContext.hpp>

const char* const TITLE = "Barebones Example";

class AppInterface : public ProjectInterface {
 public:
  void init() override {
    // AssetManager*  assetMgr  = ApplicationContext::getAssetManager();
    // WindowManager* windowMgr = ApplicationContext::getWindowManager();

    // Under normal circumstances, here you would use the AssetManager to load
    // essential assets of your project, including textures, sounds, etc.
  }

  void update() override {
    // Project-specific update logic
  }
};

int main(void) {
  ApplicationContext::registerInterface(new AppInterface());

  ApplicationContext::initialize(TITLE);
  return 0;
}