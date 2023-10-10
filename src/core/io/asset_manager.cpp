#include "asset_manager.hpp"

AssetManager::AssetManager(const char *secure_archive) : SECURE_ARCHIVE(secure_archive) {}

AssetManager::~AssetManager() {
  if (a)
    archive_read_free(a);
}

void AssetManager::openArchive() {
  a = archive_read_new();
  archive_read_support_filter_lz4(a);
  archive_read_support_format_tar(a);

  r = archive_read_open_filename(a, SECURE_ARCHIVE, 10240);

  if (r != ARCHIVE_OK) {
    // error opening archive, log archive_error_string(a)
    std::terminate();
  }
}

void AssetManager::closeArchive() { archive_read_free(a); }

unsigned char *AssetManager::getAsset(std::string assetPath, size_t &dataSize) {
  assetPath = "./" + assetPath; // prepend "./" to the asset path due to how tar
                                // structures file paths in archive entries

  auto it = assetCache.find(assetPath);

  /* Check if asset is cached */

  if (it != assetCache.end()) {
    dataSize = sizeof(it->second.get());
    return it->second.get();
  }

  /* Otherwise read from the archive */
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char *currentFile = archive_entry_pathname(entry);
    if (strcmp(currentFile, assetPath.c_str()) == 0) {
      la_int64_t size = archive_entry_size(entry); // Get file size in bytes
      auto buffer = std::make_unique<unsigned char[]>(size);
      archive_read_data(a, buffer.get(), size); // Read the data into buffer

      // Insert in cache before returning
      assetCache.insert({assetPath, std::move(buffer)});
      dataSize = size;
      return assetCache[assetPath].get();
    }
  }

  /* Asset doesn't exist in the archive */

  dataSize = 0;
  return nullptr;
}