#pragma once

#include <magnet/Application.hpp>

#include <cstdint>

namespace Magnet::Window {

constexpr uint32_t INIT_WIDTH = 1280;
constexpr uint32_t INIT_HEIGHT = 720;

void initialize(Magnet::Context& context, const char* title);

}  // namespace Magnet::Window