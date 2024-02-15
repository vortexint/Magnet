#include <spdlog/spdlog.h>

#include <magnet/Components.hpp>
#include <magnet/Time.hpp>

namespace Magnet::Components {


bool operator==(const Filter &lhs, const Filter &rhs) { 
  if (lhs.type != rhs.type) {
    return false;
  }

  switch (lhs.type) {
  case FilterType::LOWPASS:
    return lhs.gain == rhs.gain &&
            lhs.gainHighFrequency == rhs.gainHighFrequency;
    break;
  case FilterType::HIGHPASS:
    return lhs.gain == rhs.gain && lhs.gainLowFrequency == rhs.gainLowFrequency;
    break;
  case FilterType::BANDPASS:
    return lhs.gain == rhs.gain && lhs.gainLowFrequency == rhs.gainLowFrequency;
    break;
  }

  return false;
}
bool operator!=(const Filter &lhs, const Filter &rhs) { return !(lhs == rhs); }

const std::pair<Components::AudioTag, const char *> TAG_NAMES[] = {
{Components::AudioTag::VOICE, "VOIC"},
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
         lhs.position[2] == rhs.position[1] &&
         lhs.position[3] == rhs.position[3] &&
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
void AudioSource::playSound(const char *trackName) {
    
  // TODO: Figure out if we need to reset all the data everytime a sound plays
  // Reset all the state in the AudioSource,
  // but keep the old request id for cleanup
  //auto oldRequestId = this->requestId;
  //*this = AudioSource{};

  this->trackName = trackName;
  this->state = AudioPlayState::REQUESTED_PLAY;
  //this->requestId = oldRequestId;
}
void AudioSource::stop() {
  this->state = AudioPlayState::STOPPED;
}

bool operator==(const AudioSource &lhs, const AudioSource &rhs) {
  return !(lhs != rhs);
}
bool operator!=(const AudioSource &lhs, const AudioSource& rhs) {
  bool trackNameDiff = lhs.trackName != rhs.trackName;

  bool audioBuffDiff = lhs.audioBuffer != rhs.audioBuffer;
  bool requestIdDiff = lhs.requestId != rhs.requestId;
  bool stateDiff = lhs.state != rhs.state;
  bool volumeDiff = lhs.volume != rhs.volume;
  bool pitchDiff = lhs.pitch != rhs.pitch;
  bool coneDiff = lhs.cone != rhs.cone;
  bool loopingDiff = lhs.looping != rhs.looping;
  bool isSpatialDiff = lhs.isSpatial != rhs.isSpatial;
  bool effectIdDiff = lhs.effectId != rhs.effectId;
  bool filterDiff = lhs.filter != rhs.filter;
  
  return trackNameDiff || audioBuffDiff || requestIdDiff || stateDiff ||
         volumeDiff || pitchDiff || coneDiff || loopingDiff || isSpatialDiff ||
         effectIdDiff || filterDiff;
}
} // namespace Magnet