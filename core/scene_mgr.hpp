#pragma once

#include "flecs.h"

class SceneManager {
  flecs::world ecs;
public:
  SceneManager();

  void Update();
};
