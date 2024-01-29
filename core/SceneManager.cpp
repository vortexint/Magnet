#include <spdlog/spdlog.h>

#include <magnet/Components.hpp>
#include <magnet/AudioManager.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/Time.hpp>

#include "GFX/Viewport.hpp"

namespace Magnet {
using namespace Components;

SceneManager::SceneManager() {
  ecs = std::make_unique<flecs::world>();

  /* Register Components */

  ecs->component<Transform>();
  ecs->component<Camera>();
  ecs->component<AudioSource>();

  /* Scene setup */
  flecs::entity activeCamera =
    ecs->entity("Camera").add<Camera>().add<Transform>();
    
  Viewport::setActiveCamera(activeCamera);

  ecs->system<Transform, AudioSource>().iter(AudioManager::AudioSourceSystem);
}

void SceneManager::progress() {

  ecs->progress(Time::getDelta());
}

}  // namespace Magnet