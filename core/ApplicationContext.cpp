#include "magnetar/ApplicationContext.hpp"

#include "magnetar/Renderer.hpp"
#include "magnetar/ShaderManager.hpp"
#include "magnetar/WindowManager.hpp"

std::unique_ptr<flecs::world> ApplicationContext::ecs = nullptr;
std::unique_ptr<AssetManager> ApplicationContext::assetManager = nullptr;
std::unique_ptr<WindowManager> ApplicationContext::windowManager = nullptr;
std::unique_ptr<ShaderManager> ApplicationContext::shaderManager = nullptr;
std::unique_ptr<Renderer> ApplicationContext::renderer = nullptr;

void ApplicationContext::initialize(const char *secureArchivePath,
                                    const char *windowTitle) {
  ecs = std::make_unique<flecs::world>();
  assetManager = std::make_unique<AssetManager>(secureArchivePath);
  shaderManager = std::make_unique<ShaderManager>(assetManager.get());
  renderer = std::make_unique<Renderer>(ecs.get(), shaderManager.get());
  windowManager = std::make_unique<WindowManager>(renderer.get(), windowTitle);

  renderer->initialize(); // Set up renderer
}

void ApplicationContext::run() {
  while (!windowManager->shouldClose()) {
    /* Update all active systems and subsystems */

    /* Render all visible objects */
    renderer->render();

    /* Swap buffers */
    windowManager->swapBuffers();

    /* Poll for and process events */
    windowManager->pollEvents();
  }
}