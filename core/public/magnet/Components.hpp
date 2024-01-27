#pragma once

#include <magnet/AudioManager.hpp>

#include <array>

#include <cglm/cglm.h>
namespace Magnet::Components {

/* Scene Components */

/**
 * @struct Transform
 * @brief An entity's transformation relative to its parent.
 */
struct Transform {
  vec3 translation{0.0f, 0.0f, 0.0f};
  versor rotation{0.0f, 0.0f, 0.0f, 1.0f};
  vec3 scale{1.0f, 1.0f, 1.0f};
};

/* RENDERING */

struct Skybox {
  uint32_t textureID;  // ID of the skybox texture.
};

/**
 * @struct Camera
 * @brief Describes a virtual camera with it's own view frustum.
 */
struct Camera {
  float fieldOfView = 60.0f;         // Angle defining visible scene extent.
  float aspectRatio = 16.0f / 9.0f;  // Width-to-height ratio.
  float nearPlane = 0.1f;            // Distance to near clipping plane.
  float farPlane = 100.0f;           // Distance to far clipping plane.
};

struct MeshRenderer {
  uint32_t vao;
  uint32_t vbo;
  uint32_t ebo;
  uint32_t numIndices;
};

/* AUDIO */


struct AudioSource {
  std::array<std::optional<SpatialAudioRequest>, 8> requests;

  std::optional<size_t> play_sound(
    const char* name, AudioTag tag = AudioTag::NONE, 
    bool looping = false, float volume = 1.f
  );
  SpatialAudioRequest* getRequest(size_t requestId);
  bool isRequestIdValid(size_t requestId);
};

struct AudioListener {
  uint32_t padding = 0; 
  // A component can't have zero size in flecs
};


/* PHYSICS */

struct Collider {
  // TODO: Add collider types
};


// TODO: This.
// Requires Transform, Collider.
struct Rigidbody {
  bool fixed;
};

} // namespace Magnet::Components