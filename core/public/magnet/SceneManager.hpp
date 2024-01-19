#pragma once

#include <flecs.h>
#include <memory>

namespace Magnet {

class SceneManager {
  std::unique_ptr<flecs::world> ecs;
  flecs::entity activeCamera;

 public:
  SceneManager();
  void progress(float deltaTime);
  void setActiveCamera(flecs::entity camera);
};

} // namespace Magnet