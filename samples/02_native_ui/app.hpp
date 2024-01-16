#pragma once

#include <magnet/ApplicationContext.hpp>
#include <magnet/UserInterface.hpp>

using namespace Magnet;

class Interface : public ProjectInterface {
  nk_context* ctx;

 public:
  void init() override;

  void update() override;
};