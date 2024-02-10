#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <magnet/Application.hpp>
#include <magnet/ArchiveManager.hpp>
#include <magnet/Scene.hpp>
#include <magnet/Shaders.hpp>

#include "GFX/Pipeline.hpp"
#include "GFX/UI/UserInterface.hpp"
#include "GFX/WindowManager.hpp"

namespace Magnet {

ArchiveManager ApplicationContext::archiveManager(ARCH_core, ARCH_core_KEY);

ApplicationContext::ApplicationContext(ProjectInterface& interface)
  : logger("magnet"), registeredInterface(interface) {
  /* Logging */

#if defined(DEBUG)
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_level(spdlog::level::debug);
  logger.sinks().push_back(consoleSink);
#endif

  auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("output.log", true);
  logger.sinks().push_back(fileSink);

  // set default logger
  spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
}

void ApplicationContext::initialize(const char* windowTitle) {
  Window::initialize(windowTitle);
  Rendering::setupPipeline(archiveManager);
  Scene::setupECS();
  UI::setup(archiveManager);

  registeredInterface.init(*this);

  /* Main loop */
  while (!Window::shouldClose()) {
    /* Poll for and process events */
    Window::pollEvents();

    /* Update time calculations */
    Time::update(timeState);

    /* Update all ECS systems */
    Scene::progressECS(timeState);

    UI::newFrame();

    registeredInterface.update(*this);

    /* Draw everything */
    Rendering::drawFrame();
    UI::draw();

    Window::swapBuffers();
  }
  UI::shutdown();
  Shaders::cleanup();
  Window::shutdown();
}

ArchiveManager& ApplicationContext::getArchiveManager() {
  return archiveManager;
}

const Time::TimeState& ApplicationContext::getTimeState() const {
  return timeState;
}

}  // namespace Magnet