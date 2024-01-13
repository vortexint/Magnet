#pragma once

#include <magnet/ApplicationContext.hpp>
#include <magnet/UserInterface.hpp>

const char *const GAME_TITLE = "Sandbox";

class Interface : public ProjectInterface {
  GLFWwindow* window;
  nk_context* ctx;

 public:
  void init() override;

  void update() override;
};