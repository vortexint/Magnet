#pragma once

#include <magnet/Application.hpp>
#include "imgui.h"

using namespace Magnet;

class Interface : public ProjectInterface {

 public:
  void init() override;

  void update() override;
};