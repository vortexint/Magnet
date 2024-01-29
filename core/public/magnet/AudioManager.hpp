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
struct Transform;
}  // namespace Components

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
  bool is_currently_playing();
  void pause();
  void stop();
  // Resets the channel to have no settings applied to it
  // also stops the audio
  void reset();
  bool is_stopped();
  bool is_looping();
  void set_looping(bool);
  void destroy() const;
  void set_volume(float vol = 1.f);
  void set_pitch(float pitch = 1.f);

  void check_if_initialized() const;
};

struct SpatialAudioChannel {
  uint32_t source = 0;

  bool operator==(const AudioChannel& rhs) const noexcept;
  bool operator!=(const AudioChannel& rhs) const noexcept;
  static std::optional<SpatialAudioChannel> create();

  void play();
  bool is_currently_playing();
  void pause();
  void stop();
  // Resets the channel to have no settings applied to it
  // also stops the audio
  void reset();
  bool is_stopped();
  bool is_looping();
  void set_looping(bool);
  void destroy() const;
  void set_volume(float vol = 1.f);
  void set_pitch(float pitch = 1.f);

  void assign(const AudioBuffer&);
  void set_position(vec3 pos);
  void set_velocity(vec3 vel);
  void set_direction(vec3 dir);

  void check_if_initialized() const;

  /*
    Creates a cone pointing in the previously set direction,
    with an angle of angleRad in radians
    The outerVolume sets how much a listener would hear if they
    were outside the cone
  */
  void set_cone(float angleDeg = 180.f, float outerVolume = 0.f);
};

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

  size_t hash() const;
  void stop();
  void setPitch(float pitch = 1.f);
  void setVolume(float volume = 1.f);
  float getPitch() const;
  float getVolume() const;
  std::optional<SpatialAudioChannel>& getChannel();
};

struct AudioTagParameters {
  float volume = 1.f;
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
  std::optional<uint32_t> handleSpatialAudioRequest(SpatialAudioRequest request,
                                                    vec3 pos);

  void playTrackBackground(const char* track);
  void deleteTrack(const char* track);

  static void updateListener();

  static void AudioSourceSystem(flecs::iter&, Components::Transform*,
                                Components::AudioSource*);


  AudioTagParameters& getTagModifier(AudioTag);
  AudioTagParameters& getMaster() { return masterTagModifier; }
};

std::string to_string(Magnet::AudioTag);
}  // namespace Magnet