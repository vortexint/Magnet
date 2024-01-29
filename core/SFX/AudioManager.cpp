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
#include <AL/efx.h>
#include <AL/efx-presets.h>

#include <cglm/cglm.h>




namespace Magnet {
using namespace Components;

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

    if (audioSource.playState == AudioSourcePlayState::REQUESTED_PLAY) {
      audioSource.channel = audioManager.borrowChannel();

      auto track = audioManager.getTrack(audioSource.trackName);

      if (track && audioSource.channel) {
        audioSource.channel->reset();
        audioSource.channel->assign(*track);
        audioSource.channel->play();
      } else {
        audioSource.channel = std::nullopt;
      }

      audioSource.playState = AudioSourcePlayState::PLAYING;
    }
    if (audioSource.playState == AudioSourcePlayState::PLAYING) {

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
      if (audioSource.channel) {
        audioManager.returnChannel(*audioSource.channel);
      }

      audioSource.stop();
    }
  }

  // If the user plays a sound from the same AudioSource twice,
  // the old channel will be set to std::nullopt.
  // That will prevent this system from returning it

  for (
    auto borrowedIter = audioManager.borrowedSpatialAudioChannels.begin();
    borrowedIter != audioManager.borrowedSpatialAudioChannels.end();
  ) {
    SpatialAudioChannel channel{ *borrowedIter };

    if (channel.isStopped()) {
      borrowedIter = audioManager.borrowedSpatialAudioChannels.erase(borrowedIter);
      audioManager.returnChannel(channel);
    } else {
      ++borrowedIter;
    }
  }
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
void AudioManager::returnChannel(SpatialAudioChannel channel) {
  if (auto channelLoc = borrowedSpatialAudioChannels.find(channel.source);
      channelLoc != borrowedSpatialAudioChannels.end()) {
    borrowedSpatialAudioChannels.extract(channelLoc);

    freeSpatialAudioChannels.insert(channel.source);
  } else {
    spdlog::error(
      "Channel {} was already returned. This is basically a use after free",
      channel.source);
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

    vec3 new_foward = {};
    glm_quat_rotatev(cameraTranform->rotation, forward, new_foward);

    vec3 new_up = {};
    glm_quat_rotatev(cameraTranform->rotation, up, new_up);

    float f[] = {
      forward[0], forward[1], forward[2], up[0], up[1], up[2],
    };

    alListenerfv(AL_ORIENTATION, f);
  }

  /* Velocity */
  // alListener3f(AL_VELOCITY, vel[0], vel[1], vel[2]);
}

// From OpenAL-soft version 1.17,
// no source or buffer will have the value of 0
// https://stackoverflow.com/questions/71095893/can-an-openal-source-ever-be-0
std::optional<AudioBuffer> AudioBuffer::create(std::span<uint8_t> bytes,
                                               AudioFormat audio_format,
                                               size_t samples,
                                               size_t sampleRate) {
  ALenum alFormat = AL_FORMAT_STEREO16;
  switch (audio_format) {
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
    createMonoBuffer(bytes, audio_format, samples, sampleRate);

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
void AudioChannel::setLooping(bool should_loop) {
  checkIfInitialized();

  ALint al_should_loop = should_loop ? AL_TRUE : AL_FALSE;
  alSourcei(source, AL_LOOPING, al_should_loop);
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
void SpatialAudioChannel::setLooping(bool should_loop) {
  checkIfInitialized();

  ALint al_should_loop = should_loop ? AL_TRUE : AL_FALSE;
  alSourcei(source, AL_LOOPING, al_should_loop);
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

    if (auto gain = desc->gain) {
      filter.setGain(*gain);
    }

    if (auto hf = desc->gainHighFrequency) {
      filter.setGainHighFrequency(*hf);
    }

    if (auto lf = desc->gainLowFrequency) {
      filter.setGainLowFrequency(*lf);
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
    alFilterf(filter, AL_HIGHPASS_GAINLF, f);
    break;
  case AudioFilterType::BANDPASS:
    alFilterf(filter, AL_BANDPASS_GAINLF, f);
    break;
  case AudioFilterType::LOWPASS:
    spdlog::warn("Only highbass and bandpass filters support setting the low frequency gain");
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
    spdlog::warn("Only highbass and bandpass filters support setting the low frequency gain");
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

std::optional<AudioEffect> AudioEffect::create() {
  AudioEffect effect;
  alGenEffects(1, &effect.effect);
  if (alGetError() != AL_NO_ERROR) {
    spdlog::error("Failed to create OpenAL EFX effect");
    return std::nullopt;
  }

  return effect;
}
void AudioEffect::destroy() {
  alDeleteEffects(1, &effect);
}

std::optional<AuxiliaryEffectSlot> AuxiliaryEffectSlot::create() {
  AuxiliaryEffectSlot slot;
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
  alContext = alcCreateContext(audioDevice, nullptr);

  if (!audioDevice || !alContext) {
    spdlog::error("Failed to create openal device or context");
    throw std::runtime_error("Failed to create openal device or context");
  }

  alcGetIntegerv(audioDevice, ALC_MONO_SOURCES, 1, &MAX_CHANNELS);
  AUDIO_CHANNELS = 1;
  SPATIAL_AUDIO_CHANNELS = MAX_CHANNELS - AUDIO_CHANNELS;

  alcMakeContextCurrent(alContext);

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

  InitializeOpenAL_EFX(audioDevice);
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
                                 samples * sizeof(short));

  if (auto buffer =
        AudioBuffer::create(samplesData, format, samples, sampleRate)) {
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