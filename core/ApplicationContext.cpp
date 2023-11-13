#include "magnetar/ApplicationContext.hpp"

void ApplicationContext::initialize(const char *secureArchivePath,
                                    const char *gameTitle, uint32_t width,
                                    uint32_t height) {
  assetManager = std::make_unique<AssetManager>(secureArchivePath);
}

void ApplicationContext::run() {
  while (!windowManager->shouldClose()) {
    /* Update all active systems and subsystems */
    
    /* Render all visible objects */

    /* Swap buffers */

    windowManager->pollEvents();
  }
}