#pragma once

#include <cglm/cglm.h>

#include <magnet/AudioManager.hpp>  // TODO: Remove this dependency, AudioManager should hide from the public API

/**
 * ECS Component Guideline:
 * Components are data structures only. They represent an entity's state.
 * Do not add methods/logic. Keep them clean and simple.
 */

namespace Magnet::Components {

/* Scene Components */

/**
 * @struct Transform
 * @brief An entity's transformation relative to its parent.
 */
struct Transform {
  vec3 position{0.0f, 0.0f, 0.0f};
  versor rotation{0.0f, 0.0f, 0.0f, 0.0f};
  vec3 scale{1.0f, 1.0f, 1.0f};
};

/* RENDERING */

struct Skybox {
  uint32_t AssetID; // ID of the skybox texture.
};

/**
 * @struct Camera
 * @brief Describes a virtual camera and it's view frustum.
 */
struct Camera {
  float fieldOfView = 60.0f;                     // Angle defining visible scene extent
  vec2 clippingPlanes = {0.1f, 100.0f};  // Near and far clipping planes
  vec3 up{0.0f, 1.0f, 0.0f};
};

struct MeshRenderer {
  uint32_t AssetID; // ID of the mesh to render.
};

struct LightSource {
  vec3 color = {1.f, 1.f, 1.f};
  float intensity = 1.f;

  bool shadows = true;
};

struct SpotLight : public LightSource {
  vec3 direction;
  float angle;
};

struct AreaLight : public LightSource {
  vec3 direction;
  float width, height;
};

/* AUDIO */

enum class AudioSourcePlayState { REQUESTED_PLAY, PLAYING, STOPPED };

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

  void play_sound(const char* name, AudioTag tag = AudioTag::NONE,
                  bool looping = false, float volume = 1.f);

  void stop();
};

/* PHYSICS */

struct Collider {
  // TODO: collider types
};

// Requires Transform, Collider.
struct Rigidbody {
  bool fixed;
  // TODO: This.
};

}  // namespace Magnet::Components