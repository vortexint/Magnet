#pragma once

namespace Magnet::Time {
  void update();

  /**
   * @brief Get the current time in seconds
   * @return double 
   */
  double getTime();

  /**
   * @brief Get the interval in seconds between the last frame and the current frame
   * @return double 
   */
  double getDelta();
} // namespace Magnet::Time