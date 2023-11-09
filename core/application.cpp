#include "application.hpp"

Application::Application(AssetManager &asset_manager)
    : assetMgr_(asset_manager) {

}

void Application::Update() {
  sceneMgr_.Update();
}