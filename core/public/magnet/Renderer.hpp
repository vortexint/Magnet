#pragma once
/**
 * @file Renderer.hpp
 * @brief GL rendering component
 */

#include <cstdint>

#include <magnet/SceneManager.hpp>

namespace Magnet {

// Used by Window Manager
const uint32_t INIT_WIDTH = 1280;
const uint32_t INIT_HEIGHT = 720;
class Renderer {
  flecs::world* ecs;
  flecs::entity activeCamera;

  int width = INIT_WIDTH;
  int height = INIT_HEIGHT;

 public:
  Renderer();

  void render();

  /* Setters */

  void setActiveCamera(flecs::entity cameraEntity);
  void setSize(int width, int height);
};

} // namespace Magnet