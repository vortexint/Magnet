#include <spdlog/spdlog.h>

#include <magnet/Library.hpp>

namespace Magnet::Asset {
  
ID Library::addAsset(Mimetype mimetype, const std::vector<uint8_t>& data) {
  return addAsset(AssetLoader(mimetype, data));
}

AssetPtr Library::getAsset(ID id) {
  auto asset = assets.find(id);
  if (asset == assets.end()) {
    spdlog::error("Failed to find asset {}", id);
    return nullptr;
  }
  return asset->second;
}

void Library::removeAsset(ID id) {
  auto removed = assets.erase(id);
  if (removed == 0) {
    spdlog::error("Failed to remove asset {}", id);
  }
}

}  // namespace Magnet::Asset