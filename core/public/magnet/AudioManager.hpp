#pragma once

#include <optional>
#include <memory>
#include <span>
#include <vector>
#include <string>
#include <unordered_map>

#include <cglm/cglm.h>

#include <flecs.h>

#include <magnet/Components.hpp>

namespace Magnet::Components {
struct AudioSource;
struct Transform;
struct Environment;
}  // namespace Magnet::Components
namespace flecs {
struct world;
}

namespace Magnet {
struct ALBuffer;
struct ALBackend;
struct ALAudioRequest;

struct ReverbDescription {
  float density;
  float diffusion;
  float gain;
  float gainHF;
  float gainLF;
  float decayTime;
  float decayHFRatio;
  float decayLFRatio;
  float reflectionsGain;
  float reflectionsDelay;
  vec3 reflectionsPan;
  float lateReverbGain;
  float lateReverbDelay;
  vec3 lateReverbPan;
  float echoTime;
  float echoDepth;
  float modulationTime;
  float modulationDepth;
  float airAbsorptionGainHF;
  float hFReference;
  float lFReference;
  float roomRolloffFactor;
  int   decayHFLimit;
};

enum class EffectType { REVERB };

struct EffectDescription {
  EffectType type;
  union {
    ReverbDescription reverb;
  };
  EffectDescription(ReverbDescription);
};

enum class AudioFormat {
  AUDIO_FORMAT_MONO8,
  AUDIO_FORMAT_MONO16,
  AUDIO_FORMAT_STEREO8,
  AUDIO_FORMAT_STEREO16
};
size_t FormatBytesPerSample(AudioFormat format);

struct RawAudioData {
  AudioFormat format;
  std::vector<uint8_t> data;
  size_t sampleRate;

  void clear();
};


struct VolumeAdjuster {
  VolumeAdjuster();

  std::unordered_map<Components::AudioTag, float> volumes;
  float masterVolume;
};

struct AudioManagerDebugInfo {
  uint32_t slotsCount;
  uint32_t usedSlotsCount;
  uint32_t nonZeroSlotReferenceCount;
};

class AudioManager {
  ALBackend *al;
  flecs::world *ecs = nullptr;

  VolumeAdjuster volumeAdjustor;
 public:
  static constexpr float MAX_DISTANCE = 128.f;

  AudioManager();
  ~AudioManager();

  static AudioManager& getInstance() {
    static AudioManager audioMgr;
    return audioMgr;
  }

  void setupECS(flecs::world* ecs);
  VolumeAdjuster& volumes();

  void handleRequestedPlay(Components::AudioSource&, 
                          const Components::Transform&,
                          const Components::Environment&);
  void handlePlaying(Components::AudioSource&, 
                    const Components::Transform&,
                    const Components::Environment&);
  void handleStopped(Components::AudioSource&);

  static void AudioSourceSystem(flecs::iter&, Components::Transform*, Components::AudioSource*);
  
  static bool LoadAudio(std::span<const uint8_t> rawFileData, RawAudioData &);

  static void removeSource(flecs::entity e, Components::AudioSource);

  void registerAudio(const std::string&, std::span<const uint8_t> data, AudioFormat format, size_t sampleRate);
  void registerEffect(const std::string& name, EffectDescription);

  void registerAllEfxPresets();

  static void getListenerPos(vec3 pos);
  static void setListenerPos(const vec3 pos);

  AudioManagerDebugInfo getDebugInfo() const;
};
} // namespace Magnet