#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <magnet/ArchiveManager.hpp>

namespace Magnet::Asset {

// clang-format off
struct AssetData {virtual ~AssetData() = default;};
struct Texture : public AssetData {int width, height, channels; std::unique_ptr<uint8_t[]> data;};
struct Shader  : public AssetData {std::string vertexSource, fragmentSource;};
struct Sound   : public AssetData {std::unique_ptr<uint8_t[]> data;};
struct Font    : public AssetData {std::unique_ptr<uint8_t[]> data;};
struct Script  : public AssetData {std::string source;};
// clang-format on

using AssetID = size_t;  // trivially-copyable

struct Asset {
  AssetID id;
  std::unique_ptr<AssetData> data;
};

// owned by app context
class Library {
  std::unordered_map<AssetID, std::shared_ptr<Asset>> assets;

 public:
  template <typename T>
  AssetID addAsset(ArchiveManager& AchiveMgr, const std::string& filename);

  void removeAsset(AssetID id);

  std::shared_ptr<Asset> getAsset(AssetID id);
};

}  // namespace Magnet::Asset
