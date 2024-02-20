#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <magnet/Application.hpp>
#include <magnet/ArchiveManager.hpp>
#include <magnet/AudioManager.hpp>
#include <magnet/Scene.hpp>
#include <magnet/Shaders.hpp>
#include <magnet/Widgets.hpp>

#include "GFX/Pipeline.hpp"
#include "GFX/UI/UserInterface.hpp"
#include "GFX/Window.hpp"

namespace Magnet::Application {

spdlog::logger logger("Magnet");
ArchiveManager archiveManager(ARCH_core, ARCH_core_KEY);
Context* appContext;

ArchiveManager& getArchiveManager() { return archiveManager; }

void registerContext(Context& applicationContext, const char* const title) {
  appContext = &applicationContext;
  Window::initialize(applicationContext, title);
  Rendering::setupPipeline();
}

void setupLogging() {
#if defined(DEBUG)
  static auto consoleSink =
    std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_level(spdlog::level::debug);
  logger.sinks().push_back(consoleSink);
#endif

  static auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
    DEFAULT_LOG_FILENAME, true);
  logger.sinks().push_back(fileSink);

  spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
}

void initializeSystems() {
  Scene::setupECS(appContext->getECS());
  auto& ecs = appContext->getECS();
  AudioManager::getInstance().setupECS(&ecs);
  UI::setup(archiveManager);
  appContext->init();
}

void initialize() {
  if (appContext == nullptr)
    logger.critical("Application context not registered!");

  setupLogging();
  initializeSystems();

  /* Main loop */

  while (!glfwWindowShouldClose(appContext->getWindow())) {
    glfwPollEvents();

    /* Context-specific */

    // Time calculations (delta time)
    Time::update(appContext->getTimeState());

    // Update all ECS systems
    Scene::progressECS(appContext);

    UI::newFrame();

    Widgets::DevTools(appContext);

    appContext->update();

    Rendering::updatePipeline(appContext->getWindow());
  }
  UI::shutdown();
  Shaders::cleanup();

  glfwDestroyWindow(appContext->getWindow());
  glfwTerminate();
}

}  // namespace Magnet::Application