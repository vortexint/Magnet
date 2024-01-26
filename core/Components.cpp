#include <spdlog/spdlog.h>

#include <magnet/Components.hpp>
#include <magnet/Time.hpp>



namespace Magnet::Components {
  std::optional<size_t> AudioSource::play_sound(const char* name, AudioTag tag, bool looping, float volume) {
    size_t i = 0;
    for (; i < requests.size(); ++i) {
      if (!requests[i].has_value()) {
        break;
      }
    }

    if (i == requests.size()) {
      spdlog::warn("AudioSource requests buffer was overrun no music will play");
      return std::nullopt;
    }

    SpatialAudioRequest request = {};

    request.trackName = name;
    request.timestampStartedS = Magnet::Time::getTime();
    request.volume = volume;
    request.looping = looping;
    request.tag = tag;

    requests[i] = request;

    return request.hash();
  }
  SpatialAudioRequest* AudioSource::getRequest(size_t requestId) {
    size_t i = 0;
    for (; i < requests.size(); ++i) {
      if (requests[i] && requests[i]->hash() == requestId) {
        return &*requests[i];
      }
    }
  
    return nullptr;
  }

  bool AudioSource::isRequestIdValid(size_t requestId) {
    return getRequest(requestId) != nullptr;
  }

} // namespace Magnet