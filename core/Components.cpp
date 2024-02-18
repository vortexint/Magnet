#include <spdlog/spdlog.h>

#include <magnet/Components.hpp>
#include <magnet/Time.hpp>
#include <magnet/AudioManager.hpp>

namespace Magnet::Components {


bool operator==(const Filter &lhs, const Filter &rhs) { 
  return lhs.type == rhs.type && lhs.gain == rhs.gain &&
         lhs.gainHighFrequency == rhs.gainHighFrequency &&
         lhs.gainLowFrequency == rhs.gainLowFrequency;

  return false;
}
bool operator!=(const Filter &lhs, const Filter &rhs) { return !(lhs == rhs); }

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

bool operator==(const Transform &lhs, const Transform &rhs) {
  return lhs.position[0] == rhs.position[0] &&
         lhs.position[1] == rhs.position[1] &&
         lhs.position[2] == rhs.position[2] &&
         lhs.rotation[0] == rhs.rotation[0] &&
         lhs.rotation[1] == rhs.rotation[1] &&
         lhs.rotation[2] == rhs.rotation[2] &&
         lhs.rotation[3] == rhs.rotation[3] &&
         lhs.scale[0] == rhs.scale[0] &&
         lhs.scale[1] == rhs.scale[1] && 
         lhs.scale[2] == rhs.scale[2];
}
bool operator!=(const Transform &lhs, const Transform &rhs) {
  return !(lhs == rhs);
}

bool operator==(const Cone &lhs, const Cone &rhs) {
  return lhs.angleDeg == rhs.angleDeg && lhs.outerGain == rhs.outerGain;
}
bool operator!=(const Cone& lhs, const Cone &rhs) { return !(lhs == rhs); }

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

bool operator==(const AudioSource &lhs, const AudioSource &rhs) {
  return !(lhs != rhs);
}
bool operator!=(const AudioSource &lhs, const AudioSource& rhs) {
  bool trackNameDiff = lhs.trackName != rhs.trackName;

  bool requestIdDiff = lhs.requestId != rhs.requestId;
  bool stateDiff = lhs.state != rhs.state;
  bool volumeDiff = lhs.volume != rhs.volume;
  bool pitchDiff = lhs.pitch != rhs.pitch;
  bool coneDiff = lhs.cone != rhs.cone;
  bool loopingDiff = lhs.looping != rhs.looping;
  bool isSpatialDiff = lhs.mIsSpatial != rhs.mIsSpatial;
  bool filterDiff = lhs.filter != rhs.filter;
  
  return trackNameDiff || requestIdDiff || stateDiff ||
         volumeDiff || pitchDiff || coneDiff || loopingDiff || isSpatialDiff ||
         filterDiff;
}
} // namespace Magnet