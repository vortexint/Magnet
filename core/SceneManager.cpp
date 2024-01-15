#include <magnet/SceneManager.hpp>

namespace Magnet {

SceneManager::SceneManager() {
  ecs = std::make_unique<flecs::world>();
}

void SceneManager::progress(float deltaTime) {
  ecs->progress(deltaTime);
}

} // namespace Magnet