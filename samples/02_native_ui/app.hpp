#pragma once

#include <magnet/Application.hpp>
#include "imgui.h"

using namespace Magnet;

class App : public Magnet::Context {

 public:
  void init() override;
  void update() override;
};