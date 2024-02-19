#pragma once

#include <glad/glad.h>

#include <magnet/ArchiveManager.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace Magnet::Library {

struct Texture {
  int width = 0, height = 0, channels = 0;
  unsigned char* data = nullptr;
  GLuint id = 0;
  ~Texture();
};
struct Model {
  GLuint vao = 0, vbo = 0, ebo = 0;
  ~Model();
};

enum class AssetType {
  Texture,
  Model,
  Audio,
};

struct Asset {
  AssetType type;
  std::variant<Texture, Model> data;
};

using ID = uint32_t;
using AssetsList = std::unordered_map<ID, Asset>;

enum class Mimetype : uint8_t {
  EXR,   // OpenEXR
  GLTF,  // GL Transmission Format
  GLB,   // glTF Binary
  PNG,   // Portable Network Graphics
  QOI,   // Quite OK Image format
};

AssetsList& getAllAssets();
namespace Loader {
// AssetType is determined by mimetype

ID generateAsset(Mimetype mimetype, const std::vector<uint8_t>& data);
ID generateAsset(Mimetype mimetype, ArchiveManager& archiveMgr,
                 const std::string& filename);

}  // namespace Loader

ID addAsset(Asset asset);
Asset getAsset(ID);

void removeAsset(ID);

}  // namespace Magnet::Library
