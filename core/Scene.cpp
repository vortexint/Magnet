#include <spdlog/spdlog.h>

#include <magnet/AudioManager.hpp>
#include <magnet/Components.hpp>
#include <magnet/Scene.hpp>

#include "GFX/Viewport.hpp"

namespace Magnet::Scene {
using namespace Components;

flecs::world& getECS() {
  static flecs::world ecs;
  return ecs;
}

void setupECS() {
  flecs::world& ecs = getECS();

  /* Register Components */

  ecs.component<Transform>();
  ecs.component<Camera>();
  ecs.component<AudioSource>();

  /* Scene setup */
  flecs::entity activeCamera =
    ecs.entity("Camera").add<Camera>().add<Transform>();

  Viewport::setActiveCamera(activeCamera);

  ecs.system<Transform, AudioSource>().iter(AudioManager::AudioSourceSystem);
}

void progressECS(const Time::TimeState& state) {
  getECS().progress(Time::getDelta(state));
}

}  // namespace Magnet::Scene