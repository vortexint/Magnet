#pragma once

#include <magnet/ApplicationContext.hpp>
#include "imgui.h"

using namespace Magnet;

class Interface : public ProjectInterface {

 public:
  void init() override;

  void update() override;
};