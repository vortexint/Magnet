#pragma once

#include <queue>
#include <glad/glad.h>

namespace Magnet::Rendering {

enum class CommandType {
  Mesh,
  Texture
};

struct MeshCommand {
  GLuint vao;
  GLuint vertexCount;
};

struct TextureCommand {
  GLuint textureID;
};

struct Command {
  CommandType type;
  GLuint shaderID = 0;

};

std::queue<Command>& getCmdBuffer();

void submit(const Command& cmd);

}