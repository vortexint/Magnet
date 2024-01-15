#include <magnet/ApplicationContext.hpp>

const char* const TITLE = "Barebones Example";
class AppInterface : public Magnet::ProjectInterface {
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

int main(void) {
  Magnet::ApplicationContext::registerInterface(new AppInterface());

  Magnet::ApplicationContext::initialize(TITLE);
  return 0;
}