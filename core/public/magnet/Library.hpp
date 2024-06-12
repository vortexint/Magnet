#pragma once

#include <future>
#include <magnet/ArchiveManager.hpp>
#include <memory>
#include <string>

#include "tiny_gltf.h"  // includes stb_image.h, won't question it! :D
namespace Magnet::Library {

enum class AssetStatus {
  Idle,
  Loading,
  Loaded,
  Error,
};

enum class AssetType : uint8_t {
  Texture,
  Mesh,
  Audio,
  Model
};

enum class Mimetype : uint8_t {
  /* Texture */
  EXR,  // OpenEXR
  PNG,  // Portable Network Graphics
  QOI,  // Quite OK Image format
  /* Model */
  GLB,   // glTF Binary
  GLTF,  // GL Transmission Format
  /* Audio */
  OGG,  // Ogg Vorbis
  WAV,  // Waveform Audio File Format
};

class Asset {
 public:
  virtual ~Asset() = default;
  virtual void load(Mimetype mimetype, const uint8_t* inputData,
                    size_t size) = 0;
  AssetStatus getStatus() const;

 protected:
  AssetStatus status = AssetStatus::Idle;
};

struct Texture : public Asset {
 public:
  int width = 0, height = 0, channels = 0;
  uint8_t* data;

  ~Texture();
  void load(Mimetype mimetype, const uint8_t* inputData, size_t size) override;
};

/*
struct Model : public Asset {
 public:
  tinygltf::Model model;

  ~Model();
  void load(Mimetype mimetype, const uint8_t* inputData, size_t size) override;
};
*/

struct Audio : public Asset {
 public:
  uint8_t* data;

  ~Audio();
  void load(Mimetype mimetype, const uint8_t* inputData, size_t size) override;
};


using ID = uint16_t;
constexpr size_t MaxAssets = 1024;

struct AssetHolder {
  std::unique_ptr<Asset> asset;
  AssetType type;
  AssetStatus status = AssetStatus::Idle;
  std::future<void> loadFuture;
};

ID enqueueLoad(Mimetype mimetype, std::vector<uint8_t> data);
ID enqueueLoad(Mimetype mimetype, ArchiveManager& archiveMgr,
               const std::string& filename);

bool isAssetLoaded(ID id);
Asset* getAsset(ID id);
void removeAsset(ID);

}  // namespace Magnet::Library
