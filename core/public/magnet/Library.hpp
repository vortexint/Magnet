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
  QOI,   // Quite OK Image format
};

using ID = uint32_t;
using Ptr = std::shared_ptr<IAsset>;
using List = std::map<ID, Ptr>;

namespace Loader {

std::unique_ptr<IAsset> genAsset(Mimetype mimetype,
                                 const std::vector<uint8_t>& data);
                                 
std::unique_ptr<IAsset> genAsset(Mimetype mimetype, const std::string& path,
                                 ArchiveManager& archive);

}  // namespace Loader

// owned by app context
class Library {
  List assets;
  ID nextID = 1;

 public:
  // Adds Asset to the Library
  ID addAsset(std::unique_ptr<IAsset> asset);

  void removeAsset(ID);

  Ptr getAsset(ID);
  const List& getAllAssets();
};

}  // namespace Magnet::Asset
