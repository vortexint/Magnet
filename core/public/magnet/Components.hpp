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
  vec3   position{0.0f, 0.0f, 0.0f};
  versor rotation{0.0f, 0.0f, 0.0f, 0.0f};
  vec3   scale{1.0f, 1.0f, 1.0f};
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
  vec2 clippingPlanes = {0.1f, 100.0f}; // Near and far clipping planes
  vec3 up{0.0f, 1.0f, 0.0f};
};

struct MeshRenderer {
  uint32_t vao;
  uint32_t vbo;
  uint32_t ebo;
  uint32_t numIndices;
};

/* AUDIO */

enum class AudioSourcePlayState {
  REQUESTED_PLAY,
  PLAYING,
  STOPPED
};

struct AudioSource {
  AudioSourcePlayState playState;
  std::optional<SpatialAudioChannel> channel = std::nullopt;
  const char* trackName = nullptr;
  double timestampStartedS = 0.0;

  void reset();
  AudioSource();

  AudioTag tag = AudioTag::NONE;
  float volume = 1.f;
  float pitch = 1.f;
  std::optional<Cone> cone;
  bool looping = false;


  void play_sound(
    const char* name, AudioTag tag = AudioTag::NONE,
    bool looping = false, float volume = 1.f
  );

  void stop();
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