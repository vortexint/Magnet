#pragma once

#include <flecs.h>

#include <magnet/ApplicationContext.hpp>
#include "imgui.h"

const char *const GAME_TITLE = "Sound Playground";

using namespace Magnet;

class Interface : public ProjectInterface {
  GLFWwindow* window;

  flecs::entity_t mainEntityId;
  flecs::entity_t listenerEntityId;

 public:
  void init() override;

  void update() override;
};