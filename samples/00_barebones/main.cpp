#include <magnet/Application.hpp>

using namespace Magnet;

const char* const TITLE = "Barebones Example";
class AppInterface : public ProjectInterface {
public:
  void init() override {
    // ArchiveManager archiveMgr("archive.magnet", "key")
    // WindowManager* windowMgr = ApplicationContext::getWindowManager();

    // Here you would use the ArchiveManager to load essential assets of your
    // project, including shaders, textures, sounds, etc...
  }

  void update() override {
    // Project-specific
  }
};

/* ENTRY POINT */

int main(void) {
  ApplicationContext appCtx;

  appCtx.registerInterface(new AppInterface());

  appCtx.initialize(TITLE);
  return 0;
}