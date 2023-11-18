#include "magnetar/ApplicationContext.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "magnetar/Renderer.hpp"
#include "magnetar/ShaderManager.hpp"
#include "magnetar/WindowManager.hpp"

std::unique_ptr<flecs::world>  ApplicationContext::ecs = nullptr;
std::unique_ptr<AssetManager>  ApplicationContext::assetManager = nullptr;
std::unique_ptr<ShaderManager> ApplicationContext::shaderManager = nullptr;
std::unique_ptr<Renderer>      ApplicationContext::renderer = nullptr;
std::unique_ptr<WindowManager> ApplicationContext::windowManager = nullptr;

void ApplicationContext::initialize(const char *windowTitle) {
  /* Set up logging */
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
    "log.txt", true);

  auto logger = std::make_shared<spdlog::logger>(
    "multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
  spdlog::set_default_logger(logger);

  /* Initialize components*/
  ecs = std::make_unique<flecs::world>();
  assetManager = std::make_unique<AssetManager>();
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