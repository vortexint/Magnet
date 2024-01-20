#pragma once

#include <magnet/ApplicationContext.hpp>
#include <magnet/UserInterface.hpp>

using namespace Magnet;

class Interface : public ProjectInterface {

 public:
  void init() override;

  void update() override;
};