#include <magnet/Application.hpp>

using namespace Magnet;

const char* const TITLE = "Primitives Example";

class App : public Magnet::Context {
 public:
  void init() override {
    ArchiveManager archiveMgr(ARCH_assets);
  }

  void update() override {
    // Project-specific
  }
};

int main(void) {
  App application;

  Application::registerContext(application, TITLE);
  Application::initialize();

  return 0;
}