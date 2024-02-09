#include <magnet/Scene.hpp>
#include <spdlog/spdlog.h>

#include "qoi.h"
#include "stb_image.h"

namespace Magnet::Scene {

using AssetCache = std::unordered_map<AssetID, std::unique_ptr<Asset>>;

AssetCache& getAssetCache() {
  static AssetCache cache;
  return cache;
}

Asset* getAsset(AssetID id) {
  return nullptr;
}

} // namespace Magnet