#pragma once

#include <flecs.h>

#include "Components.hpp"

// forward declarations
struct GLFWwindow;

using namespace magnetar::components;

class Renderer {
  flecs::world* ecs;
  flecs::entity activeCamera;

 public:
  Renderer(flecs::world* ecs);

  void setActiveCamera(flecs::entity cameraEntity);

  static void resizeCallback(GLFWwindow* window, int width, int height);
};