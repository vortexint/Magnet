#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/ArchiveManager.hpp>
#include <magnet/Renderer.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/Time.hpp>

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
    std::make_shared<spdlog::sinks::basic_file_sink_mt>("run.log", true);
  auto logger = std::make_shared<spdlog::logger>(
    "multi_sink", spdlog::sinks_init_list{consoleSink, fileSink});
  spdlog::set_default_logger(logger);
}

void ApplicationContext::initialize(const char* windowTitle) {
  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW");
  }

  WindowManager& windowMgr = WindowManager::getInstance();
  SceneManager& sceneMgr = SceneManager::getInstance();
  Renderer& renderer = Renderer::getInstance();
  UI ui;

  windowMgr.setTitle(windowTitle);
  registeredInterface->init();

  /* Main loop */
  while (!windowMgr.shouldClose()) {
    /* Poll for and process events */
    windowMgr.pollEvents();

    /* Update time calculations */
    Time::update();

    /* Update all active systems and subsystems */
    sceneMgr.progress();

    ui.newFrame();

    registeredInterface->update();

    /* Draw everything */
    renderer.drawFrame();
    ui.draw();

    windowMgr.swapBuffers();
  }
  // Deconstruct order (Last In, First Out):
  // UI -> Renderer -> SceneManager -> WindowManager
}

ArchiveManager& ApplicationContext::getArchiveManager() { return archiveManager; }

}  // namespace Magnet