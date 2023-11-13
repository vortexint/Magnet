#include "magnetar/ApplicationContext.hpp"

std::unique_ptr<flecs::world> ApplicationContext::ecs = nullptr;
std::unique_ptr<AssetManager> ApplicationContext::assetManager = nullptr;
std::unique_ptr<WindowManager> ApplicationContext::windowManager = nullptr;
std::unique_ptr<Renderer> ApplicationContext::renderer = nullptr;

void ApplicationContext::initialize(const char *secureArchivePath,
                                    const char *gameTitle, uint32_t width,
                                    uint32_t height) {
  ecs = std::make_unique<flecs::world>();
  assetManager = std::make_unique<AssetManager>(secureArchivePath);
  windowManager = std::make_unique<WindowManager>(gameTitle, width, height);
  renderer = std::make_unique<Renderer>(ecs.get());

  // dependency injection to avoid coupling
  windowManager->setResizeCallback(renderer.get());
}

void ApplicationContext::run() {
  while (!windowManager->shouldClose()) {
    /* Update all active systems and subsystems */

    /* Render all visible objects */

    /* Swap buffers */

    windowManager->pollEvents();
  }
}