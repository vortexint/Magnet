#pragma once

#include <flecs.h>
#include <glad/glad.h>

#include <memory>
#include <vector>

#include <magnet/Application.hpp>
namespace Magnet::Scene {

/* ECS */

void setupECS(flecs::world& ecs);
void progressECS(Context* context);

}  // namespace Magnet::Scene