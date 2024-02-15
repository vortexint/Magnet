#pragma once

#include <vector>
#include <optional>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <span>
#include <array>

#include <magnet/AudioManager.hpp>
#include <magnet/Components.hpp>

struct ALCdevice;
struct ALCcontext;

namespace Magnet::Components {
struct Cone;
}

namespace Magnet {
struct ALBuffer;
struct ALEffect;
struct ALAuxiliaryEffectSlot;


struct ALSource {
  static std::optional<ALSource> create();
  uint32_t source = 0;
  
  bool isStopped();
  void setLooping(bool);
  void stop();
  void reset();
  void setBuffer(ALBuffer buffer);
  void play();
  void link(ALAuxiliaryEffectSlot, uint32_t sendIndex);
  void unlink(uint32_t sendIndex);

  void setPosition(const vec3);
  void setRotationVersor(const versor);
  void setVolume(float);
  void setPitch(float);
  void setCone(const Components::Cone &);

  void destroy();
};



struct ALBuffer {
  static std::optional<ALBuffer> create(std::span<const uint8_t> cBytes,
                                        AudioFormat audioFormat,
                                        size_t sampleRate);
  static std::optional<ALBuffer> createMonoBuffer(std::span<const uint8_t> cBytes,
                                                  AudioFormat audioFormat,
                                                  size_t sampleRate);
  uint32_t buffer = 0;

  void destroy();
};
/*
  An effect slot allows multiple ALSource to reuse the same effect.
  First assign an effect to the effect slot. There is only one effect per slot.
  Next link the source with the effect slot.
  A source can have a limited number of slots linked to it (ex: 2).
 */
struct ALAuxiliaryEffectSlot {
  static std::optional<ALAuxiliaryEffectSlot> create(bool shouldLog = true);
  void assign(ALEffect);
  void clear();
  uint32_t slot = 0;

  
  void destroy();
};
struct ALFilter {
  static std::optional<ALFilter> create();
  uint32_t filter = 0;

  
  void destroy();
};

struct ALEffect {
  static std::optional<ALEffect> create();
  static std::optional<ALEffect> create(EffectDescription);
  uint32_t effect = 0;

  void setReverb(ReverbDescription reverb);

  void destroy();
};

struct ALAudioRequest {
  static constexpr int ENVIRONMENTAL_EFFECT_SEND = 0;

  uint32_t requestId;
  ALSource source;
  ALFilter filter;

  std::optional<Components::AudioSource> oldAudioSource;
  std::optional<Components::Transform> oldTransform;

  std::optional<uint32_t> environmentalEffectSlotId;

  static std::optional<ALAudioRequest> create(uint32_t requestId);

  void destroy();
};


class ALBackend {
  ALCdevice *device;
  ALCcontext *context;

  static int m_MAX_CHANNELS;
  static int m_MAX_SLOTS;
  static int m_MAX_SLOTS_PER_CHANNEL;

  std::vector<ALAudioRequest> requests;
  std::unordered_set<uint32_t> freeRequestIds;
  std::unordered_set<uint32_t> usedRequestIds;

  // TODO: Make the values in all these maps not be pointers
  std::unordered_map<std::string, ALBuffer> buffers;
  std::unordered_map<std::string, ALBuffer> monoBuffers;
  std::unordered_map<std::string, ALEffect> effects;


  std::vector<ALAuxiliaryEffectSlot> slots;
  std::vector<int32_t> slotReferenceCounts;
  std::unordered_map<std::string, uint32_t> effectsToSlots;
  std::unordered_set<uint32_t> freeSlots;
  std::unordered_set<uint32_t> usedSlots;

 public:
  static int MAX_CHANNELS();
  static int MAX_SLOTS();
  static int MAX_SLOTS_PER_CHANNEL();

  std::optional<uint32_t> incrementOrCreateEffectSlot(const std::string& effectName);
  void decrementEffectSlot(uint32_t);

  ALBackend();
  ~ALBackend();

  ALBackend(const ALBackend &) = delete;
  ALBackend &operator=(const ALBackend &) = delete;

  /*
    OpenAL can only have a certain amount of effects active at one time
    Each frame 
   */
  
  void setParameters(uint32_t requestId, Components::AudioSource &,
                     const Components::Transform &,
                     const Components::Environment &, const VolumeAdjuster &);

  void registerEffect(const std::string &name, EffectDescription);

  void genBuffers(const std::string&, std::span<const uint8_t>, AudioFormat, size_t sampleRate);
  std::optional<ALBuffer> getMonoBuffer(const std::string&);
  std::optional<ALBuffer> getBuffer(const std::string&);

  std::optional<uint32_t> newRequest();
  void freeRequest(uint32_t);
  ALAudioRequest* getRequest(uint32_t);
  void setEnvironmentalEffect(uint32_t, const std::string&);
  void clearEnvironmentalEffect(uint32_t);

  static void setListenerPos(vec3 pos);
  static void getListenerPos(vec3 pos);
};

  
}  // namespace Magnet