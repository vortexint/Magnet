#pragma once

#include "flecs.h"

class SceneManager {
  flecs::world ecs_;
  flecs::entity activeCamera_;

  void InitializeScene();
public:
  SceneManager();

  void Update();

  flecs::entity GetActiveCamera() const;
};
