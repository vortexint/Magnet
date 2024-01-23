#pragma once
/**
 * @file Renderer.hpp
 * @brief GL rendering component
 */

#include <cstdint>

namespace Magnet {
class Renderer {
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

  static void setViewportSize(int width, int height);
};

}  // namespace Magnet