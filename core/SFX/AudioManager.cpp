#include <AL/al.h>
#include <AL/alc.h>
#include <cglm/cglm.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/AssetManager.hpp>
#include <magnet/AudioManager.hpp>
#include <magnet/Components.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/Time.hpp>

#define STB_VORBIS_IMPLEMENTATION
#include <stb_vorbis.h>

#include "GFX/Viewport.hpp"

#define AL_ALEXT_PROTOTYPES
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-presets.h>

#include <cglm/cglm.h>


// NOTE:
// OpenAl-Soft has a bunch of examples in openal-soft/examples
// It shows how to use EFX and HRTF extensions


namespace Magnet {
using namespace Components;

AudioFilterDescription::AudioFilterDescription(AudioFilterType type)
  : filterType(type) {
  gain = 1.f;
  gainHighFrequency = 1.f;
  gainLowFrequency = 1.f;
}

const char* alGetErrorString(int err) {
  switch (err) {
    case AL_NO_ERROR:
      return "AL_NO_ERROR";
    case AL_INVALID_ENUM:
      return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE:
      return "AL_INVALID_VALUE";
    case AL_OUT_OF_MEMORY:
      return "AL_OUT_OF_MEMORY";
    default:
      return "Unknown error code";
  }
}
AudioTagParameters& AudioManager::getTagModifier(AudioTag tag) {
  assert(tagModifier.contains(tag));
  return tagModifier[tag];
}
void AudioManager::AudioSourceSystem(flecs::iter& iter, Transform* transforms,
                                     AudioSource* sources) {
  AudioManager& audioManager = AudioManager::getInstance();


  for (size_t row : iter) {
    auto& transform = transforms[row];
    auto& audioSource = sources[row];

    if (audioSource.getPlayState() == AudioSourcePlayState::REQUESTED_PLAY) {
      bool requestSucceeded = true;
      audioSource.channel = audioManager.borrowChannel();


      if (audioSource.filterDesc) {
        audioSource.filter = AudioFilter::create(
          audioSource.filterDesc->filterType,
          audioSource.filterDesc
        );
      }
      if (audioSource.reverb) {
        audioSource.effect = AudioEffect::create(
          *audioSource.reverb
        );
      }

      if ((audioSource.filter || audioSource.effect) && audioSource.channel) {
        AudioFilter* filter = nullptr;
        if (audioSource.filter) {
          // DEBUG
          //filter = &*audioSource.filter;
          audioSource.channel->setFilter(*audioSource.filter);
        }
        
        if (audioSource.effect) {
          audioManager.effectSlot.attachEffect(*audioSource.effect);
        }
        audioSource.channel->setAuxiliaryEffectSlot(&audioManager.effectSlot, filter);

      }

      std::optional<AudioBuffer> track = std::nullopt;

      assert((audioSource.audioBuffer != nullptr) ^ (audioSource.getTrackName() != nullptr));

      if (audioSource.audioBuffer) {
        track = *audioSource.audioBuffer;
      } else {
        track = audioManager.getTrack(audioSource.getTrackName());
      }

      if (track && audioSource.channel) {
        audioSource.channel->reset();
        audioSource.channel->assign(*track);
        audioSource.channel->play();

        audioSource.playState = AudioSourcePlayState::PLAYING;
      } else {
        spdlog::error("Failed to play track");
        requestSucceeded = false;
      }
      
     
      

      if (!requestSucceeded) {
        audioSource.stop();
        spdlog::warn("Audio play request failed");
      }
    }
    if (audioSource.getPlayState() == AudioSourcePlayState::PLAYING) {
      assert(audioSource.channel);

      float finalVolume = audioSource.volume *
        audioManager.getTagModifier(audioSource.tag).volume *
        audioManager.getMaster().volume;

      audioSource.channel->setVolume(finalVolume);
      audioSource.channel->setPosition(transform.position);

      vec3 forward = { 1.f, 0.f, 0.f };
      vec3 forwardRes = {};

      glm_quat_rotatev(transform.rotation, forward, forwardRes);
      audioSource.channel->setDirection(forwardRes);

      audioSource.channel->setPitch(audioSource.pitch);
    }

    if (
      audioSource.playState == AudioSourcePlayState::STOPPED || 
      !audioSource.channel ||
      audioSource.channel->isStopped()
    ) {
      audioSource.stop();
    }
  }

  audioManager.returnBorrowedChannels();
}
std::optional<SpatialAudioChannel> AudioManager::borrowChannel() {
  if (freeSpatialAudioChannels.size() == 0) {
    spdlog::warn("There are no more free audio channels!");
    return std::nullopt;
  }

  auto channel =
    freeSpatialAudioChannels.extract(freeSpatialAudioChannels.begin()).value();

  borrowedSpatialAudioChannels.insert(channel);

  return SpatialAudioChannel{channel};
}
void AudioManager::returnBorrowedChannels() {
  for (
    auto borrowedIter = borrowedSpatialAudioChannels.begin();
    borrowedIter != borrowedSpatialAudioChannels.end();
    ) {
    SpatialAudioChannel channel{ *borrowedIter };

    if (channel.isStopped()) {
      freeSpatialAudioChannels.insert(*borrowedIter);
      borrowedIter = borrowedSpatialAudioChannels.erase(borrowedIter);
    } else {
      ++borrowedIter;
    }
  }
}

void AudioManager::updateListener() {
  Transform* cameraTranform = Viewport::activeCamera.get_mut<Transform>();

  /* Position */
  alListener3f(AL_POSITION, cameraTranform->position[0],
               cameraTranform->position[1], cameraTranform->position[2]);
  
  /* Orientation */
  {
    vec3 forward = {1.f, 0.f, 0.f};
    vec3 up = {0.f, 1.f, 0.f};

    vec3 newForward = {};
    glm_quat_rotatev(cameraTranform->rotation, forward, newForward);

    vec3 newUp = {};
    glm_quat_rotatev(cameraTranform->rotation, up, newUp);

    float f[] = {
      forward[0], forward[1], forward[2], up[0], up[1], up[2],
    };

    alListenerfv(AL_ORIENTATION, f);
  }

  /* Velocity */
  // alListener3f(AL_VELOCITY, vel[0], vel[1], vel[2]);
}

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
}
size_t FormatBytesPerSample(AudioFormat format) {
  switch (format)
  {
  case Magnet::AudioFormat::AUDIO_FORMAT_MONO16:
    return sizeof(uint16_t);
    break;
  case Magnet::AudioFormat::AUDIO_FORMAT_STEREO16:
    return sizeof(uint16_t) * 2;
    break;
  case Magnet::AudioFormat::AUDIO_FORMAT_STEREO8:
    return sizeof(uint8_t) * 2;
    break;
  case Magnet::AudioFormat::AUDIO_FORMAT_MONO8:
    return sizeof(uint8_t);
    break;
  }
}

// From OpenAL-soft version 1.17,
// no source or buffer will have the value of 0
// https://stackoverflow.com/questions/71095893/can-an-openal-source-ever-be-0
std::optional<AudioBuffer> AudioBuffer::create(std::span<const uint8_t> cBytes,
                                               AudioFormat audioFormat,
                                               size_t sampleRate) {
  
  std::vector<uint8_t> bufferCopy(cBytes.begin(), cBytes.end());
  std::span<uint8_t> bytes(bufferCopy.data(), bufferCopy.size());

  ALenum alFormat = AudioFormatToALint(audioFormat);
  size_t samples = bytes.size() / FormatBytesPerSample(audioFormat);

  ALuint originalBuffer = 0;
  alGenBuffers(1, &originalBuffer);
  alBufferData(originalBuffer, alFormat, bytes.data(), bytes.size_bytes(),
               sampleRate);
  if (originalBuffer == 0 || alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to create AL buffer data {}", alGetErrorString(err));
    return std::nullopt;
  }


  auto monoBufferRes =
    createMonoBuffer(bytes, audioFormat, samples, sampleRate);

  if (!monoBufferRes) {
    return std::nullopt;
  }

  return AudioBuffer{originalBuffer, *monoBufferRes, samples, sampleRate};
}
std::optional<uint32_t> AudioBuffer::createMonoBuffer(std::span<uint8_t> bytes,
                                                      AudioFormat format,
                                                      size_t samples,
                                                      size_t sampleRate) {
  ALenum alFormat = AL_FORMAT_STEREO16;
  switch (format) {
    case AudioFormat::AUDIO_FORMAT_MONO8:
      alFormat = AL_FORMAT_MONO8;
      break;
    case AudioFormat::AUDIO_FORMAT_MONO16:
      alFormat = AL_FORMAT_MONO16;
      break;
    case AudioFormat::AUDIO_FORMAT_STEREO8:
      alFormat = AL_FORMAT_STEREO8;
      break;
    case AudioFormat::AUDIO_FORMAT_STEREO16:
      alFormat = AL_FORMAT_STEREO16;
      break;
  }

  uint32_t monoBufferSizeBytes = 0;
  ALenum monoBufferFormat = 0;
  if (alFormat == AL_FORMAT_MONO8 || alFormat == AL_FORMAT_MONO16) {
    monoBufferSizeBytes = bytes.size_bytes();
  } else {
    assert(alFormat == AL_FORMAT_STEREO8 || alFormat == AL_FORMAT_STEREO16);
    assert(bytes.size_bytes() % 2 == 0);

    monoBufferSizeBytes = bytes.size_bytes() / 2;

    if (alFormat == AL_FORMAT_STEREO16) {
      short* stereoBytes = reinterpret_cast<short*>(bytes.data());

      for (size_t i = 0; i < bytes.size_bytes() / (2 * sizeof(short)); ++i) {
        stereoBytes[i] = (stereoBytes[2 * i] + stereoBytes[2 * i + 1]) / 2;
      }
    } else {
      uint8_t* stereoBytes = bytes.data();
      for (size_t i = 0; i < bytes.size_bytes() / (2 * sizeof(uint8_t)); ++i) {
        stereoBytes[i] = (stereoBytes[2 * i] + stereoBytes[2 * i + 1]) / 2;
      }
    }
  }

  if (alFormat == AL_FORMAT_STEREO16) {
    monoBufferFormat = AL_FORMAT_MONO16;
  } else if (alFormat == AL_FORMAT_STEREO8) {
    monoBufferFormat = AL_FORMAT_MONO8;
  } else {
    monoBufferFormat = alFormat;
  }

  ALuint monoBuffer = 0;
  alGenBuffers(1, &monoBuffer);
  alBufferData(monoBuffer, monoBufferFormat, bytes.data(), monoBufferSizeBytes,
               sampleRate);
  if (monoBuffer == 0 || alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to create AL mono buffer data {}",
                  alGetErrorString(err));
    return std::nullopt;
  }

  return monoBuffer;
}
void AudioBuffer::destroy() const {
  alDeleteBuffers(1, &originalBuffer);
  alDeleteBuffers(1, &monoBuffer);
}
double AudioBuffer::length() const {
  float scale = 1.f;
  double seconds =
    static_cast<double>(samples) / static_cast<double>(sampleRate);
  return seconds;
}

void AudioChannel::reset() {
  stop();
  alSourcei(source, AL_BUFFER, 0);
  alSourcef(source, AL_ROLLOFF_FACTOR, 1.f);

  vec4 zero_vec = {0.f, 0.f, 0.f, 0.f};
  alSourcefv(source, AL_POSITION, zero_vec);
  alSourcefv(source, AL_VELOCITY, zero_vec);
  alSourcefv(source, AL_DIRECTION, zero_vec);

  setLooping(false);
}
std::optional<AudioChannel> AudioChannel::create() {
  AudioChannel source;
  alGenSources(1, &source.source);
  if (source.source == 0) {
    auto err = alGetError();
    spdlog::error("Failed to create AL source {}", alGetErrorString(err));
    return std::nullopt;
  }
  return source;
}

void AudioChannel::assign(const AudioBuffer& buffer) {
  if (!isStopped()) {
    stop();
  }
  alSourcei(source, AL_BUFFER, buffer.originalBuffer);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to assign AL source to buffer {}",
                  alGetErrorString(err));
  }
}
void AudioChannel::play() {
  checkIfInitialized();

  alSourcePlay(source);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to play track {}", alGetErrorString(err));
  }
}
bool AudioChannel::isCurrentlyPlaying() {
  ALint state = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;
}
void AudioChannel::stop() {
  checkIfInitialized();

  alSourceStop(source);
}
bool AudioChannel::isStopped() {
  checkIfInitialized();

  ALint sourceState = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
  return sourceState == AL_STOPPED;
}
void AudioChannel::pause() {
  checkIfInitialized();

  alSourcePause(source);
}
bool AudioChannel::isLooping() {
  checkIfInitialized();

  ALint looping = AL_FALSE;
  alGetSourcei(source, AL_LOOPING, &looping);
  return looping == AL_TRUE;
}
void AudioChannel::setLooping(bool shouldLoop) {
  checkIfInitialized();

  ALint alShouldLoop = shouldLoop ? AL_TRUE : AL_FALSE;
  alSourcei(source, AL_LOOPING, alShouldLoop);
}
void AudioChannel::destroy() const { alDeleteSources(1, &source); }
void AudioChannel::setVolume(float vol) { alSourcef(source, AL_GAIN, vol); }
void AudioChannel::setPitch(float pitch) {
  AL_PITCH;
  alSourcef(source, AL_PITCH, pitch);
}
void AudioChannel::checkIfInitialized() const {
  if (source == 0) {
    spdlog::warn("source is 0, did you forget to initialize it?");
  }
}

std::optional<SpatialAudioChannel> SpatialAudioChannel::create() {
  SpatialAudioChannel source;
  alGenSources(1, &source.source);
  if (source.source == 0) {
    auto err = alGetError();
    spdlog::error("Failed to create AL source {}", alGetErrorString(err));
    return std::nullopt;
  }
  return source;
}
void SpatialAudioChannel::assign(const AudioBuffer& buffer) {
  if (!isStopped()) {
    stop();
  }
  alSourcei(source, AL_BUFFER, buffer.monoBuffer);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to assign AL source to buffer {}",
                  alGetErrorString(err));
  }
}
void SpatialAudioChannel::play() {
  checkIfInitialized();

  alSourcePlay(source);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to play track {}", alGetErrorString(err));
  }
}
bool SpatialAudioChannel::isCurrentlyPlaying() {
  ALint state = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;
}
void SpatialAudioChannel::stop() {
  checkIfInitialized();

  alSourceStop(source);
}
void SpatialAudioChannel::reset() {
  stop();
  alSourcei(source, AL_BUFFER, 0);
  alSourcef(source, AL_ROLLOFF_FACTOR, 1.f);

  vec4 zero_vec = {0.f, 0.f, 0.f, 0.f};
  alSourcefv(source, AL_POSITION, zero_vec);
  alSourcefv(source, AL_VELOCITY, zero_vec);
  alSourcefv(source, AL_DIRECTION, zero_vec);

  setLooping(false);
}
bool SpatialAudioChannel::isStopped() {
  checkIfInitialized();

  ALint sourceState = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
  return sourceState == AL_STOPPED;
}
void SpatialAudioChannel::pause() {
  checkIfInitialized();

  alSourcePause(source);
}
bool SpatialAudioChannel::isLooping() {
  checkIfInitialized();

  ALint looping = AL_FALSE;
  alGetSourcei(source, AL_LOOPING, &looping);
  return looping == AL_TRUE;
}
void SpatialAudioChannel::setLooping(bool shouldLoop) {
  checkIfInitialized();

  alSourcei(source, AL_LOOPING, shouldLoop ? AL_TRUE : AL_FALSE);
}
void SpatialAudioChannel::destroy() const { alDeleteSources(1, &source); }
void SpatialAudioChannel::setVolume(float vol) {
  alSourcef(source, AL_GAIN, vol);
}
void SpatialAudioChannel::setPitch(float pitch) {
  AL_PITCH;
  alSourcef(source, AL_PITCH, pitch);
}
void SpatialAudioChannel::setPosition(vec3 pos) {
  alSourcefv(source, AL_POSITION, pos);
  if (alGetError() != AL_NO_ERROR) {
    assert(false);
  }
}
void SpatialAudioChannel::setVelocity(vec3 vel) {
  alSourcefv(source, AL_VELOCITY, vel);
}
void SpatialAudioChannel::setDirection(vec3 dir) {
  alSourcefv(source, AL_DIRECTION, dir);
}
void SpatialAudioChannel::checkIfInitialized() const {
  if (source == 0) {
    spdlog::warn("source is 0, did you forget to initialize it?");
  }
}
void SpatialAudioChannel::setCone(float angleDeg, float outerVolume) {
  if (fabs(angleDeg) < 0.001f) {
    alSourcef(source, AL_CONE_OUTER_ANGLE, 360.f);
  }
  // Openal uses degrees
  alSourcef(source, AL_CONE_INNER_ANGLE, angleDeg);
  alSourcef(source, AL_CONE_OUTER_GAIN, outerVolume);
}
void SpatialAudioChannel::setFilter(AudioFilter &filter) {
  alSourcei(source, AL_DIRECT_FILTER, filter.filter);
}
void SpatialAudioChannel::setAuxiliaryEffectSlot(
  AuxiliaryEffectSlot *effectSlot = nullptr, 
  AudioFilter *filter = nullptr
) {
  // TODO: Add an optional third parameter for the filter
  ALint slotID = AL_EFFECTSLOT_NULL;
  if (effectSlot) {
    slotID = effectSlot->slot;
  }
  ALint filterID = AL_FILTER_NULL;
  //ALint filterEnabled = 0;
  if (filter) {
    filterID = filter->filter;
    //filterEnabled = 1;
  }
  // DEBUG: Replace filter enabled with 0
  alSource3i(source, AL_AUXILIARY_SEND_FILTER, slotID, 0 /*filterEnabled*/, filterID);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to set axuliary effect slot");
  }
}
void SpatialAudioChannel::disableAuxiliaryEffectSlot() {
  //TODO: Figure out what sends are
  // For this function we pass 1 as the send argument
  // while in the previous we passed 0
  // AL_AUXILIARY_SEND_FILTER takes three integer arguments
  // The first is the effect, the third is the filter
  // The second argument is you're attaching a filter or not
  // 0 means don't attach filter
  // 1 means do attach filter
  ALC_MAX_AUXILIARY_SENDS;
  alSource3i(source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 1, AL_FILTER_NULL);
}

std::optional<AudioFilter> AudioFilter::create(AudioFilterType type, std::optional<AudioFilterDescription> desc) {
  AudioFilter filter;
  alGenFilters(1, &filter.filter);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to create OpenAL EFX filter");
    return std::nullopt;
  }

  ALint filterType = 0;
  switch (type) {
  case AudioFilterType::LOWPASS:
    filterType = AL_FILTER_LOWPASS;
    break;
  case AudioFilterType::HIGHPASS:
    filterType = AL_FILTER_HIGHPASS;
    break;
  case AudioFilterType::BANDPASS:
    filterType = AL_FILTER_BANDPASS;
    break;
  }

  alFilteri(filter.filter, AL_FILTER_TYPE, filterType);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to set EFX Filter type");
    filter.destroy();
    return std::nullopt;
  }

  if (desc) {

    AudioFilterType type = desc->filterType;
    filter.setGain(desc->gain);


    if (
      desc->filterType == AudioFilterType::LOWPASS ||
      desc->filterType == AudioFilterType::BANDPASS) {
      filter.setGainHighFrequency(desc->gainHighFrequency);
    }
    if (
      desc->filterType == AudioFilterType::HIGHPASS ||
      desc->filterType == AudioFilterType::BANDPASS) {
      filter.setGainLowFrequency(desc->gainLowFrequency);
    }
  }

  return filter;
}
void AudioFilter::destroy() {
  alDeleteFilters(1, &filter);
}
void AudioFilter::setGain(float f) {
  ALenum parameter = 0;
  switch (filterType()) {
  case AudioFilterType::LOWPASS:
    parameter = AL_LOWPASS_GAIN;
    break;
  case AudioFilterType::HIGHPASS:
    parameter = AL_HIGHPASS_GAIN;
    break;
  case AudioFilterType::BANDPASS:
    parameter = AL_BANDPASS_GAIN;
    break;
  }

  alFilterf(filter, parameter, f);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::warn("Failed to set filter gain");
  }
}
void AudioFilter::setGainHighFrequency(float f) {
  switch (filterType()) {
  case AudioFilterType::HIGHPASS:
    spdlog::warn("Only lowpass and bandpass filters support setting the low frequency gain");
    break;
  case AudioFilterType::BANDPASS:
    alFilterf(filter, AL_BANDPASS_GAINLF, f);
    break;
  case AudioFilterType::LOWPASS:
    alFilterf(filter, AL_HIGHPASS_GAINLF, f);
    break;
  }
}
void AudioFilter::setGainLowFrequency(float f) {
  switch (filterType()) {
  case AudioFilterType::HIGHPASS:
    alFilterf(filter, AL_HIGHPASS_GAINLF, f);
    break;
  case AudioFilterType::BANDPASS:
    alFilterf(filter, AL_BANDPASS_GAINLF, f);
    break;
  case AudioFilterType::LOWPASS:
    spdlog::warn("Only highpass and bandpass filters support setting the low frequency gain");
    break;
  }
}
AudioFilterType AudioFilter::filterType() {
  ALint filterType;
  alGetFilteri(filter, AL_FILTER_TYPE, &filterType);
  switch (filterType) {
  case AL_FILTER_LOWPASS:
    return AudioFilterType::LOWPASS;
    break;
  case AL_FILTER_HIGHPASS:
    return AudioFilterType::HIGHPASS;
    break;
  case AL_FILTER_BANDPASS:
    return AudioFilterType::BANDPASS;
    break;
  default:
    // There should be no situtation
    // where getting the filter type is AL_FILTER_NONE
    assert(false);
  }

  return AudioFilterType::LOWPASS;
}

std::optional<AudioEffect> AudioEffect::create(AudioEffectType type) {
  AudioEffect effect;
  alGenEffects(1, &effect.effect);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to create OpenAL EFX effect");
    return std::nullopt;
  }

  ALint alEffectType = AL_EFFECT_NULL;
  bool effectSupported = false;
  switch (type)
  {
  case Magnet::AudioEffectType::EAXREVERB:
    alEffectType = AL_EFFECT_EAXREVERB;
    effectSupported = true;
    break;
  case Magnet::AudioEffectType::REVERB:
    alEffectType = AL_EFFECT_REVERB;
    break;
  case Magnet::AudioEffectType::CHORUS:
    alEffectType = AL_EFFECT_CHORUS;
    break;
  case Magnet::AudioEffectType::DISTORTION:
    alEffectType = AL_EFFECT_DISTORTION;
    break;
  case Magnet::AudioEffectType::ECHO:
    alEffectType = AL_EFFECT_ECHO;
    break;
  case Magnet::AudioEffectType::FLANGER:
    alEffectType = AL_EFFECT_FLANGER;
    break;
  case Magnet::AudioEffectType::FREQUENCY_SHIFTER:
    alEffectType = AL_EFFECT_FREQUENCY_SHIFTER;
    break;
  case Magnet::AudioEffectType::VOCAL_MORPHER:
    alEffectType = AL_EFFECT_VOCAL_MORPHER;
    break;
  case Magnet::AudioEffectType::PITCH_SHIFTER:
    alEffectType = AL_EFFECT_PITCH_SHIFTER;
    break;
  case Magnet::AudioEffectType::RING_MODULATOR:
    alEffectType = AL_EFFECT_RING_MODULATOR;
    break;
  case Magnet::AudioEffectType::AUTOWAH:
    alEffectType = AL_EFFECT_AUTOWAH;
    break;
  case Magnet::AudioEffectType::COMPRESSOR:
    alEffectType = AL_EFFECT_COMPRESSOR;
    break;
  case Magnet::AudioEffectType::EQUALIZER:
    alEffectType = AL_EFFECT_EQUALIZER;
    break;
  default:
    break;
  }

  alEffecti(effect.effect, AL_EFFECT_TYPE, alEffectType);
  if (alEffectType == AL_EFFECT_NULL || alGetError() != AL_NO_ERROR) {
    spdlog::error("Effect Type is not supported\n");
    effect.destroy();
    return std::nullopt;
  }

  return effect;
}
void AudioEffect::setEAXReverb(const EAXReverbDescription &reverb) {

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
    alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, reverb.airAbsorptionGainHF);
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
    alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb.airAbsorptionGainHF);
    alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb.roomRolloffFactor);
    alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, reverb.decayHFLimit);
  }
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to set eax reverb");
  }
}
std::optional<AudioEffect> AudioEffect::create(const EAXReverbDescription &reverb) {
  auto effect = create(AudioEffectType::EAXREVERB);
  if (!effect) {
    return std::nullopt;
  }
  
  effect->setEAXReverb(reverb);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to set eaxreverb variables, eaxreverb might not be supported");
    effect->destroy();
    return std::nullopt;
  }

  assert(effect);

  return effect;
}
void AudioEffect::destroy() {
  alDeleteEffects(1, &effect);
}

std::optional<AuxiliaryEffectSlot> AuxiliaryEffectSlot::create() {
  AuxiliaryEffectSlot slot = {};
  alGenAuxiliaryEffectSlots(1, &slot.slot);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to create OpenAL EFX AuxiliaryEffectSlot");
    return std::nullopt;
  }

  return slot;
}
void AuxiliaryEffectSlot::destroy() {
  alDeleteAuxiliaryEffectSlots(1, &slot);
}

void AuxiliaryEffectSlot::attachEffect(AudioEffect &effect) {
  alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect.effect);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to attack effect to auxiliary effect slot");
  }
}


std::optional<Recorder> Recorder::create() {
  // I was planning to let the user choose the format they wanted
  // but OpenAL freaks out when I choose a stereo format
  // It overruns the buffer I give it
  AudioFormat format = AudioFormat::AUDIO_FORMAT_MONO16;
  ALint alFormat = AudioFormatToALint(format);
  auto captureDevice = alcCaptureOpenDevice(nullptr, FREQUENCY, alFormat, BUFFER_SIZE);
  if (captureDevice == nullptr) {
    spdlog::error("Failed to find a audio capture device");
    return std::nullopt;
  }

  Recorder recorder = { captureDevice, false, format, {} };
  recorder.buffer.reserve(BUFFER_SIZE * FormatBytesPerSample(format));

  return recorder;
}
void Recorder::startCapture() {
  assert(captureStarted == false);

  alcCaptureStart(captureDevice);
  captureStarted = true;
}
void Recorder::update() {
  if (!captureStarted) {
    return;
  }
  ALint samples = 0;
  alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, AudioFormatToALint(format), &samples);
  
  int64_t bytesRequired = FormatBytesPerSample(format) * samples;

  const int MEGABYTES = 1048576;
  
  if (buffer.capacity() - buffer.size() < bytesRequired) {
    size_t newSize = std::max(buffer.capacity() * 2, buffer.size() + bytesRequired);
    buffer.reserve(newSize);
  }
  //assert(buffer.size() <= 100 * MEGABYTES); // Lets not let it get too big

  size_t oldSize = buffer.size();
  buffer.resize(oldSize + bytesRequired);

  alcCaptureSamples(captureDevice, &buffer.data()[oldSize], samples);
}
void Recorder::stopCapture() {
  assert(captureStarted == true);

  alcCaptureStop(captureDevice);
  captureStarted = false;
}
void Recorder::clear() {
  buffer.clear();
}
std::span<const uint8_t> Recorder::toSpan() const {
  return std::span<const uint8_t>(buffer.data(), buffer.size());
}
void Recorder::destroy() {
  alcCaptureCloseDevice(captureDevice);
}

void AudioManager::InitializeOpenAL_EFX(ALCdevice *audioDevice) {
  if (alcIsExtensionPresent(audioDevice, ALC_EXT_EFX_NAME) == AL_FALSE) {
    spdlog::error("EFX extension is not present on device");
    throw std::runtime_error("EFX extension is not present on device");
  }

}

AudioManager::AudioManager()
  : assetManager(nullptr) {
  assetManager = new AssetManager("data.magnet", nullptr);
  audioDevice = alcOpenDevice(nullptr);

  ALint attribs[4] = { 0 };

  attribs[0] = ALC_MAX_AUXILIARY_SENDS;
  attribs[1] = 4;

  alContext = alcCreateContext(audioDevice, attribs);

  if (!audioDevice || !alContext) {
    spdlog::error("Failed to create openal device or context");
    throw std::runtime_error("Failed to create openal device or context");
  }

  alcGetIntegerv(audioDevice, ALC_MONO_SOURCES, 1, &MAX_CHANNELS);
  AUDIO_CHANNELS = 1;
  SPATIAL_AUDIO_CHANNELS = MAX_CHANNELS - AUDIO_CHANNELS;

  alcMakeContextCurrent(alContext);

  InitializeOpenAL_EFX(audioDevice);


  effectSlot = AuxiliaryEffectSlot::create().value(); // TODO: Check for errors

  alDistanceModel(AL_INVERSE_DISTANCE);

  background = AudioChannel::create().value();

  for (int i = 0; i < SPATIAL_AUDIO_CHANNELS; ++i) {
    if (auto channel = SpatialAudioChannel::create()) {
      freeSpatialAudioChannels.insert(channel->source);
    }
  }

  for (auto tag : {AudioTag::NONE, AudioTag::VOICE, AudioTag::SOUND_EFFECTS,
                   AudioTag::MUSIC}) {
    tagModifier[tag] = AudioTagParameters{};
  }
}
AudioManager::~AudioManager() {
  for (auto& spatialAudioChannel : freeSpatialAudioChannels) {
    SpatialAudioChannel{spatialAudioChannel}.destroy();
  }
  freeSpatialAudioChannels.clear();
  for (auto& spatialAudioChannel : borrowedSpatialAudioChannels) {
    SpatialAudioChannel{spatialAudioChannel}.destroy();
  }
  borrowedSpatialAudioChannels.clear();

  for (auto& [trackPath, trackData] : tracks) {
    trackData.destroy();
  }

  tracks.clear();

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(alContext);
  alcCloseDevice(audioDevice);
}

struct DeleteStbVorbisData {
  void operator()(short* ptr) const { free(ptr); }
};

std::optional<AudioBuffer> AudioManager::getTrack(const char* track) {
  assert(track);

  if (auto trackLoc = tracks.find(track); trackLoc != tracks.end()) {
    return trackLoc->second;
  }

  int spacialAudioChannels = 0, sampleRate = 0;
  short* hData = nullptr;

  std::vector<uint8_t> fileBuffer;
  assetManager->getAsset(track, fileBuffer);
  if (fileBuffer.size() == 0) {
    spdlog::error("Failed to open file {}", track);
    return std::nullopt;
  }

  int samples =
    stb_vorbis_decode_memory(fileBuffer.data(), fileBuffer.size(),
                             &spacialAudioChannels, &sampleRate, &hData);
  std::unique_ptr<short, DeleteStbVorbisData> data(hData);
  if (data == nullptr) {
    spdlog::error("Failed to open file {}", track);
    return std::nullopt;
  }
  auto format = AudioFormat::AUDIO_FORMAT_STEREO16;
  if (spacialAudioChannels == 1) {
    format = AudioFormat::AUDIO_FORMAT_MONO16;
  }

  std::span<uint8_t> samplesData(reinterpret_cast<uint8_t*>(data.get()),
                                 samples * FormatBytesPerSample(format));

  if (auto buffer =  AudioBuffer::create(samplesData, format, sampleRate)) {
    tracks[track] = *buffer;

    return *buffer;
  }

  return std::nullopt;
}

void AudioManager::playTrackBackground(const char* track) {
  if (auto trackLoc = tracks.find(track); trackLoc != tracks.end()) {
    background.assign(trackLoc->second);
    background.play();
    if (alGetError() != AL_NO_ERROR) {
      auto err = alGetError();
      spdlog::error("Failed to play track {} {}", track, alGetErrorString(err));
    }
  } else {
    spdlog::warn("Track could not be played track was not found {}", track);
  }
}

void AudioManager::deleteTrack(const char* track) {
  if (auto trackLoc = tracks.find(track); trackLoc != tracks.end()) {
    trackLoc->second.destroy();
    tracks.erase(trackLoc);
  } else {
    spdlog::error("Track {} was not found and could not be deleted", track);
  }
}

std::string to_string(Magnet::AudioTag tag) {
  switch (tag) {
    case Magnet::AudioTag::VOICE:
      return "VOICE";
    case Magnet::AudioTag::SOUND_EFFECTS:
      return "SOUND_EFFECTS";
    case Magnet::AudioTag::MUSIC:
      return "MUSIC";
    case Magnet::AudioTag::NONE:
      return "NONE";
  };
}

}  // namespace Magnet