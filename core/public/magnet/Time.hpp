#pragma once

namespace Magnet {

/**
 * @brief Get the current time since the epoch in milliseconds.
 *
 * This function uses the std::chrono library to get the current time from the
 * system clock, and then converts it to a duration since the epoch in
 * milliseconds.
 *
 * @return The current time since the epoch in milliseconds as a double.
 */
double getTime();
} // namespace Magnet