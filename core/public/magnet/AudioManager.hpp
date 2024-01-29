#pragma once
#include <cglm/cglm.h>
#include <flecs.h>

#include <magnet/AssetManager.hpp>
#include <magnet/SceneManager.hpp>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct ALCdevice;
struct ALCcontext;

namespace Magnet {
struct SpatialAudioChannel;

namespace Components {
  struct AudioSource;
  struct AudioListener;
  struct Transform;
}

struct GlobalAudioListener {
  static void setPosition(vec3);
  static void setVelocity(vec3);
  static void set_orientation_versor(versor v);
  static void set_orientation_forward_up(vec3 forward, vec3 up);

  // AL_METERS_PER_UNIT
};

enum class AudioFormat {
  AUDIO_FORMAT_MONO8,
  AUDIO_FORMAT_MONO16,
  AUDIO_FORMAT_STEREO8,
  AUDIO_FORMAT_STEREO16
};

struct AudioBuffer {
  uint32_t originalBuffer = 0;
  uint32_t monoBuffer = 0;
  uint64_t samples = 0;
  uint64_t sampleRate = 0;

  static std::optional<AudioBuffer> create(std::span<uint8_t> bytes,
                                           AudioFormat, size_t samples,
                                           size_t sampleRate);
  static std::optional<uint32_t> createMonoBuffer(std::span<uint8_t> bytes,
                                                  AudioFormat, size_t samples,
                                                  size_t sampleRate);
  void destroy() const;
  double length() const;
};

enum class AudioTag {
  VOICE = 0,
  SOUND_EFFECTS = 1,
  MUSIC = 2,
  NONE,
};

struct SpatialAudioChannel;

struct Cone {
  float angleDeg = 0.f;
  float outerGain = 0.f;
};

struct AudioChannel {
  uint32_t source = 0;

  static std::optional<AudioChannel> create();
  /*
    Stops the current track and changes the AudioBuffer for the channel
    There can only be one audio buffer per channel
  */
  virtual void assign(const AudioBuffer&);
  void play();
  bool isCurrentlyPlaying();
  void pause();
  void stop();
  // Resets the channel to have no settings applied to it
  // also stops the audio
  void reset();
  bool isStopped();
  bool isLooping();
  void setLooping(bool);
  void destroy() const;
  void setVolume(float vol = 1.f);
  void setPitch(float pitch = 1.f);

  void checkIfInitialized() const;
};

struct AudioFilter;

struct SpatialAudioChannel {
  uint32_t source = 0;

  bool operator==(const AudioChannel& rhs) const noexcept;
  bool operator!=(const AudioChannel& rhs) const noexcept;
  static std::optional<SpatialAudioChannel> create();

  void play();
  bool isCurrentlyPlaying();
  void pause();
  void stop();
  // Resets the channel to have no settings applied to it
  // also stops the audio
  void reset();
  bool isStopped();
  bool isLooping();
  void setLooping(bool);
  void destroy() const;
  void setVolume(float vol = 1.f);
  void setPitch(float pitch = 1.f);

  void assign(const AudioBuffer&);
  void setPosition(vec3 pos);
  void setVelocity(vec3 vel);
  void setDirection(vec3 dir);

  void checkIfInitialized() const;

  /*
    Creates a cone pointing in the previously set direction,
    with an angle of angleRad in radians
    The outerVolume sets how much a listener would hear if they
    were outside the cone
  */
  void setCone(float angleDeg = 180.f, float outerVolume = 0.f);


  void setFilter(AudioFilter&);
};

struct AudioFilter;

struct SpatialAudioRequest {
  const char* trackName = nullptr;
  AudioTag tag = AudioTag::NONE;
  double timestampStartedS = 0;
  float volume = 1.f;
  float pitch = 1.f;
  std::optional<SpatialAudioChannel> channel;
  std::optional<Cone> coneSettings;
  bool looping = false;
  bool user_requests_stop = false;
  std::optional<AudioFilterDescription> filterDescription;
  std::optional<AudioFilter> filter;

  size_t hash() const;
  void stop();
  void setPitch(float pitch = 1.f);
  void setVolume(float volume = 1.f);
  float getPitch() const;
  float getVolume() const;
  std::optional<SpatialAudioChannel>& getChannel();

  void setFilter(AudioFilter&);


  // AL_AIR_ABSORBTION_FACTOR
  // AL_ROOM_ROLLOFF_FACTOR
  // AL_CONE_OUTER_GAINHF
  // AL_DIRECT_FILTER_GAINHF_AUDIO
  // AL_AUXILIARY_SEND_FILTER_GAIN_AUTO
  // AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO
  // 
};

struct AudioTagParameters {
  float volume = 1.f;
};

enum class AudioFilterType {
  LOWPASS,
  HIGHPASS,
  BANDPASS
};

struct AudioFilterDescription {
  AudioFilterType filterType;
  std::optional<float> gain;
  std::optional<float> gainHighFrequency;
  std::optional<float> gainLowFrequency;
};

struct AudioFilter {
  uint32_t filter = 0;

  static std::optional<AudioFilter> create(AudioFilterType, std::optional<AudioFilterDescription>);
  void destroy();

  void setGain(float);
  /*
    Sets the volume of sound in high frequency
    Only can be used with low or band pass filter
  */
  void setGainHighFrequency(float);
  /*
    Set the volume of sound in low frequency
    Can only be used with high or band pass filter
  */
  void setGainLowFrequency(float);

  AudioFilterType filterType();
};

struct AudioEffect {
  uint32_t effect = 0;

  std::optional<AudioEffect> create();
  void destroy();

  // AL_EFFECT_NULL
  // AL_EFFECT_EAXREVERB
  // AL_EFFECT_REVERB
  // AL_EFFECT_CHORUS
  // AL_EFFECT_DISTORTION
  // AL_EFFECT_ECHO
  // AL_EFFECT_FLANGER
  // AL_EFFECT_FREQUENCY_SHITER
  // AL_EFFECT_VOCAL_MORPHER
  // AL_EFFECT_PITCH_SHIFTER
  // AL_EFFECT_RING_MODULATOR
  // AL_EFFECT_AUTOWAH
  // AL_EFFECT_COMPRESSOR
  // AL_EFFECT_EQUALIZER
};

struct AuxiliaryEffectSlot {
  uint32_t slot;

  std::optional<AuxiliaryEffectSlot> create();
  void destroy();

  // TODO: Implement AL_AUXILIARY_SEND_FILTER

  // TODO: Check the maximum number of auxiliary sends
  // ALC_MAX_AUXILIARY_SENDS


};

class AudioManager {
  AssetManager* assetManager;
  std::unordered_map<std::string, AudioBuffer> tracks;

  AudioChannel background;
  std::unordered_set<uint32_t> freeSpatialAudioChannels;
  std::unordered_set<uint32_t> borrowedSpatialAudioChannels;

  std::unordered_map<AudioTag, AudioTagParameters> tagModifier;
  AudioTagParameters masterTagModifier;

  ALCdevice* audioDevice;
  ALCcontext* alContext;

  int32_t MAX_CHANNELS = 0;
  int32_t AUDIO_CHANNELS = 0;
  int32_t SPATIAL_AUDIO_CHANNELS = 0;

  static void InitializeOpenAL_EFX(ALCdevice*);
public:
  static AudioManager& getInstance() {
    static AudioManager audioManager;
    return audioManager;
  }

  AudioManager();
  ~AudioManager();

  AudioManager(const AudioManager&) = delete;
  AudioManager& operator=(const AudioManager&) = delete;

  std::optional<SpatialAudioChannel> borrowChannel();
  void returnChannel(SpatialAudioChannel channel);

  std::optional<AudioBuffer> getTrack(const char* track);

  void playTrackBackground(const char* track);
  void deleteTrack(const char* track);

  static void updateListener();

  static void AudioSourceSystem(flecs::iter&, Components::Transform*,
                                Components::AudioSource*);


  AudioTagParameters& getTagModifier(AudioTag);
  AudioTagParameters& getMaster() { return masterTagModifier; }


  static void AudioSourceSystem(flecs::iter&, Components::Transform*, Components::AudioSource*);
  static void AudioListenerSystem(flecs::iter&, Components::Transform*, Components::AudioListener*);
};

std::string to_string(Magnet::AudioTag);
}  // namespace Magnet