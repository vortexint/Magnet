#pragma once

// Viewport.hpp
// Decoupled from renderer, interacts with GL, ECS.

#include <glad/glad.h>
#include <flecs.h>

struct GLFWwindow;

namespace Magnet {

// Viewport monostate
class Viewport {
  static flecs::entity activeCamera;
  static int width, height;

  Viewport() = delete;
  Viewport(const Viewport&) = delete;
  void operator=(const Viewport&) = delete;

  friend class AudioManager;
 public:
  static void updateFrustum(GLuint baseShaderID);
  
  static void setSize(GLFWwindow* window, int width, int height);
  static void setActiveCamera(flecs::entity camera);

  static flecs::entity getActiveCamera() { return activeCamera; }
};

}  // namespace Magnet