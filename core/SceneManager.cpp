#include <magnet/Components.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/Time.hpp>

namespace Magnet {
using namespace Magnet::Components;

SceneManager::SceneManager() {
  ecs = std::make_unique<flecs::world>();

  /* Scene setup */
  activeCamera = ecs->entity();
  activeCamera.set<Transform>({}).set<Camera>({});
}

void SceneManager::progress() { ecs->progress(Time::getDelta()); }

void SceneManager::setActiveCamera(flecs::entity camera) {
  activeCamera = camera;
}

}  // namespace Magnet