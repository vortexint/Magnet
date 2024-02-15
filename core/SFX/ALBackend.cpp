
#include <AL/al.h>
#include <AL/alc.h>

#define AL_ALEXT_PROTOTYPES
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-presets.h>

#include <spdlog/spdlog.h>

#include <SFX/ALBackend.hpp>

#include <magnet/Components.hpp>
#include <magnet/Geometry.hpp>

#include <iostream>

namespace Magnet {

ALint AudioFormatToALint(AudioFormat audioFormat) {
  switch (audioFormat) {
    case AudioFormat::AUDIO_FORMAT_MONO8:
      return AL_FORMAT_MONO8;
      break;
    case AudioFormat::AUDIO_FORMAT_MONO16:
      return AL_FORMAT_MONO16;
      break;
    case AudioFormat::AUDIO_FORMAT_STEREO8:
      return AL_FORMAT_STEREO8;
      break;
    case AudioFormat::AUDIO_FORMAT_STEREO16:
      return AL_FORMAT_STEREO16;
      break;
  }

  assert(false);
  return 0;
}

std::optional<ALSource> ALSource::create() {
  ALSource source;
  alGenSources(1, &source.source);
  if (source.source == 0) {
    auto err = alGetError();
    spdlog::error("Failed to create AL source");
    return std::nullopt;
  }
  return source;
}

void ALSource::destroy() { alDeleteSources(1, &source); }

bool ALSource::isStopped() {
  ALint sourceState = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
  return sourceState == AL_STOPPED;
}
void ALSource::setLooping(bool shouldLoop) {
  ALint alShouldLoop = shouldLoop ? AL_TRUE : AL_FALSE;
  alSourcei(source, AL_LOOPING, alShouldLoop);
}
void ALSource::stop() {
  alSourceStop(source);
}
void ALSource::reset() {
  stop();
  alSourcei(source, AL_BUFFER, 0);
  alSourcef(source, AL_ROLLOFF_FACTOR, 1.f);

  vec4 zero_vec = {0.f, 0.f, 0.f, 0.f};
  alSourcefv(source, AL_POSITION, zero_vec);
  alSourcefv(source, AL_VELOCITY, zero_vec);
  alSourcefv(source, AL_DIRECTION, zero_vec);

  for (size_t sendIndex = 0; sendIndex < ALBackend::MAX_SLOTS_PER_CHANNEL(); ++sendIndex) {
    alSource3i(source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, sendIndex,
               AL_FILTER_NULL);
  }

  setLooping(false);
}
void ALSource::setBuffer(ALBuffer buffer) {
  if (!isStopped()) {
    stop();
  }
  alSourcei(source, AL_BUFFER, buffer.buffer);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to assign AL source to buffer");
  }
}
void ALSource::play() {
  alSourcePlay(source);
  
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to play track");
  }
}
void ALSource::link(ALAuxiliaryEffectSlot slot, uint32_t sendIndex) {
  assert(sendIndex < ALBackend::MAX_SLOTS_PER_CHANNEL());
  alSource3i(source, AL_AUXILIARY_SEND_FILTER, slot.slot, sendIndex,
             AL_FILTER_NULL);
}
void ALSource::unlink(uint32_t sendIndex) {
  assert(sendIndex < ALBackend::MAX_SLOTS_PER_CHANNEL());
  alSource3i(source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, sendIndex,
             AL_FILTER_NULL);
}
void ALSource::setPosition(const vec3 pos) {
  alSourcefv(source, AL_POSITION, pos);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::warn("Setting position failed");
  }
}
void ALSource::setRotationVersor(const versor v) {
  versor vCopy = {v[0], v[1], v[2], v[3]};

  vec3 forward = {};
  GET_FORWARD(forward);

  vec3 up = {};
  GET_UP(up);
  glm_quat_rotatev(vCopy, up, up);
  glm_quat_rotatev(vCopy, forward, forward);

  float orientation[] = {forward[0], forward[1], forward[2],
                         up[0],      up[1],      up[2]};

  alSourcefv(source, AL_ORIENTATION, orientation);
}
void ALSource::setVolume(float volume) {
  alSourcef(source, AL_GAIN, volume);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::warn("Setting volume failed");
  }
}
void ALSource::setPitch(float pitch) {
  alSourcef(source, AL_PITCH, pitch);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::warn("Setting pitch failed");
  }
}
void ALSource::setCone(const Components::Cone &cone) {
  alSourcef(source, AL_CONE_INNER_ANGLE, cone.angleDeg);
  alSourcef(source, AL_CONE_OUTER_GAIN, cone.outerGain);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::warn("Setting cone failed");
  }
}

std::optional<ALBuffer> ALBuffer::create(std::span<const uint8_t> bytes,
                                         AudioFormat audioFormat,
                                         size_t sampleRate) {
  ALenum alFromat = AudioFormatToALint(audioFormat);

  ALBuffer buffer;
  alGenBuffers(1, &buffer.buffer);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to create buffer");
    return std::nullopt;
  }
  alBufferData(buffer.buffer, alFromat, bytes.data(), bytes.size_bytes(), sampleRate);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to fill buffer with data");
    alDeleteBuffers(1, &buffer.buffer);
    return std::nullopt;
  }
  return buffer;
}
std::optional<ALBuffer> ALBuffer::createMonoBuffer(std::span<const uint8_t> bytes,
                                                     AudioFormat audioFormat,
                                                     size_t sampleRate) {
  if (audioFormat == AudioFormat::AUDIO_FORMAT_MONO8 ||
    audioFormat == AudioFormat::AUDIO_FORMAT_MONO16) {
    return ALBuffer::create(bytes, audioFormat, sampleRate);
  }

  ALenum alFormat = AudioFormatToALint(audioFormat);
  std::vector<uint8_t> copyBuffer(bytes.begin(), bytes.end());
  if (audioFormat == AudioFormat::AUDIO_FORMAT_STEREO8) {
    uint8_t *stereoBytes = copyBuffer.data();
    for (size_t i = 0; i < bytes.size_bytes() / (2 * sizeof(uint8_t)); ++i) {
      stereoBytes[i] = (stereoBytes[2 * i] + stereoBytes[2 * i + 1]) / 2;
    }
    
    alFormat = AudioFormatToALint(AudioFormat::AUDIO_FORMAT_MONO8);
  } else if (audioFormat == AudioFormat::AUDIO_FORMAT_STEREO16) {
    // It is important that the type is int16_t instead of uint16_t
    short *stereoBytes =
      static_cast<short *>(static_cast<void *>(copyBuffer.data()));
    for (size_t i = 0; i < bytes.size_bytes() / (2 * sizeof(short)); ++i) {
      stereoBytes[i] = (stereoBytes[2 * i] + stereoBytes[2 * i + 1]) / 2;
    }

    alFormat = AudioFormatToALint(AudioFormat::AUDIO_FORMAT_MONO16);
  } else {
    assert(false);
  }

  size_t monoBufferByteSize = bytes.size() / 2;
  
  ALBuffer buffer;
  alGenBuffers(1, &buffer.buffer);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to create buffer");
    return std::nullopt;
  }
  alBufferData(buffer.buffer, alFormat, copyBuffer.data(), monoBufferByteSize, sampleRate);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to fill buffer with data");
    alDeleteBuffers(1, &buffer.buffer);
    return std::nullopt;
  }
 
  return buffer;
}
void ALBuffer::destroy() { alDeleteBuffers(1, &buffer); }

std::optional<ALAuxiliaryEffectSlot> ALAuxiliaryEffectSlot::create(bool shouldLog) {
  ALAuxiliaryEffectSlot slot = {};
  alGenAuxiliaryEffectSlots(1, &slot.slot);
  
  if (alGetError() != AL_NO_ERROR) {
    /*
      OpenAL has a maximum number of Auxiliary effect slots you can
      create. To find this out you just keep creating slots till it fails.
      The problem is that when it fails it logs and error.
      This would just be noise in a regular application so the shouldLog
      parameter just makes it fail quietly instead of annoying everyone.
    */ 
    if (shouldLog) {
      spdlog::error("Failed to create OpenAL EFX AuxiliaryEffectSlot");
    }
    return std::nullopt;
  }

  return slot;
}
void ALAuxiliaryEffectSlot::assign(ALEffect effect) {
  alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect.effect);
}
void ALAuxiliaryEffectSlot::clear() {
  alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
}
void ALAuxiliaryEffectSlot::destroy() {
  alDeleteAuxiliaryEffectSlots(1, &slot);
}
std::optional<ALFilter> ALFilter::create() {
  ALFilter filter;
  alGenFilters(1, &filter.filter);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to create OpenAL EFX filter");
    return std::nullopt;
  }

return filter;
}
void ALFilter::destroy() { alDeleteFilters(1, &filter); }
std::optional<ALEffect> ALEffect::create() {
  ALEffect effect;
  alGenEffects(1, &effect.effect);
  if (alGetError() != AL_NO_ERROR) {
      spdlog::error("Failed to create OpenAL EFX effect");
      return std::nullopt;
  }

  return effect;
}
std::optional<ALEffect> ALEffect::create(EffectDescription description) {
  auto effect = ALEffect::create();
  if (!effect) {
    return std::nullopt;
  }
  
  switch (description.type) {
  case EffectType::REVERB:
      effect->setReverb(description.reverb);
    break;
  default:
    spdlog::error("Unkown effect type");
    return std::nullopt;
    break;
  }

  return effect;
}
void ALEffect::setReverb(ReverbDescription reverb) {
  if (alGetEnumValue("AL_EFFECT_EAXREVERB") != 0) {
    alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
    alEffectf(effect, AL_EAXREVERB_DENSITY, reverb.density);
    alEffectf(effect, AL_EAXREVERB_DIFFUSION, reverb.diffusion);
    alEffectf(effect, AL_EAXREVERB_GAIN, reverb.gain);
    alEffectf(effect, AL_EAXREVERB_GAINHF, reverb.gainHF);
    alEffectf(effect, AL_EAXREVERB_GAINLF, reverb.gainLF);
    alEffectf(effect, AL_EAXREVERB_DECAY_TIME, reverb.decayTime);
    alEffectf(effect, AL_EAXREVERB_DECAY_HFRATIO, reverb.decayHFRatio);
    alEffectf(effect, AL_EAXREVERB_DECAY_LFRATIO, reverb.decayLFRatio);
    alEffectf(effect, AL_EAXREVERB_REFLECTIONS_GAIN, reverb.reflectionsGain);
    alEffectf(effect, AL_EAXREVERB_REFLECTIONS_DELAY, reverb.reflectionsDelay);
    alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, reverb.reflectionsPan);
    alEffectf(effect, AL_EAXREVERB_LATE_REVERB_GAIN, reverb.lateReverbGain);
    alEffectf(effect, AL_EAXREVERB_LATE_REVERB_DELAY, reverb.lateReverbDelay);
    alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, reverb.lateReverbPan);
    alEffectf(effect, AL_EAXREVERB_ECHO_TIME, reverb.echoTime);
    alEffectf(effect, AL_EAXREVERB_ECHO_DEPTH, reverb.echoDepth);
    alEffectf(effect, AL_EAXREVERB_MODULATION_TIME, reverb.modulationTime);
    alEffectf(effect, AL_EAXREVERB_MODULATION_DEPTH, reverb.modulationDepth);
    alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF,
              reverb.airAbsorptionGainHF);
    alEffectf(effect, AL_EAXREVERB_HFREFERENCE, reverb.hFReference);
    alEffectf(effect, AL_EAXREVERB_LFREFERENCE, reverb.lFReference);
    alEffectf(effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, reverb.roomRolloffFactor);
  } else {
    alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
    alEffectf(effect, AL_REVERB_DENSITY, reverb.density);
    alEffectf(effect, AL_REVERB_DIFFUSION, reverb.diffusion);
    alEffectf(effect, AL_REVERB_GAIN, reverb.gain);
    alEffectf(effect, AL_REVERB_GAINHF, reverb.gainHF);
    alEffectf(effect, AL_REVERB_DECAY_TIME, reverb.decayTime);
    alEffectf(effect, AL_REVERB_DECAY_HFRATIO, reverb.decayHFRatio);
    alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, reverb.reflectionsGain);
    alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, reverb.reflectionsDelay);
    alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, reverb.lateReverbGain);
    alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, reverb.lateReverbDelay);
    alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF,
              reverb.airAbsorptionGainHF);
    alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb.roomRolloffFactor);
    alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, reverb.decayHFLimit);
  }
}
void ALEffect::destroy() { alDeleteEffects(1, &effect); }
std::optional<ALAudioRequest> ALAudioRequest::create(uint32_t requestId) {
  std::optional<ALSource> source = ALSource::create();
  std::optional<ALFilter> filter = ALFilter::create();
  

  if (filter && source) {
    return ALAudioRequest{requestId, *source, *filter};
  }

  return std::nullopt;
}
void ALAudioRequest::destroy() {
  source.destroy();
  filter.destroy();
}
int ALBackend::m_MAX_CHANNELS = 0;
int ALBackend::m_MAX_SLOTS = 0;
int ALBackend::m_MAX_SLOTS_PER_CHANNEL = 0;

int ALBackend::MAX_CHANNELS() { return m_MAX_CHANNELS; }
int ALBackend::MAX_SLOTS() { return m_MAX_SLOTS; }
int ALBackend::MAX_SLOTS_PER_CHANNEL() { return m_MAX_SLOTS_PER_CHANNEL; }
std::optional<uint32_t> ALBackend::incrementOrCreateEffectSlot(
  const std::string& effectName) {

  // NOTE: incrementOrCreateEffectSlot checks if an effect name is
  // in effectToSlots to determine if it is new
  // This assumption is false, decrementEffectSlot does not remove
  // any elements from effectsToSlots so if you inc, dec, inc, dec this causes
  // the slot from freeSlots not to be put into used slots
  // 
  // The following code is to fix that bug and delete the necessary element in effectsToSlots
  if (auto slotIdLoc = effectsToSlots.find(effectName);
      slotIdLoc != effectsToSlots.end() && !usedSlots.contains(slotIdLoc->second)) {
    effectsToSlots.erase(slotIdLoc);
  }

  if (!effectsToSlots.contains(effectName)) {
    if (freeSlots.size() == 0) {
      spdlog::warn("No more free slots remain");
      return std::nullopt;
    }

    if (auto effectLoc = effects.find(effectName); effectLoc != effects.end()) {
      uint32_t slotId = freeSlots.extract(freeSlots.begin()).value();
      usedSlots.insert(slotId);
      assert(slotReferenceCounts[slotId] == 0);

      effectsToSlots[effectName] = slotId;
      
      slots[slotId].assign(effectLoc->second);
    } else {
      spdlog::error(
        "Could not create effect slot. Effect with name: {} was not found",
        effectName);
      return std::nullopt;
    }
  }

  if (auto slotIdLoc = effectsToSlots.find(effectName);
      slotIdLoc != effectsToSlots.end()) {
    uint32_t slotId = slotIdLoc->second;

    ++slotReferenceCounts[slotId];
    return slotId;
  }

  return std::nullopt;
}
void ALBackend::decrementEffectSlot(uint32_t slotId) {
  assert(slotId < slots.size());
  assert(slotReferenceCounts[slotId] != 0);

  --slotReferenceCounts[slotId];

  if (slotReferenceCounts[slotId] == 0) {
    assert(usedSlots.contains(slotId));
    slots[slotId].clear();

    usedSlots.extract(usedSlots.find(slotId)).value();
    freeSlots.insert(slotId);
  }
}

ALBackend::ALBackend() {
  device = alcOpenDevice(nullptr);

  ALint attribs[4] = { 0 };
  attribs[0] = ALC_MAX_AUXILIARY_SENDS;
  attribs[1] = 4;

  context = alcCreateContext(device, nullptr);
  if (!device || !context) {
    throw std::runtime_error("Failed to create openal device or context");
  }
  alcMakeContextCurrent(context);

  m_MAX_CHANNELS = 0;
  alcGetIntegerv(device, ALC_MONO_SOURCES, 1, &m_MAX_CHANNELS);

  m_MAX_SLOTS_PER_CHANNEL = 0;
  alcGetIntegerv(device, ALC_MAX_AUXILIARY_SENDS, 1, &m_MAX_SLOTS_PER_CHANNEL);
  if (m_MAX_SLOTS_PER_CHANNEL <= 2) {
    spdlog::warn("On this device OpenAL only supports {} sound effects per channel"
                 ". Other sound effects will be ignored", m_MAX_SLOTS_PER_CHANNEL);
  }
  
  alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);


  uint32_t slotId = 0;
  while (auto slot = ALAuxiliaryEffectSlot::create(false)) {
    slots.push_back(*slot);
    freeSlots.insert(slotId);
    ++slotId;
  }
  m_MAX_SLOTS = slots.size();
  for (size_t i = 0; i < slots.size(); ++i) {
    slotReferenceCounts.push_back(0);
  } 
   

  requests.reserve(m_MAX_CHANNELS);
  for (uint32_t i = 0; i < m_MAX_CHANNELS; ++i) {
    if (auto request = ALAudioRequest::create(i)) {
      requests.push_back(*request);
      freeRequestIds.insert(i);
    } else {
      spdlog::error("Could not create the amount of sources specified");
      break;
    }
  }

  // In case enough requests can't be created
  m_MAX_CHANNELS = requests.size();
}
ALBackend::~ALBackend() {
  for (auto &request : requests) {
    request.destroy();
  }
  requests.clear();

  freeRequestIds.clear();
  usedRequestIds.clear();

  for (auto &[name, buffer] : buffers) {
    buffer.destroy();
  }
  buffers.clear();

  for (auto &[name, monoBuffer] : monoBuffers) {
    monoBuffer.destroy();
  }
  monoBuffers.clear();

  for (auto &[name, effect] : effects) {
    effects.clear();
  }

  for (auto &slot : slots) {
    slot.destroy();
  }
  slots.clear();

  slotReferenceCounts.clear();
  effectsToSlots.clear();
  freeSlots.clear();
  usedSlots.clear();

}

void ALBackend::setParameters(uint32_t requestId,
                              Components::AudioSource &component,
                              const Components::Transform &transform,
                              const Components::Environment &environment,
                              const VolumeAdjuster &volumes) {
 
  if (ALAudioRequest *request = getRequest(requestId)) {
    if (environment.effectName) {
      setEnvironmentalEffect(requestId, *environment.effectName);
    } else {
      clearEnvironmentalEffect(requestId);
    }
    if (request->oldTransform != transform) {
      // This only triggers if the audio source is not spatial,
      // but the transform has changed
      if (!component.isSpatial) {
        spdlog::warn(
          "The audio source is spatial but the transform is changing."
          "The changes in position will not effect the audio source");
      } else {
        request->source.setPosition(transform.position);
        request->source.setRotationVersor(transform.rotation);
      }
    }

    
    float finalVolume = component.volume * volumes.masterVolume *
                        volumes.volumes.at(component.tag);
    request->source.setVolume(finalVolume);
    if (request->oldAudioSource != component) {
      request->oldAudioSource = component;

      // TODO: Use tags and master volume to adjust volume not just
      // source.volume
      request->source.setPitch(component.pitch);
      if (component.cone) {
        request->source.setCone(*component.cone);
      }
      request->source.setLooping(component.looping);
    }
  }
}
void ALBackend::registerEffect(const std::string &name, EffectDescription description) {
  if (effects.contains(name)) {
    spdlog::warn("Effect with name {} has alread been registered", name);
    return;
  }

  if (auto effect = ALEffect::create(description)) {
    effects[name] = *effect;
  } else {
    spdlog::warn("Effect {} was not able to be created", name);
  }
}
void ALBackend::genBuffers(const std::string &name,
                           std::span<const uint8_t> data, AudioFormat format,
                           size_t sampleRate) {
  if (buffers.contains(name)) {
    return;
  } else {
    auto buffer = ALBuffer::create(data, format, sampleRate);
    auto monoBuffer = ALBuffer::createMonoBuffer(data, format, sampleRate);

    if (buffer && monoBuffer) {
      buffers[name] = *buffer;
      monoBuffers[name] = *monoBuffer;
    }
  }
}
std::optional<ALBuffer> ALBackend::getMonoBuffer(const std::string &name) {
  if (auto monoBufferLoc = monoBuffers.find(name); monoBufferLoc != monoBuffers.end()) {
    return monoBufferLoc->second;
  }

  return std::nullopt;
}
std::optional<ALBuffer> ALBackend::getBuffer(const std::string &name) {
  if (auto bufferLoc = buffers.find(name); bufferLoc != buffers.end()) {
    return bufferLoc->second;
  }

  return std::nullopt;
}

std::optional<uint32_t> ALBackend::newRequest() {
  if (freeRequestIds.size() == 0) {
    return std::nullopt;
  }

  uint32_t channel = freeRequestIds.extract(freeRequestIds.begin()).value();

  usedRequestIds.insert(channel);

  return channel;
}
void ALBackend::freeRequest(uint32_t requestId) {
  if (!usedRequestIds.contains(requestId)) {
    spdlog::error("freeRequest was called twice on the same id");
    return;
  }

  if (auto idLoc = usedRequestIds.find(requestId); idLoc != usedRequestIds.end()) {
    
    ALAudioRequest *request = getRequest(requestId);
    assert(request);
    if (request->environmentalEffectSlotId) {
      decrementEffectSlot(*request->environmentalEffectSlotId);
    }

    request->source.reset();

    usedRequestIds.erase(idLoc);
    freeRequestIds.insert(requestId);
  }
}
ALAudioRequest* ALBackend::getRequest(uint32_t requestId) {
  if (!usedRequestIds.contains(requestId)) {
    spdlog::error("Invalid Id used");
    return nullptr;
  } else {
    assert(requestId < requests.size());

    return &requests[requestId];
  }
}
void ALBackend::setEnvironmentalEffect(uint32_t requestId,
                                       const std::string &name) {
  // if the effect slot for an effect doesn't exist
  // then create it and 
  if (auto *request = getRequest(requestId)) {

    if (request->environmentalEffectSlotId) {
      if (auto slotIdLoc = effectsToSlots.find(name);
          slotIdLoc != effectsToSlots.end()) {

        if (slotIdLoc->second == request->environmentalEffectSlotId) {
          return;
        }
      }
    }

    clearEnvironmentalEffect(requestId);
    if (auto slotId = incrementOrCreateEffectSlot(name)) {
      request->source.link(slots[*slotId], ALAudioRequest::ENVIRONMENTAL_EFFECT_SEND);
      request->environmentalEffectSlotId = slotId;
    } else {
      spdlog::warn(
        "No effect slots are available, no effect will be applied");
      return;
    }
  } else {
    spdlog::warn("Invalid requestId is being used {}", requestId);
    return;
  }

}
void ALBackend::clearEnvironmentalEffect(uint32_t requestId) {
  if (auto *request = getRequest(requestId)) {
    if (request->environmentalEffectSlotId) {
      request->source.unlink(ALAudioRequest::ENVIRONMENTAL_EFFECT_SEND);
      decrementEffectSlot(*request->environmentalEffectSlotId);
      request->environmentalEffectSlotId = std::nullopt;
    }
  } else {
    spdlog::warn("Invalid requestId is being used {}", requestId);
    return;
  }
}


void ALBackend::setListenerPos(vec3 pos) {
  alListenerfv(AL_POSITION, pos);
}
void ALBackend::getListenerPos(vec3 pos) {
  alGetListenerfv(AL_POSITION, pos);
}
}