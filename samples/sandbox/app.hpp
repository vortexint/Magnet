#pragma once

#include <magnet/ApplicationContext.hpp>
#include "imgui.h"

const char *const GAME_TITLE = "Sandbox";

using namespace Magnet;

class Interface : public ProjectInterface {
  GLFWwindow* window;

 public:
  void init() override;

  void update() override;
};