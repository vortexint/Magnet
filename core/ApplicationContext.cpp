#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/ArchiveManager.hpp>
#include <magnet/Scene.hpp>

#include <GFX/Renderer.hpp>
#include "GFX/UI/UserInterface.hpp"
#include "GFX/WindowManager.hpp"

namespace Magnet {

ProjectInterface* registeredInterface = nullptr;

void ApplicationContext::registerInterface(ProjectInterface* interface) {
  registeredInterface = interface;
}

ApplicationContext::ApplicationContext()
  : archiveManager(ARCH_core, ARCH_core_KEY) {
  /* Logging */
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto fileSink =
    std::make_shared<spdlog::sinks::basic_file_sink_mt>("output.log", true);
  auto logger = std::make_shared<spdlog::logger>(
    "multi_sink", spdlog::sinks_init_list{consoleSink, fileSink});
  spdlog::set_default_logger(logger);
}

void ApplicationContext::initialize(const char* windowTitle) {


  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW");
  }

  WindowManager& windowMgr = WindowManager::getInstance();

  Renderer::setupPipeline(archiveManager);
  Scene::setupECS();
  UI::setup(archiveManager);

  windowMgr.setTitle(windowTitle);
  registeredInterface->init();

  /* Main loop */
  while (!windowMgr.shouldClose()) {
    /* Poll for and process events */
    windowMgr.pollEvents();

    /* Update time calculations */
    Time::update(timeState);

    /* Update all active systems and subsystems */
    Scene::progressECS(timeState);

    UI::newFrame();

    registeredInterface->update();

    /* Draw everything */
    Renderer::drawFrame();
    UI::draw();

    windowMgr.swapBuffers();
  }
  UI::shutdown();
}

ArchiveManager& ApplicationContext::getArchiveManager() { return archiveManager; }

const Time::TimeState& ApplicationContext::getTimeState() const {
  return timeState;
}

}  // namespace Magnet