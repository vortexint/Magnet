#pragma once

#include <flecs.h>
#include <memory>

namespace Magnet {

class SceneManager {
  std::unique_ptr<flecs::world> ecs;
public:
  SceneManager();
  void progress();
};

} // namespace Magnet