#include <magnet/ApplicationContext.hpp>

using namespace Magnet;

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
    // Project-specific
  }
};

/* ENTRY POINT */

int main(void) {
  auto& applicationContext = ApplicationContext::getInstance();

  applicationContext.registerInterface(new AppInterface());

  applicationContext.initialize(TITLE);
  return 0;
}