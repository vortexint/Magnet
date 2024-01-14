#include <magnet/ApplicationContext.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <magnet/Renderer.hpp>
#include <magnet/ShaderManager.hpp>
#include <magnet/WindowManager.hpp>

namespace Magnet {

std::unique_ptr<flecs::world> ApplicationContext::ecs = nullptr;
std::unique_ptr<AssetManager> ApplicationContext::assetManager = nullptr;
std::unique_ptr<ShaderManager> ApplicationContext::shaderManager = nullptr;
std::unique_ptr<WindowManager> ApplicationContext::windowManager = nullptr;
std::unique_ptr<Renderer> ApplicationContext::renderer = nullptr;

ProjectInterface *registeredInterface = nullptr;

void ApplicationContext::registerInterface(ProjectInterface *interface) {
  registeredInterface = interface;
}

void ApplicationContext::initialize(const char *windowTitle) {
  /* Logging */
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);
  auto logger = std::make_shared<spdlog::logger>(
    "multi_sink", spdlog::sinks_init_list{consoleSink, fileSink});
  spdlog::set_default_logger(logger);

  /* Component Initialization */
  ecs           = std::make_unique<flecs::world>();
  assetManager  = std::make_unique<AssetManager>(ARCH_core, ARCH_core_KEY);
  shaderManager = std::make_unique<ShaderManager>(assetManager.get());
  windowManager = std::make_unique<WindowManager>(windowTitle);
  renderer      = std::make_unique<Renderer>(ecs.get(), shaderManager.get());

  registeredInterface->init();

  // Main loop
  while (!windowManager->shouldClose()) {
    // Update all active systems and subsystems
    ecs->progress();

    // Render everything
    renderer->render();

    registeredInterface->update();

    // Swap buffers
    windowManager->swapBuffers();

    // Poll for and process events
    windowManager->pollEvents();
  }
}

}  // namespace Magnet