#include <magnet/Time.hpp>

namespace Magnet::Time {

void update(TimeState& state) {
  state.currentTime = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = state.currentTime - state.lastTime;
  state.deltaTime = elapsed.count();
  state.lastTime = state.currentTime;
}

double getTime() {
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed =
    now - std::chrono::time_point<std::chrono::system_clock>();
  return elapsed.count();
}

double getDelta(const TimeState& state) { return state.deltaTime; }

}  // namespace Magnet::Time
