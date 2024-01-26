#include <magnet/Components.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/Time.hpp>

namespace Magnet {
using namespace Magnet::Components;

SceneManager::SceneManager() {
  ecs = std::make_unique<flecs::world>();

  /* Scene setup */
  activeCamera = ecs->entity();
  activeCamera.set<Transform>({}).set<Camera>({});

  ecs->component<Components::AudioListener>();
  ecs->component<Components::AudioSource>();
  ecs->system<Components::Transform, Components::AudioSource>()
    .iter(AudioManager::AudioSourceSystem);
  ecs->system<Components::Transform, Components::AudioListener>()
    .iter(AudioManager::AudioListenerSystem);
}

void SceneManager::progress() { ecs->progress(Time::getDelta()); }

void SceneManager::setActiveCamera(flecs::entity camera) {
  activeCamera = camera;
}

}  // namespace Magnet