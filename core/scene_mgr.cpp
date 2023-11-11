#include "scene_mgr.hpp"
#include "components.hpp"

SceneManager::SceneManager() {
  ecs_.component<magnetar::Camera>("Camera");
  ecs_.component<magnetar::Rigidbody>("Rigidbody");
  ecs_.component<magnetar::Transform>("Transform");

  flecs::entity camera =
      ecs_.entity().add<magnetar::Transform>().add<magnetar::Camera>();
}

void SceneManager::Update() { ecs_.progress(); }