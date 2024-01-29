#pragma once

#include <flecs.h>

#include <memory>
namespace Magnet {

class SceneManager {
  std::unique_ptr<flecs::world> ecs;

  SceneManager();
  SceneManager(const SceneManager&) = delete;
  void operator=(const SceneManager&) = delete;
 public:
  static SceneManager& getInstance() {
    static SceneManager instance;
    return instance;
  }

  flecs::world& getECS() const { return *ecs; }
  

  void progress();
};
}  // namespace Magnet