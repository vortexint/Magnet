#include <spdlog/spdlog.h>

#include <bitset>
#include <magnet/Library.hpp>

namespace Magnet::Library {

std::bitset<MaxAssets> assetSlots;
std::array<AssetHolder, MaxAssets> assets;
std::mutex assetMutex;

ID generateID() {
  // Try to find a free slot.
  for (ID i = 0; i < MaxAssets; ++i) {
    if (!assetSlots[i]) {    // If the slot is free
      assetSlots[i] = true;  // Occupy the slot
      return i;              // Return the ID (which is the array index)
    }
  }

  // If all slots are occupied.
  throw std::runtime_error("Exceeded asset limit");
}

Asset* getDummyAsset(AssetType type) {
  // Assume we have predefined dummy assets assigned the first few IDs.
  switch (type) {
    case AssetType::Texture:
      return static_cast<Asset*>(assets[0].asset.get());
    case AssetType::Model:
      return static_cast<Asset*>(assets[1].asset.get());
    case AssetType::Audio:
      return static_cast<Asset*>(assets[2].asset.get());
    default:
      throw std::invalid_argument("Unsupported asset type");
  }
}

ID enqueueLoad(Mimetype mimetype, std::vector<uint8_t> data) {
  std::lock_guard<std::mutex> lock(assetMutex);

  if (assets.size() > MaxAssets)
    throw std::runtime_error("Exceeded asset limit");

  ID newID = generateID();
  AssetHolder holder;

  switch (mimetype) {
    case Mimetype::EXR:
    case Mimetype::PNG:
    case Mimetype::QOI:
      holder.asset = std::make_unique<Texture>();
      break;
    case Mimetype::GLB:
    case Mimetype::GLTF:
      holder.asset = std::make_unique<Model>();
      break;
    case Mimetype::OGG:
    case Mimetype::WAV:
      holder.asset = std::make_unique<Audio>();
      break;
    default:
      throw std::invalid_argument("Unsupported mimetype");
  }

  assets[newID] = std::move(holder);

  assets[newID].status = AssetStatus::Loading;
  assets[newID].loadFuture =
    std::async(std::launch::async, [data = std::move(data), newID, mimetype]() {
      try {
        assets[newID].asset->load(mimetype, data.data(), data.size());
        assets[newID].status = AssetStatus::Loaded;
      } catch (const std::exception& e) {
        assets[newID].status = AssetStatus::Error;
        spdlog::error("Failed to load asset: {}", e.what());
      }
    });

  return newID;
}

ID enqueueLoad(Mimetype mimetype, ArchiveManager& archiveMgr,
               const std::string& filename) {
  std::vector<uint8_t> buffer;
  archiveMgr.loadFile(filename, buffer);

  return enqueueLoad(mimetype, std::move(buffer));
}

Asset* getAsset(ID id) {
  std::lock_guard<std::mutex> lock(assetMutex);

  if (id >= MaxAssets || !assetSlots[id]) {  // Fixed the condition to check if
                                             // the slot is occupied.
    spdlog::error("Invalid asset ID: {}", id);
    return nullptr;
  }

  AssetHolder& holder = assets[id];

  if (holder.status == AssetStatus::Error) {
    spdlog::error("Asset failed to load");
    return nullptr;
  }

  if (holder.loadFuture.valid() &&
      holder.loadFuture.wait_for(std::chrono::seconds(0)) ==
        std::future_status::ready)
    holder.status = AssetStatus::Loaded;

  if (holder.status == AssetStatus::Loading) return getDummyAsset(holder.type);

  return holder.asset.get();
}

void removeAsset(ID id) {
  std::lock_guard<std::mutex> lock(assetMutex);

  if (id >= MaxAssets || !assetSlots[id])
    throw std::out_of_range("Invalid asset ID");

  AssetHolder& holder = assets[id];

  if (holder.loadFuture.valid()) {
    if (holder.status == AssetStatus::Loading)
      throw std::runtime_error("Cannot remove an asset that is still loading");

    holder.loadFuture
      .wait();  // Wait for the loading to complete if not already done.
  }

  assetSlots[id] = false;  // free the slot
}

}  // namespace Magnet::Library