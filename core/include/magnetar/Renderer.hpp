#pragma once

#include "Components.hpp"
#include <flecs.h>

using namespace magnetar::components;

class Renderer {
  void RenderEntity(Transform &transform, MeshRenderer &render);

public:
  Renderer(flecs::world &ecs);

  void SetActiveCamera(flecs::entity cameraEntity);
};