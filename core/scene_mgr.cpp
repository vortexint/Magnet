#include "scene_mgr.hpp"
#include "components.hpp"


SceneManager::SceneManager() {
  
}

void SceneManager::Update() { ecs.progress(); }