#include "../app.hpp"
#include "assets.hpp"

AssetManager::AssetManager() {
  a = archive_read_new();

  archive_read_support_filter_lz4(a);
  archive_read_support_format_tar(a);

  r = archive_read_open_filename(a, "data", 10240);

  if (r != ARCHIVE_OK) {
    // error opening archive, log archive_error_string(a)
    std::terminate();
  }
}

AssetManager::~AssetManager() { r = archive_read_free(a); }

std::unique_ptr<unsigned char[]>
AssetManager::LoadAsset(const std::string &assetPath, size_t &dataSize) {
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char *entryName = archive_entry_pathname(entry);

    if (entryName && std::string(entryName) == assetPath) {
      dataSize = archive_entry_size(entry);
      auto assetData = std::make_unique<unsigned char[]>(dataSize);

      archive_read_data(a, assetData.get(), dataSize);

      return assetData;
    }
  }

  dataSize = 0;

  // could not find the asset

  return nullptr;
}

AssetManager &getAssetManager() {
  static AssetManager assetManager;
  return assetManager;
}