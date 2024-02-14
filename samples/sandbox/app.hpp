#pragma once

#include <magnet/Application.hpp>

#include <GLFW/glfw3.h>
#include "imgui.h"

const char *const GAME_TITLE = "Sandbox";

using namespace Magnet;

class App : public Magnet::Context {
  GLFWwindow* window;

 public:
  void init() override;

  void update() override;
};