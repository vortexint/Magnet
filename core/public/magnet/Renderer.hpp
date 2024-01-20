#pragma once
/**
 * @file Renderer.hpp
 * @brief GL rendering component
 */

#include <cstdint>

#include <magnet/SceneManager.hpp>

namespace Magnet {
namespace Renderer {
  void initialize();
  void terminate();

  void drawFrame();

  /* Setters */

  void setActiveCamera(flecs::entity cameraEntity);
  void setSize(int width, int height);
} // namespace Renderer

} // namespace Magnet