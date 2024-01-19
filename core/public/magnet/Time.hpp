#pragma once

namespace Magnet {

namespace Time {
  void Update();

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

} // namespace Time

} // namespace Magnet