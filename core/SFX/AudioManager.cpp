#include <spdlog/spdlog.h>

#include <SFX/ALBackend.hpp>
#include <SFX/EfxPresetList.hpp>
#include <magnet/AudioManager.hpp>
#include <magnet/Components.hpp>
#include <magnet/Scene.hpp>
#include <magnet/Geometry.hpp>

#define STB_VORBIS_IMPLEMENTATION
#include <stb_vorbis.h>

namespace Magnet {

EffectDescription::EffectDescription(ReverbDescription reverb)
  : type(EffectType::REVERB), reverb{} {
  this->type = EffectType::REVERB;
  this->reverb = reverb;
}

size_t FormatBytesPerSample(AudioFormat format) {
  switch (format) {
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
  assert(false);
  return 0;
}

VolumeAdjuster::VolumeAdjuster() : masterVolume(1.f) {
  for (auto tag : Components::allAudioTags()) {
    volumes[tag] = 1.f;
  }
}

AudioManager::AudioManager()
  : al(new ALBackend()) {

}
AudioManager::~AudioManager() { delete al; }

void AudioManager::setupECS(flecs::world* ecs) { this->ecs = ecs; }
VolumeAdjuster& AudioManager::volumes() { return volumeAdjustor; }
void AudioManager::handleRequestedPlay(Components::AudioSource &source, 
                                      const Components::Transform &transform, 
                                      const Components::Environment &environment) {
  assert(source.trackName != nullptr ^ source.audioBuffer != nullptr);

  if (source.requestId) {
    al->freeRequest(*source.requestId);
    source.requestId = std::nullopt;
  }


  source.requestId = al->newRequest();
  if (source.requestId == std::nullopt) {
    spdlog::warn("Could not play sound newAudioRequest failed");
    return;
  }

  std::optional<ALBuffer> buffer = std::nullopt;
  if (source.trackName) {
    if (source.isSpatial) {
      buffer = al->getMonoBuffer(source.trackName);
    } else {
      buffer = al->getBuffer(source.trackName);
    }
    source.trackName = nullptr;
  } else if (source.audioBuffer) {
    buffer = *source.audioBuffer;
    source.audioBuffer = nullptr;
  }

  if (buffer == std::nullopt) {
    spdlog::error("Failed to get track");
    source.state = Components::AudioPlayState::STOPPED;
    return;
  }

  if (auto *request = al->getRequest(source.requestId.value()); request && buffer) {
    al->setParameters(source.requestId.value(), source, transform, environment, volumeAdjustor);
    request->source.setBuffer(*buffer);
    request->source.play();
  }
  source.state = Components::AudioPlayState::PLAYING;

  assert(source.trackName == nullptr);
  assert(source.audioBuffer == nullptr);
  assert(source.requestId);
}
void AudioManager::handlePlaying(Components::AudioSource &source, 
                                const Components::Transform &transform,
                                const Components::Environment &environment) {
  assert(source.trackName == nullptr);
  assert(source.audioBuffer == nullptr);
  assert(source.requestId);

  if (auto *request = al->getRequest(source.requestId.value())) {
    al->setParameters(source.requestId.value(), source, transform,
                           environment, volumeAdjustor);

    if (request->source.isStopped()) {
      source.state = Components::AudioPlayState::STOPPED;
    }
  }

}
void AudioManager::handleStopped(Components::AudioSource &source, 
                                const Components::Transform &transform,
                                const Components::Environment &environment) {
  if (source.requestId) {
    al->freeRequest(*source.requestId);
    source.requestId = std::nullopt;
  }
}

void AudioManager::AudioSourceSystem(flecs::iter &iter, Components::Transform *transforms, Components::AudioSource *sources) {
  auto &audioMgr = AudioManager::getInstance();
  
  
  flecs::filter<Components::Transform, Components::Environment> environments_query =
    audioMgr.ecs->filter<Components::Transform, Components::Environment>();

  std::vector<std::pair<Components::Transform, Components::Environment>>
    environments;
  const int ENV_DISTANCE = 1000;
  vec3 listenerPos = { 0.f, 0.f, 0.f };
  ALBackend::getListenerPos(listenerPos);
  environments_query.iter(
    [&listenerPos, ENV_DISTANCE, &environments](flecs::iter& it, Components::Transform* transform,
      Components::Environment* environment) {
      for (size_t row : it) {
        AABB envBoundingBox = AABB::fromTransform(transform[row]);
        bool cornerIsWithinDistance = false;
        for (auto corner : envBoundingBox.corners()) {
          cornerIsWithinDistance |= 
            glm_vec3_distance2(corner.data(), listenerPos) <= ENV_DISTANCE * ENV_DISTANCE;
        }
  
        if (cornerIsWithinDistance) {
          environments.push_back(std::make_pair(transform[row], environment[row]));
        }
      }
  });

  auto findEnvironment = [&](vec3 pos)->std::optional < Components::Environment > {
    for (auto &[transform, environment]: environments) {
      auto aabb = AABB::fromPositionAndSize(transform.position, transform.scale);
      if (aabb.rotatedAABBContains(transform.rotation, pos)) {
        return environment;
      }
    }

    return std::nullopt;
  };

  for (size_t i: iter) {
    auto &transform = transforms[i];
    auto &source = sources[i];

    Components::Environment environment;
    if (auto environmentOpt = findEnvironment(transform.position)) {
      environment = *environmentOpt;
    }

    switch (source.state) {
      case Components::AudioPlayState::REQUESTED_PLAY:
        audioMgr.handleRequestedPlay(source, transform, environment);
        break;
      case Components::AudioPlayState::PLAYING:
        audioMgr.handlePlaying(source, transform, environment);
        break;
      case Components::AudioPlayState::STOPPED:
        audioMgr.handleStopped(source, transform, environment);
        break;
    }
  }
}

std::optional<RawAudioData> AudioManager::LoadAudio(std::span<const uint8_t> rawFileData) { 
  int spacialAudioChannels = 0, sampleRate = 0;
  short* hData = nullptr;
  int samples =
    stb_vorbis_decode_memory(rawFileData.data(), rawFileData.size(),
                             &spacialAudioChannels, &sampleRate, &hData);

  if (hData == nullptr) {
    spdlog::error("Could not parse audio file data");
    return std::nullopt;
  }

  AudioFormat format = AudioFormat::AUDIO_FORMAT_STEREO16;
  if (spacialAudioChannels == 1) {
    format = AudioFormat::AUDIO_FORMAT_MONO16;
  }
  size_t rawDataByteSize = FormatBytesPerSample(format) * samples;

  std::span<const uint8_t> bytes(
    static_cast<uint8_t*>(static_cast<void*>(hData)),
    rawDataByteSize);

  return RawAudioData(
    format,
    std::vector<uint8_t>(bytes.begin(), bytes.end()),
    sampleRate
  );
}
void AudioManager::registerAudio(const std::string &name, std::span < const uint8_t > data, AudioFormat format, size_t sampleRate) {
  al->genBuffers(name, data, format, sampleRate);
}
void AudioManager::registerEffect(const std::string& name, EffectDescription desc) {
  al->registerEffect(name, desc);
}
void AudioManager::registerAllEfxPresets() {
  for (auto &[name, reverb] : getEFXPresets()) {
    this->registerEffect(name, reverb);
  }
}


void AudioManager::getListenerPos(vec3 pos) { ALBackend::getListenerPos(pos); }
void AudioManager::setListenerPos(const vec3 pos) {
  vec3 posCopy = {pos[0], pos[1], pos[2]};

  ALBackend::setListenerPos(posCopy);
}

} // namespace Magnet