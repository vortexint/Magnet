#include <spdlog/spdlog.h>

#include <magnet/Components.hpp>
#include <magnet/Time.hpp>



namespace Magnet::Components {
  AudioSource::AudioSource() {
    reset();
  }
  void AudioSource::play_sound(
    const char* name, 
    AudioTag tag, 
    bool looping, 
    float volume,
    std::optional<AudioFilterDescription> desc,
    std::optional<EAXReverbDescription> reverb
  ) {
    stop();

    this->reverb = reverb;
    this->trackName = name;
    this->timestampStartedS = Magnet::Time::getTime();
    this->volume = volume;
    this->looping = looping;
    this->tag = tag;
    this->filterDesc = desc;

    this->playState = AudioSourcePlayState::REQUESTED_PLAY;
  }
  void AudioSource::play_sound(
    AudioBuffer* audioBuffer,
    AudioTag tag,
    bool looping,
    float volume,
    std::optional<AudioFilterDescription> desc,
    std::optional<EAXReverbDescription> reverb
  ) {
    stop();

    this->reverb = reverb;
    this->audioBuffer = audioBuffer;
    this->timestampStartedS = Magnet::Time::getTime();
    this->volume = volume;
    this->looping = looping;
    this->tag = tag;
    this->filterDesc = desc;

    this->playState = AudioSourcePlayState::REQUESTED_PLAY;
  }
  AudioSourcePlayState AudioSource::getPlayState() {
    return playState;
  }
  const char* AudioSource::getTrackName() {
    return trackName;
  }
  void AudioSource::reset() {
    playState = AudioSourcePlayState::STOPPED;
    channel = std::nullopt;
    filterDesc = std::nullopt;
    if (filter) {
      filter->destroy();
    }
    filter = std::nullopt;
    reverb = std::nullopt;
    if (effect) {
      effect->destroy();
      effect = std::nullopt;
    }
    
    audioBuffer = nullptr;
    trackName = nullptr;
    channel = std::nullopt;
    timestampStartedS = 0.0;

    tag = AudioTag::NONE;
    volume = 1.f;
    pitch = 1.f;
    cone = std::nullopt;
    looping = false;
  }

  void AudioSource::stop() {
    if (channel) {
      channel->stop();
    }

    reset();
  }

} // namespace Magnet