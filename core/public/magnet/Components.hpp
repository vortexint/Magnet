#pragma once

#include <cglm/cglm.h>

#include <magnet/AudioManager.hpp>  // TODO: Remove this dependency, AudioManager should be hidden from the public API
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
  uint32_t textureID;  // ID of the skybox texture.
};

/**
 * @struct Camera
 * @brief Describes a virtual camera and it's view frustum.
 */
struct Camera {
  float fieldOfView = 60.0f;             // Angle defining visible scene extent
  vec2 clippingPlanes = {0.1f, 100.0f};  // Near and far clipping planes
  vec3 up{0.0f, 1.0f, 0.0f};
};

struct MeshRenderer {
  uint32_t vao;
  uint32_t vbo;
  uint32_t ebo;
  uint32_t numIndices;
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

class AudioSource {
  friend void AudioManager::AudioSourceSystem(flecs::iter& iter, 
                                              Transform* transforms,
                                              AudioSource* sources);
  AudioSourcePlayState playState;
  std::optional<SpatialAudioChannel> channel;
  std::optional<AudioFilterDescription> filterDesc;
  std::optional<AudioFilter> filter;
  std::optional<EAXReverbDescription> reverb;
  std::optional<AudioEffect> effect;

  AudioBuffer* audioBuffer = nullptr;
  const char* trackName = nullptr;
  AudioBuffer *buffer;
  double timestampStartedS = 0.0;
public:
  AudioTag tag = AudioTag::NONE;
  float volume = 1.f;
  float pitch = 1.f;
  std::optional<Cone> cone;
  bool looping = false;

  void reset();
  AudioSource();

  AudioSourcePlayState getPlayState();
  const char* getTrackName();


  void play_sound(
    const char* name, AudioTag tag = AudioTag::NONE,
    bool looping = false, float volume = 1.f,
    std::optional<AudioFilterDescription> desc = std::nullopt,
    std::optional<EAXReverbDescription> reverb = std::nullopt
  );
  void play_sound(
    AudioBuffer *buffer, AudioTag tag = AudioTag::NONE,
    bool looping = false, float volume = 1.f,
    std::optional<AudioFilterDescription> desc = std::nullopt,
    std::optional<EAXReverbDescription> reverb = std::nullopt
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

}  // namespace Magnet::Components