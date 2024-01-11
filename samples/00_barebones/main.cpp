#include <magnet/ApplicationContext.hpp>

const char* const TITLE = "Barebones Example";

class AppInterface : public ProjectInterface {
 public:
  void init() override {
    // AssetManager  assetMgr("archive.magnet", "key")
    // WindowManager* windowMgr = ApplicationContext::getWindowManager();

    // Here you would use the AssetManager to load essential assets of your
    // project, including shaders, textures, sounds, etc...
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