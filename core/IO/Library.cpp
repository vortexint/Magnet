#include <spdlog/spdlog.h>

#include <magnet/Library.hpp>

namespace Magnet::Asset {

ID Library::addAsset(std::unique_ptr<IAsset> asset) {
  ID id = nextID++;
  assets[id] = std::move(asset);
  return id;
}

Ptr Library::getAsset(ID id) {
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