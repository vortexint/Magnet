#pragma once

#include <magnet/ApplicationContext.hpp>

const char *const GAME_TITLE = "Sandbox";

class Interface : public ProjectInterface {
 public:
  void init() override;

  void update() override;
};