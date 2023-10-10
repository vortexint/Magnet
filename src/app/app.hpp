#pragma once

#include "core/io/asset_manager.hpp"

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

const char *const GAME_TITLE = "Uniforge";
const char *const SECURE_ASSETS_ARCHIVE = "data";

class GameContext {
  AssetManager &assetMgr_;

public:
  GameContext(AssetManager &asset_manager) : assetMgr_(asset_manager) {}
};