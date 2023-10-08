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

unsigned char *AssetManager::LoadAsset(const std::string &assetPath,
                                       size_t &dataSize) {
  auto it = assetCache.find(assetPath);

  // If asset is already in cache
  if (it != assetCache.end()) {
    dataSize = sizeof(it->second.get());
    return it->second.get();
  }

  // Else, asset should be read from the archive
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char *currentFile = archive_entry_pathname(entry);
    if (currentFile == assetPath) {
      la_int64_t size = archive_entry_size(entry); // Get file size in bytes
      auto buffer = std::make_unique<unsigned char[]>(size);
      archive_read_data(a, buffer.get(), size); // Read the data into buffer

      // Insert in cache before returning
      assetCache.insert({assetPath, std::move(buffer)});
      dataSize = size;
      return assetCache[assetPath].get();
    }
  }

  // Fires if Asset Doesn't exist in the archive
  dataSize = 0;
  return nullptr;
}