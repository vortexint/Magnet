#pragma once

#include <optional>
#include <span>
#include <string>

#include <cglm/cglm.h>

#include <magnet/Library.hpp>  // For ID

/**
 * ECS Component Guideline:
 * Components are data structures only. They represent an entity's state.
 * Do not add methods/logic. Keep them clean and simple.
 */
// TODO: Remove when asset system is finished
namespace Magnet {
struct ALBuffer;
class AudioManager;
class ALBackend;
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
  
  
  bool operator==(const Transform&) const = default;
};


/* RENDERING */

struct Skybox {
  Library::ID HDRi;
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
  Library::ID mesh;  // ID of the loaded mesh to render.
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

  bool operator==(const Cone&) const = default;
};



enum class FilterType { LOWPASS, HIGHPASS, BANDPASS };

struct Filter {
  FilterType type = FilterType::BANDPASS;
  float gain = 1.f;
  float gainHighFrequency = 1.f;
  float gainLowFrequency = 1.f;

  bool operator==(const Filter&) const = default;
};

enum class AudioTag { VOICE, SOUND_EFFECTS, MUSIC, NONE };
std::optional<AudioTag> from_string(const std::string&);
std::string to_string(AudioTag);
std::span<const AudioTag> allAudioTags();


enum class AudioPlayState { REQUESTED_PLAY, PLAYING, STOPPED };

class AudioSource {
  friend class Magnet::AudioManager;
  friend class Magnet::ALBackend;

  const char* trackName = nullptr;
  std::optional<uint32_t> requestId = std::nullopt;
  bool mIsSpatial = true;

public:
  AudioPlayState state = AudioPlayState::STOPPED;
  AudioTag tag = AudioTag::NONE;
  float volume = 1.f;
  float pitch = 1.f;
  std::optional<Cone> cone = std::nullopt;
  bool looping = false;
  std::optional<Filter> filter;

  const bool isSpatial() const;

  void playSound(const char* trackName, bool mIsSpatial = true);
  void stop();

  
  bool operator==(const AudioSource&) const = default;
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