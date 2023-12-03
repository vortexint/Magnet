#include <magnet/ApplicationContext.hpp>

const char* const TITLE = "Barebones Example";

class Interface : public ProjectInterface {
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
  ApplicationContext::registerInterface(new Interface());

  ApplicationContext::initialize(TITLE);
  return 0;
}