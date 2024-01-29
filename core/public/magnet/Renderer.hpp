#pragma once

// Renderer.hpp
// Manages the rendering pipeline

#include <cstdint>

namespace Magnet {

class Renderer {
  int width, height;

  Renderer();
  Renderer(const Renderer&) = delete;
  void operator=(const Renderer&) = delete;
 public:
  static Renderer& getInstance() {
    static Renderer instance;
    return instance;
  }
  void drawFrame() const;
};

}  // namespace Magnet