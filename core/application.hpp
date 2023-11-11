#pragma once

#include "io/asset_mgr.hpp"
#include "scene_mgr.hpp"

class Application {
  AssetManager &assetMgr_;
  SceneManager &sceneMgr_;

public:
  Application(AssetManager &asset_manager,
              SceneManager &SceneManager);

  void Update();
};