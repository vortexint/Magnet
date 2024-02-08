#pragma once

#include <flecs.h>
#include <glad/glad.h>

#include <memory>
#include <vector>

namespace Magnet::Time {
// Forward declaration
struct TimeState;
}

namespace Magnet::Scene {

/* ECS */

flecs::world& getECS();
void setupECS();
void progressECS(const Time::TimeState& state);

/* Asset Factory */

// The user will not define assets, this is for reading only
struct Asset {
  virtual ~Asset() = default;
  virtual void load() = 0;
};

enum class AssetType {IMAGE,SOUND,SCRIPT,VIDEO,FONT,};

struct Shader : public Asset {
  GLuint id;
  std::string vertexSource, fragmentSource;

  void load() override;
};

struct Texture : public Asset {
  int width, height, channels;
  std::vector<uint8_t> data;

  void load() override;
};

using AssetID = size_t;

AssetID loadAsset();

}  // namespace Magnet::Scene