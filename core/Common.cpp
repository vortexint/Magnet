#include <chrono>
#include <magnet/Time.hpp>

namespace Magnet {

double getTime() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

}