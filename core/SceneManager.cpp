#include <magnet/Components.hpp>
#include <magnet/Renderer.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/Time.hpp>

#include <spdlog/spdlog.h>

namespace Magnet {
using namespace Magnet::Components;

SceneManager::SceneManager() {
  ecs = std::make_unique<flecs::world>();
  Renderer& renderer = Renderer::getInstance();

  /* Scene setup */
  flecs::entity camera = ecs->entity("Camera")
                             .add<Camera>()
                             .add<Transform>();

  renderer.setActiveCamera(camera);
}

void SceneManager::progress() { ecs->progress(Time::getDelta()); }

}  // namespace Magnet