#include <spdlog/spdlog.h>
#include <glad/glad.h>

#include <magnet/Library.hpp>

namespace Magnet::Library {

AssetsList& getAllAssets() {
  static AssetsList assets;
  return assets;
}

ID addAsset(Asset asset) {
  static ID current_id = 0;
  AssetsList& assets = getAllAssets();
  assets[++current_id] = std::make_shared<Asset>(std::move(asset));
  return current_id;
}

std::shared_ptr<Asset> getAsset(ID id) {
  AssetsList& assets = getAllAssets();
  return assets.at(id);
}

void removeAsset(ID id) {
  AssetsList& assets = getAllAssets();
  assets.erase(id);
}

}  // namespace Magnet::Library