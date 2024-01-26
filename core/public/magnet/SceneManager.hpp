#pragma once

#include <flecs.h>

#include <memory>
namespace Magnet {

class SceneManager {
  std::unique_ptr<flecs::world> ecs;
  flecs::entity activeCamera;
  SceneManager();
  SceneManager(const SceneManager&) = delete;
  void operator=(const SceneManager&) = delete;

 public:
  static SceneManager& getInstance() {
    static SceneManager instance;
    return instance;
  }
  flecs::world& getECS() { return *ecs;  }

  void progress();

  void setActiveCamera(flecs::entity camera);
};
}  // namespace Magnet