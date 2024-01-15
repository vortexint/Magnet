

#include <magnet/Time.hpp>

namespace Magnet {

auto Time::currentTime = std::chrono::high_resolution_clock::now();
auto Time::lastTime = std::chrono::high_resolution_clock::now();
double Time::deltaTime = 0.0;

void Time::Update() {
  currentTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = currentTime - lastTime;
  deltaTime = elapsed.count();
  lastTime = currentTime;
}

double Time::getTime() {
  auto now = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed =
    now - std::chrono::time_point<std::chrono::high_resolution_clock>();
  return elapsed.count();
}
double Time::getDelta() { return deltaTime; }

} // namespace Magnet