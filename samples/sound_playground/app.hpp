#pragma once

#include <flecs.h>
#include <GLFW/glfw3.h>

#include <magnet/Application.hpp>
#include "imgui.h"

const char *const GAME_TITLE = "Sound Playground";

using namespace Magnet;

class App : public Magnet::Context {
  GLFWwindow* window;

  flecs::entity_t mainEntityId;

  ArchiveManager *archiveMgr;
  std::vector<flecs::entity_t> environmentIds;
  std::vector<flecs::entity_t> sourceIds;
  std::vector<std::string> trackNames;
 public:
  void init() override;
  void update() override;
};