#pragma once

#include <cstdint>

// Forward declarations
struct GLFWwindow;
namespace Magnet::Window {

const uint32_t INIT_WIDTH = 1280;
const uint32_t INIT_HEIGHT = 720;

void initialize(const char* title);
void shutdown();

void swapBuffers();
void pollEvents();

bool shouldClose();

}  // namespace Magnet::Window