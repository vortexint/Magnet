#pragma once

#include <optional>
#include <span>
#include <string>

#include <cglm/cglm.h>

// TODO: Remove when asset system is finished
namespace Magnet {
struct ALBuffer;
}

namespace Magnet::Components {

/* Scene Components */

/**
 * @struct Transform
 * @brief An entity's transformation relative to its parent.
 */
struct Transform {
  vec3   position{0.0f, 0.0f, 0.0f};
  versor rotation{0.0f, 0.0f, 0.0f, 1.0f};
  vec3   scale{1.0f, 1.0f, 1.0f};
};

bool operator==(const Transform&, const Transform&);
bool operator!=(const Transform&, const Transform&);

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


struct Environment {
  std::optional<const char*> effectName;
};

struct Cone {
  float angleDeg = 0.f;
  float outerGain = 0.f;
  
};
bool operator==(const Cone&, const Cone&);
bool operator!=(const Cone&, const Cone&);



enum class FilterType { LOWPASS, HIGHPASS, BANDPASS };

struct Filter {
  FilterType type = FilterType::BANDPASS;
  float gain = 1.f;
  float gainHighFrequency = 1.f;
  float gainLowFrequency = 1.f;
};
bool operator==(const Filter&, const Filter&);
bool operator!=(const Filter&, const Filter&);


enum class AudioTag { VOICE, SOUND_EFFECTS, MUSIC, NONE };
std::optional<AudioTag> from_string(const std::string&);
std::string to_string(AudioTag);
std::span<const AudioTag> allAudioTags();


enum class AudioPlayState { REQUESTED_PLAY, PLAYING, STOPPED };

struct AudioSource {
  const char* trackName = nullptr;
  ALBuffer* audioBuffer = nullptr;
  std::optional<uint32_t> requestId = std::nullopt;
  bool isSpatial = true;
  // TODO: Make the previous 4 members private

  AudioPlayState state = AudioPlayState::STOPPED;
  AudioTag tag = AudioTag::NONE;
  float volume = 1.f;
  float pitch = 1.f;
  std::optional<Cone> cone = std::nullopt;
  bool looping = false;
  std::optional<uint32_t> effectId = std::nullopt;
  std::optional<Filter> filter;

  void playSound(const char* trackName);
  void stop();
};

bool operator==(const AudioSource&, const AudioSource&);
bool operator!=(const AudioSource&, const AudioSource&);


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