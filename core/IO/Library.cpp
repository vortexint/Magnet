#include <spdlog/spdlog.h>

#include <magnet/Library.hpp>

namespace Magnet::Library {

AssetsList& getAllAssets() {
  static AssetsList assets;
  return assets;
}

ID addAsset(Asset asset) {
  static ID id = 0;
  AssetsList& assets = getAllAssets();
  assets[id] = std::move(asset);
  return id++;
}

Asset getAsset(ID id) {
  AssetsList& assets = getAllAssets();
  return assets[id];
}

void removeAsset(ID id) {
  AssetsList& assets = getAllAssets();
  assets.erase(id);
}


}  // namespace Magnet::Library