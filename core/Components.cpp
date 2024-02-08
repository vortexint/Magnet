#include <spdlog/spdlog.h>

#include <magnet/Components.hpp>
#include <magnet/Time.hpp>

namespace Magnet::Components {
  AudioSource::AudioSource() {
    reset();
  }
  void AudioSource::play_sound(const char* name, AudioTag tag, bool looping, float volume) {
    stop();

    trackName = name;

    timestampStartedS = Magnet::Time::getTime();
    this->volume = volume;
    this->looping = looping;
    this->tag = tag;

    playState = AudioSourcePlayState::REQUESTED_PLAY;
  }
  void AudioSource::reset() {
    channel = std::nullopt;
    playState = AudioSourcePlayState::STOPPED;

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