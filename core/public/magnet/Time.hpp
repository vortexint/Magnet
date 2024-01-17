#pragma once

#include <chrono>

namespace Magnet {

class Time {
  static std::chrono::high_resolution_clock::time_point currentTime, lastTime;
  static double deltaTime;

 public:
  static void Update();

  /**
   * @brief Get the current time
   * @return double 
   */
  static double getTime();

  /**
   * @brief Get the time between the last frame and the current frame
   * @return double 
   */
  static double getDelta();
};

} // namespace Magnet