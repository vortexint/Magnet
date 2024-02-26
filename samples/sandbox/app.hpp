#pragma once

#include <spdlog/spdlog.h>

#include <magnet/Application.hpp>
#include <magnet/Library.hpp>

#include "imgui.h"

const char* const GAME_TITLE = "Sandbox";

using namespace Magnet;
using namespace Magnet::Library;

class App : public Magnet::Context {
  GLFWwindow* window;
  ArchiveManager archiveMgr;

 public:
  App() : archiveMgr(ARCH_data, ARCH_data_KEY) {}
  void init() override;

  void update() override;
};