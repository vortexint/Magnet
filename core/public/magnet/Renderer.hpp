#pragma once
/**
 * @file Renderer.hpp
 * @brief GL rendering component
 */

#include <cstdint>
#include <flecs.h>

namespace Magnet {

class Renderer {
  int width, height;
  flecs::entity activeCameraEntity;

  Renderer();
  Renderer(const Renderer&) = delete;
  void operator=(const Renderer&) = delete;
 public:
  static Renderer& getInstance() {
    static Renderer instance;
    return instance;
  }
  void drawFrame() const;

  /* Setters */

  void setViewportSize(int width, int height);
  void setActiveCamera(flecs::entity camera);
};

}  // namespace Magnet