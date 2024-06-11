#include <spdlog/spdlog.h>

#include <magnet/Components.hpp>
#include <magnet/Time.hpp>
#include <magnet/AudioManager.hpp>

namespace Magnet::Components {
const std::pair<Components::AudioTag, const char *> TAG_NAMES[] = {
{Components::AudioTag::VOICE, "VOICE"},
{Components::AudioTag::SOUND_EFFECTS, "SOUND_EFFECTS"},
{Components::AudioTag::MUSIC, "MUSIC"},
{Components::AudioTag::NONE, "NONE"}};

std::optional<AudioTag> from_string(const std::string &s) {
  for (const auto &[tag, name] : TAG_NAMES) {
    if (s == name) {
      return tag;
    }
  }
  return std::nullopt;
}
std::string to_string(AudioTag hTag) {
  for (const auto &[tag, name] : TAG_NAMES) {
    if (hTag == tag) {
      return name;
    }
  }
  assert(false);
  return "";
}
std::span<const AudioTag> allAudioTags() {
  static const AudioTag tags[] = {AudioTag::VOICE, AudioTag::SOUND_EFFECTS,
                           AudioTag::MUSIC, AudioTag::NONE};
  std::span<const AudioTag> tagSpan(tags, sizeof(tags) / sizeof(tags[0]));

  return tagSpan;
}

const bool AudioSource::isSpatial() const { return mIsSpatial; }
void AudioSource::playSound(const char *trackName, bool isSpatial) {
    
  // TODO: Figure out if we need to reset all the data everytime a sound plays
  // Reset all the state in the AudioSource,
  // but keep the old request id for cleanup
  

  this->mIsSpatial = isSpatial;
  this->trackName = trackName;
  this->state = AudioPlayState::REQUESTED_PLAY;
}
void AudioSource::stop() {
  this->state = AudioPlayState::STOPPED;
}
} // namespace Magnet