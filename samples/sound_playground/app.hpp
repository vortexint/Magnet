#pragma once

#include <flecs.h>
#include <GLFW/glfw3.h>

#include <magnet/Application.hpp>
#include <magnet/AudioManager.hpp>
#include "imgui.h"

const char *const GAME_TITLE = "Sound Playground";

using namespace Magnet;


class App : public Magnet::Context {
  GLFWwindow* window;


  ArchiveManager *archiveMgr;
  std::vector<flecs::entity_t> environmentIds;
  std::vector<flecs::entity_t> sourceIds;
  std::vector<std::string> trackNames;

 
  static constexpr bool AUDIO_DEBUG_ENABLED = false;
  void audioDebugInit();
  void audioDebugInit(flecs::world &ecs);

  flecs::entity newAudioSource(flecs::world &ecs);
  flecs::entity newEnvironment(flecs::world &ecs, const char *);
 public:
  void init() override;
  void update() override;
};