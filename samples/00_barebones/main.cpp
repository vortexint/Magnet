#include <magnet/Application.hpp>

using namespace Magnet;
class myApp : public Magnet::Context {
 public:
  void init() override {
    // ArchiveManager archiveMgr("archive.magnet", "key")

    // Here you would use your ArchiveManager to load essential assets of your
    // project, including shaders, textures, sounds, etc...

    // For quick logging, you can use the default logger which has a console
    // (only DEBUG) and file (output.log) sinks.
    spdlog::info("Hello, world!");
  }

  void update() override {
    // ...
  }
};

/* ENTRY POINT */

int main(void) {
  myApp application;

  Application::registerContext(application, "My App");
  Application::initialize();
  
  return 0;
}