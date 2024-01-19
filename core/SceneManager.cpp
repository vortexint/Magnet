#include <magnet/SceneManager.hpp>
#include <magnet/Components.hpp>

namespace Magnet {

using namespace Components;

SceneManager::SceneManager() {
  ecs = std::make_unique<flecs::world>();
}

void SceneManager::progress(float deltaTime) {
  ecs->progress(deltaTime);
}

void SceneManager::setActiveCamera(flecs::entity camera) {
  activeCamera = camera;
}

} // namespace Magnet