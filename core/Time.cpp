#include <chrono>
#include <magnet/Time.hpp>

namespace Magnet::Time {

std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
std::chrono::system_clock::time_point lastTime = std::chrono::system_clock::now();
double deltaTime = 0.0;

void update() {
  currentTime = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = currentTime - lastTime;
  deltaTime = elapsed.count();
  lastTime = currentTime;
}

double getTime() {
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed =
    now - std::chrono::time_point<std::chrono::system_clock>();
  return elapsed.count();
}

double getDelta() { return deltaTime; }

}  // namespace Magnet::Time