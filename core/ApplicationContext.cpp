#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/ShaderManager.hpp>
#include <magnet/WindowManager.hpp>
#include <magnet/Renderer.hpp>
#include <magnet/InputManager.hpp>
#include <magnet/Time.hpp>

namespace Magnet {

std::unique_ptr<SceneManager> ApplicationContext::sceneManager = nullptr;
std::unique_ptr<AssetManager> ApplicationContext::assetManager = nullptr;
std::unique_ptr<WindowManager> ApplicationContext::windowManager = nullptr;
std::unique_ptr<InputManager> ApplicationContext::inputManager = nullptr;
std::unique_ptr<Renderer> ApplicationContext::renderer = nullptr;

ProjectInterface *registeredInterface = nullptr;

void ApplicationContext::registerInterface(ProjectInterface *interface) {
  registeredInterface = interface;
}

void ApplicationContext::initialize(const char *windowTitle) {
  /* Logging */
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("run.log", true);
  auto logger = std::make_shared<spdlog::logger>(
    "multi_sink", spdlog::sinks_init_list{consoleSink, fileSink});
  spdlog::set_default_logger(logger);

  /* Initialization */

  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW");
  }

  sceneManager = std::make_unique<SceneManager>();
  assetManager = std::make_unique<AssetManager>(ARCH_core, ARCH_core_KEY);
  windowManager = std::make_unique<WindowManager>(windowTitle);
  inputManager = std::make_unique<InputManager>();
  renderer = std::make_unique<Renderer>();

  registeredInterface->init();

  /* Main loop */
  while (!windowManager->shouldClose()) {
    Time::Update();

    /* Poll for and process events */
    windowManager->pollEvents();

    /* Update all active systems and subsystems */
    sceneManager->progress(Time::getDelta());

    registeredInterface->update();

    /* Draw everything */
    renderer->render();

    windowManager->swapBuffers();
  }
}

}  // namespace Magnet