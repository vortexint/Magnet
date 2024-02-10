#include <magnet/Application.hpp>

using namespace Magnet;

const char* const TITLE = "Barebones Example";
class AppInterface : public ProjectInterface {
public:
 void init(ApplicationContext& ctx) override {
   // ArchiveManager archiveMgr("archive.magnet", "key")

   // Here you would use the ArchiveManager to load essential assets of your
   // project, including shaders, textures, sounds, etc...
 }

 void update(ApplicationContext& ctx) override {
   // ...
 }
};

/* ENTRY POINT */

int main(void) {
  AppInterface appInterface;
  ApplicationContext appCtx(appInterface);

  appCtx.initialize(TITLE);
  return 0;
}