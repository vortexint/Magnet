#include <magnet/AudioManager.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/AssetManager.hpp>
#include <magnet/ApplicationContext.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/Components.hpp>
#include <magnet/Time.hpp>

#include <spdlog/spdlog.h>

#define STB_VORBIS_IMPLEMENTATION
#include <stb_vorbis.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <cglm/cglm.h>



namespace Magnet {
const char* alGetErrorString(int err) {
  switch (err) {
  case AL_NO_ERROR: return "AL_NO_ERROR";
  case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
  case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
  case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
  default:
    return "Unknown error code";
  }
}
AudioTagParameters& AudioManager::getTagModifier(AudioTag tag) {
  return tagModifier[tag];
}
void AudioManager::AudioSourceSystem(
  flecs::iter& iter, Components::Transform* transforms, Components::AudioSource* sources) {
  AudioManager& audioManager = AudioManager::getInstance();

  for (size_t row : iter) {
    auto& transform = transforms[row];
    auto& audioSource = sources[row];

    for (auto &request : audioSource.requests) {
      if (!request) {
        continue;
      }

      auto& channel = request->channel;
      auto track = audioManager.getTrack(request->trackName);
      if (!channel) {
        request->channel = audioManager.borrowChannel();

        if (track) {
          request->channel->reset();
          request->channel->assign(*track);
          request->channel->play();
        }
      }
      if (channel) {
        float finalVolume = request->volume *
          audioManager.getTagModifier(request->tag).volume *
          audioManager.getMaster().volume;

        request->channel->set_volume(finalVolume);
        request->channel->set_position(transform.translation);

        vec3 forward = { 1.f, 0.f, 0.f };
        vec3 forwardRes = {};
        
        glm_quat_rotatev(transform.rotation, forward, forwardRes);
        request->channel->set_direction(forwardRes);

        // TODO: Full implement
      }

      if (!channel || !track || channel->is_stopped()) {
        if (channel) {
          audioManager.returnChannel(*channel);
          channel = std::nullopt;
        }
        request = std::nullopt;
      }
    }
  }
}
std::optional<SpatialAudioChannel> AudioManager::borrowChannel() {
  if (freeSpatialAudioChannels.size() == 0) {
    spdlog::warn("There are no more free audio channels");
    return std::nullopt;
  }

  auto channel = 
    freeSpatialAudioChannels.extract(freeSpatialAudioChannels.begin())
    .value();

  borrowedSpatialAudioChannels.insert(channel);

  return SpatialAudioChannel{ channel };
}
void AudioManager::returnChannel(SpatialAudioChannel channel) {
  if (
    auto channelLoc = borrowedSpatialAudioChannels.find(channel.source);
    channelLoc != borrowedSpatialAudioChannels.end()
  ) {

    borrowedSpatialAudioChannels.extract(channelLoc);

    freeSpatialAudioChannels.insert(channel.source);
  } else {
    spdlog::error("Channel {} was already returned. This is basically a use after free", channel.source);
  }
}
void AudioManager::AudioListenerSystem(flecs::iter& iter, Components::Transform* transforms, Components::AudioListener* listener) {
  uint32_t listenerCount = 0;
  bool has_set_position = false;
  for (auto row : iter) {
    ++listenerCount;

    if (!has_set_position) {
      auto& transform = transforms[row];

      GlobalAudioListener::set_position(transform.translation);
      GlobalAudioListener::set_orientation_versor(transform.rotation);
      // TODO: Use this once we have some physics
      // GlobalAudioListener::set_velocity()
      

      has_set_position = true;
    }
  }

  if (listenerCount > 1) {
    spdlog::warn("There should only ever be one audio listener, there are {}", listenerCount);
  }
}

void GlobalAudioListener::set_position(vec3 pos) {
  alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);
}
void GlobalAudioListener::set_velocity(vec3 vel) {
  alListener3f(AL_VELOCITY, vel[0], vel[1], vel[2]);
}


void GlobalAudioListener::set_orientation_versor(versor v) {
  vec3 forward = { 1.f, 0.f, 0.f };
  vec3 up = { 0.f, 1.f, 0.f };

  vec3 new_foward = {};
  glm_quat_rotatev(v, forward, new_foward);

  vec3 new_up = {};
  glm_quat_rotatev(v, up, new_up);

  set_orientation_forward_up(forward, up);
}
void GlobalAudioListener::set_orientation_forward_up(vec3 forward, vec3 up) {
  float f[] = {
    forward[0],
    forward[1],
    forward[2],
    up[0],
    up[1],
    up[2],
  };

  alListenerfv(AL_ORIENTATION, f);
}

// From OpenAL-soft version 1.17, 
// no source or buffer will have the value of 0
// https://stackoverflow.com/questions/71095893/can-an-openal-source-ever-be-0
std::optional<AudioBuffer> AudioBuffer::create(std::span<uint8_t> bytes, AudioFormat audio_format, size_t samples, size_t sampleRate) {
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
  alBufferData(originalBuffer, alFormat, bytes.data(), bytes.size_bytes(), sampleRate);
  if (originalBuffer == 0 || alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to create AL buffer data {}", alGetErrorString(err));
    return std::nullopt;
  }

  auto monoBufferRes = createMonoBuffer(bytes, audio_format, samples, sampleRate);

  if (!monoBufferRes) {
    return std::nullopt;
  }

  return AudioBuffer{
    originalBuffer,
    *monoBufferRes,
    samples,
    sampleRate
  };
}
std::optional<uint32_t> AudioBuffer::createMonoBuffer(std::span<uint8_t> bytes, AudioFormat format, size_t samples, size_t sampleRate) {
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
  }
  else {
    assert(alFormat == AL_FORMAT_STEREO8 || alFormat == AL_FORMAT_STEREO16);

    assert(bytes.size_bytes() % 2 == 0);

    monoBufferSizeBytes = bytes.size_bytes() / 2;

    if (alFormat == AL_FORMAT_STEREO16) {
      short* stereoBytes = reinterpret_cast<short*>(bytes.data());

      for (size_t i = 0; i < bytes.size_bytes() / (2 * sizeof(short)); ++i) {
        stereoBytes[i] = (stereoBytes[2 * i] + stereoBytes[2 * i + 1]) / 2;
      }
    }
    else {
      uint8_t* stereoBytes = bytes.data();
      for (size_t i = 0; i < bytes.size_bytes() / (2 * sizeof(uint8_t)); ++i) {
        stereoBytes[i] = (stereoBytes[2 * i] + stereoBytes[2 * i + 1]) / 2;
      }
    }
  }


  if (alFormat == AL_FORMAT_STEREO16) {
    monoBufferFormat = AL_FORMAT_MONO16;
  }
  else if (alFormat == AL_FORMAT_STEREO8) {
    monoBufferFormat = AL_FORMAT_MONO8;
  }
  else {
    monoBufferFormat = alFormat;
  }


  ALuint monoBuffer = 0;
  alGenBuffers(1, &monoBuffer);
  alBufferData(monoBuffer, monoBufferFormat, bytes.data(), monoBufferSizeBytes, sampleRate);
  if (monoBuffer == 0 || alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to create AL mono buffer data {}", alGetErrorString(err));
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
  double seconds = static_cast<double>(samples) / static_cast<double>(sampleRate);
  return seconds;
}


size_t SpatialAudioRequest::hash() const {
  size_t trackNameHash = std::hash<const char*>()(trackName);
  size_t timestampHash = std::hash<double>()(timestampStartedS);

  size_t finalHash = trackNameHash;
  finalHash ^= timestampHash + 0x9e3779b9 + (finalHash << 6) + (finalHash >> 2);

  return finalHash;
}
void SpatialAudioRequest::stop() {
  if (channel) {
    channel->stop();
  }
}
void SpatialAudioRequest::setPitch(float pitch) {
  this->pitch = pitch;
}
void SpatialAudioRequest::setVolume(float volume) {
  this->volume = volume;
}
float SpatialAudioRequest::getPitch() const {
  return this->pitch;
}
float SpatialAudioRequest::getVolume() const {
  return this->volume;
}

void AudioChannel::reset() {
  stop();
  alSourcei(source, AL_BUFFER, 0);
  alSourcef(source, AL_ROLLOFF_FACTOR, 1.f);

  vec4 zero_vec = { 0.f, 0.f, 0.f, 0.f };
  alSourcefv(source, AL_POSITION, zero_vec);
  alSourcefv(source, AL_VELOCITY, zero_vec);
  alSourcefv(source, AL_DIRECTION, zero_vec);

  set_looping(false);
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
  if (!is_stopped()) {
    stop();
  }
  alSourcei(source, AL_BUFFER, buffer.originalBuffer);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to assign AL source to buffer {}", alGetErrorString(err));
  }
}
void AudioChannel::play() {
  check_if_initialized();

  alSourcePlay(source);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to play track {}", alGetErrorString(err));
  }
}
bool AudioChannel::is_currently_playing() {
  ALint state = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;

}
void AudioChannel::stop() {
  check_if_initialized();

  alSourceStop(source);
}
bool AudioChannel::is_stopped() {
  check_if_initialized();

  ALint sourceState = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
  return sourceState == AL_STOPPED;
}
void AudioChannel::pause() {
  check_if_initialized();

  alSourcePause(source);
}
bool AudioChannel::is_looping() {
  check_if_initialized();

  ALint looping = AL_FALSE;
  alGetSourcei(source, AL_LOOPING, &looping);
  return looping == AL_TRUE;
}
void AudioChannel::set_looping(bool should_loop) {
  check_if_initialized();

  ALint al_should_loop = should_loop ? AL_TRUE : AL_FALSE;
  alSourcei(source, AL_LOOPING, al_should_loop);
}
void AudioChannel::destroy() const {
  alDeleteSources(1, &source);
}
void AudioChannel::set_volume(float vol) {
  alSourcef(source, AL_GAIN, vol);
}
void AudioChannel::set_pitch(float pitch) {
  AL_PITCH;
  alSourcef(source, AL_PITCH, pitch);
}
void AudioChannel::check_if_initialized() const {
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
  if (!is_stopped()) {
    stop();
  }
  alSourcei(source, AL_BUFFER, buffer.monoBuffer);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to assign AL source to buffer {}", alGetErrorString(err));
  }
}
void SpatialAudioChannel::play() {
  check_if_initialized();

  alSourcePlay(source);
  if (alGetError() != AL_NO_ERROR) {
    auto err = alGetError();
    spdlog::error("Failed to play track {}", alGetErrorString(err));
  }
}
bool SpatialAudioChannel::is_currently_playing() {
  ALint state = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;

}
void SpatialAudioChannel::stop() {
  check_if_initialized();

  alSourceStop(source);
}
void SpatialAudioChannel::reset() {
  stop();
  alSourcei(source, AL_BUFFER, 0);
  alSourcef(source, AL_ROLLOFF_FACTOR, 1.f);

  vec4 zero_vec = { 0.f, 0.f, 0.f, 0.f };
  alSourcefv(source, AL_POSITION, zero_vec);
  alSourcefv(source, AL_VELOCITY, zero_vec);
  alSourcefv(source, AL_DIRECTION, zero_vec);

  set_looping(false);
}
bool SpatialAudioChannel::is_stopped() {
  check_if_initialized();

  ALint sourceState = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
  return sourceState == AL_STOPPED;
}
void SpatialAudioChannel::pause() {
  check_if_initialized();

  alSourcePause(source);
}
bool SpatialAudioChannel::is_looping() {
  check_if_initialized();

  ALint looping = AL_FALSE;
  alGetSourcei(source, AL_LOOPING, &looping);
  return looping == AL_TRUE;
}
void SpatialAudioChannel::set_looping(bool should_loop) {
  check_if_initialized();

  ALint al_should_loop = should_loop ? AL_TRUE : AL_FALSE;
  alSourcei(source, AL_LOOPING, al_should_loop);
}
void SpatialAudioChannel::destroy() const {
  alDeleteSources(1, &source);
}
void SpatialAudioChannel::set_volume(float vol) {
  alSourcef(source, AL_GAIN, vol);
}
void SpatialAudioChannel::set_pitch(float pitch) {
  AL_PITCH;
  alSourcef(source, AL_PITCH, pitch);
}
void SpatialAudioChannel::set_position(vec3 pos) {
  alSourcefv(source, AL_POSITION, pos);
  if (alGetError() != AL_NO_ERROR) {
    assert(false);
  }
}
void SpatialAudioChannel::set_velocity(vec3 vel) {
  alSourcefv(source, AL_VELOCITY, vel);
}
void SpatialAudioChannel::set_direction(vec3 dir) {
  alSourcefv(source, AL_DIRECTION, dir);
}
void SpatialAudioChannel::check_if_initialized() const {
  if (source == 0) {
    spdlog::warn("source is 0, did you forget to initialize it?");
  }
}
void SpatialAudioChannel::set_cone(float angleDeg, float outerVolume) {
  if (fabs(angleDeg) < 0.001f) {
    alSourcef(source, AL_CONE_OUTER_ANGLE, 360.f);
  }
  // Openal uses degrees
  alSourcef(source, AL_CONE_INNER_ANGLE, angleDeg);
  alSourcef(source, AL_CONE_OUTER_GAIN, outerVolume);
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

  for (auto tag : { AudioTag::NONE, AudioTag::VOICE, AudioTag::SOUND_EFFECTS, AudioTag::MUSIC }) {
    tagModifier[tag] = AudioTagParameters{};
  }
}
AudioManager::~AudioManager() {
  for (auto& spatialAudioChannel : freeSpatialAudioChannels) {
    SpatialAudioChannel{ spatialAudioChannel }.destroy();
  }
  freeSpatialAudioChannels.clear();
  for (auto& spatialAudioChannel : borrowedSpatialAudioChannels) {
    SpatialAudioChannel{ spatialAudioChannel }.destroy();
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
  void operator()(short* ptr) const {
    free(ptr);
  }
};

std::optional<AudioBuffer> AudioManager::getTrack(const char* track) {
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

  int samples = stb_vorbis_decode_memory(
    fileBuffer.data(), fileBuffer.size(), &spacialAudioChannels,
    &sampleRate, &hData
  );
  std::unique_ptr<short, DeleteStbVorbisData> data(hData);
  if (data == nullptr) {
    spdlog::error("Failed to open file {}", track);
    return std::nullopt;
  }
  auto format = AudioFormat::AUDIO_FORMAT_STEREO16;
  if (spacialAudioChannels == 1) {
    format = AudioFormat::AUDIO_FORMAT_MONO16;
  }

  std::span<uint8_t> samplesData(reinterpret_cast<uint8_t*>(data.get()), samples * sizeof(short));

  if (auto buffer = AudioBuffer::create(samplesData, format, samples, sampleRate)) {
    tracks[track] = *buffer;

    return *buffer;
  }

  return std::nullopt;
}
/*
std::optional<uint32_t> AudioManager::handleSpatialAudioRequest(SpatialAudioRequest request, vec3 pos) {
  uint32_t freeChannelIndex = 0;
  for (; freeChannelIndex < spacialAudioChannels.size(); ++freeChannelIndex) {
    if (!spacialAudioChannels[freeChannelIndex].is_currently_playing()) {
      break;
    }
  }
  if (freeChannelIndex == spacialAudioChannels.size()) {
    return std::nullopt;
  }

  SpatialAudioChannel& freeChannel = spacialAudioChannels[freeChannelIndex];
  AudioBuffer* buffer = nullptr;
  if (auto bufferLoc = tracks.find(request.trackName); bufferLoc != tracks.end()) {
    buffer = &bufferLoc->second;
  }
  if (!buffer) {
    return std::nullopt;
  }


  freeChannel.reset();
  freeChannel.set_looping(request.looping);
  freeChannel.assign(*buffer);
  freeChannel.set_position(pos);
  freeChannel.play();

  return freeChannelIndex;
}
*/
void AudioManager::playTrackBackground(const char* track) {
  if (auto trackLoc = tracks.find(track); trackLoc != tracks.end()) {
    background.assign(trackLoc->second);
    background.play();
    if (alGetError() != AL_NO_ERROR) {
      auto err = alGetError();
      spdlog::error("Failed to play track {} {}", track, alGetErrorString(err));
    }
  }
  else {
    spdlog::warn("Track could not be played track was not found {}", track);
  }
}

void AudioManager::deleteTrack(const char* track) {
  if (auto trackLoc = tracks.find(track); trackLoc != tracks.end()) {
    trackLoc->second.destroy();
    tracks.erase(trackLoc);
  }
  else {
    spdlog::error("Track {} was not found and could not be deleted", track);
  }
}


} // namespace Magnet


namespace std {
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
}