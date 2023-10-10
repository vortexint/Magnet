#include "asset_manager.hpp"

AssetManager::AssetManager(const char *secure_archive)
    : SECURE_ARCHIVE(secure_archive) {
  int openResult = openArchive();
  if (openResult == -1) {
    // Log error message
    std::cout << "Error opening archive: " << archive_error_string(a)
              << std::endl;
    return;
  }
}

AssetManager::~AssetManager() { closeArchive(); }

int AssetManager::openArchive() {
  if (!archiveOpened) {
    a = archive_read_new();
    archive_read_support_filter_lz4(a);
    archive_read_support_format_tar(a);

    int r = archive_read_open_filename(a, SECURE_ARCHIVE, BLOCK_SIZE);
    if (r != ARCHIVE_OK) {
      // Return error code
      return -1;
    }

    archiveOpened = true;
  }

  return 0;
}

void AssetManager::closeArchive() {
  if (archiveOpened) {
    archive_read_free(a);
    a = nullptr;
    archiveOpened = false;
  }
}

unsigned char *AssetManager::getAsset(std::string assetPath, size_t &dataSize) {
  // Attempt to open the archive if it's closed
  if (!archiveOpened) {
    int openResult = openArchive();
    if (openResult == -1)
      return nullptr;
  }

  assetPath = "./" + assetPath; // prepend "./" to the asset path.

  /* Check if asset is cached */

  auto it = assetCache.find(assetPath);

  if (it != assetCache.end()) {
    dataSize = sizeof(it->second.get());
    return it->second.get();
  }

  /* Otherwise read from the archive */

  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char *currentFile = archive_entry_pathname(entry);
    if (strcmp(currentFile, assetPath.c_str()) == 0) {
      // Get file size in bytes and assign directly to dataSize
      dataSize = archive_entry_size(entry);

      auto buffer = std::make_unique<unsigned char[]>(dataSize);

      // Read the data into buffer
      archive_read_data(a, buffer.get(), dataSize);

      // Insert in cache before returning
      assetCache.insert({assetPath, std::move(buffer)});

      return assetCache[assetPath].get();
    }
  }

  /* Asset doesn't exist in the archive */

  dataSize = 0;
  return nullptr;
}