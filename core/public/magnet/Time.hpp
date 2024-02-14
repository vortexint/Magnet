#pragma once
#include <chrono>
namespace Magnet::Time {
struct TimeState {
  std::chrono::system_clock::time_point lastTime =
    std::chrono::system_clock::now();
  double deltaTime = 0.0;
};

// Update a time state
void update(TimeState& state);

// Get the time in seconds since epoch
double getTime();

// Get interval between the last two updates
double getDelta(const TimeState& state);
float getFPS(const TimeState& state);

}  // namespace Magnet::Time