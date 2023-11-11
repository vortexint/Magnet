#include "application.hpp"

Application::Application(AssetManager &asset_manager,
                         SceneManager &scene_manager)
    : assetMgr_(asset_manager), sceneMgr_(scene_manager) {}

void Application::Update() {
  sceneMgr_.Update();
}