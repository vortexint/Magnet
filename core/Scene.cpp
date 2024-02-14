#include <spdlog/spdlog.h>

#include <magnet/AudioManager.hpp>
#include <magnet/Components.hpp>
#include <magnet/Scene.hpp>

#include "GFX/Viewport.hpp"

namespace Magnet::Scene {
using namespace Components;

void setupECS(flecs::world& ecs) {
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

void progressECS(Context* context) {
  context->getECS().progress(Time::getDelta(context->getTimeState()));
}

}  // namespace Magnet::Scene