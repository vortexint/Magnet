#pragma once

#include <queue>

namespace Magnet::Rendering {

enum class CommandType {
  Mesh,
  Texture
};

struct Command {
  CommandType type;

};

std::queue<Command>& getCmdBuffer();

void submit(const Command& cmd);

}