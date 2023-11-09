#pragma once

#include "application.hpp"

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

const char *const GAME_TITLE = "Sandbox";
const char *const SECURE_ASSETS_ARCHIVE = "data";

class GameContext : public Application {
public:
  GameContext(AssetManager &asset_manager) : Application(asset_manager) {
    // Additional initialization if needed
  }

  // Implement specific functionalities for the GameContext
  // You can override the base class methods as needed
};