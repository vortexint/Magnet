#pragma once

#include <flecs.h>
#include <memory>

namespace Magnet {

class SceneManager {
  std::unique_ptr<flecs::world> ecs;
public:
  SceneManager();
  void progress(float deltaTime);
};

} // namespace Magnet