#pragma once

#include <glad/glad.h>

#include <magnet/ArchiveManager.hpp>
#include <map>
#include <memory>
#include <string>

namespace Magnet::Asset {

struct IAsset {
  virtual ~IAsset() = default;
};

struct Texture : public IAsset {
  int width = 0, height = 0, channels = 0;
  GLuint id = 0;
  virtual ~Texture();
};

struct Model : public IAsset {
  GLuint vao = 0, vbo = 0, ebo = 0;
  virtual ~Model();
};

enum class Mimetype : uint8_t {
  EXR,   // OpenEXR
  GLTF,  // GL Transmission Format
  GLB,   // glTF Binary
  PNG,   // Portable Network Graphics
  QOI,   // Quite Ok Image format
};

using ID = uint32_t;
using AssetPtr = std::shared_ptr<IAsset>;
using AssetsList = std::map<ID, AssetPtr>;

std::unique_ptr<IAsset> AssetLoader(Mimetype mimetype,
                                    const std::vector<uint8_t>& data);

// owned by app context
class Library {
  AssetsList assets;
  ID nextID = 1;

 public:
  ID addAsset(std::unique_ptr<IAsset> asset);
  ID addAsset(Mimetype mimetype, const std::vector<uint8_t>& data);
  ID addAsset(Mimetype mimetype, const std::string& path,
              ArchiveManager& archive);
  void removeAsset(ID);

  AssetPtr getAsset(ID);
  const AssetsList& getAllAssets() const { return assets; }
};

}  // namespace Magnet::Asset
