#pragma once

#include <chrono>

namespace Magnet {

class Time {
  static std::chrono::high_resolution_clock::time_point currentTime, lastTime;
  static double deltaTime;

 public:
  static void Update();

  static double getTime();
  static double getDelta();
};

} // namespace Magnet