#include "magnetar/AssetManager.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <spdlog/spdlog.h>

AssetManager::AssetManager(const char* secure_archive)
  : SECURE_ARCHIVE(secure_archive) {
  int openResult = openArchive();
  if (openResult == -1)
    spdlog::critical("Error opening archive: {}", archive_error_string(a));
  else
    spdlog::info("Archive opened and indexed successfully");
}

AssetManager::~AssetManager() { closeArchive(); }

int AssetManager::openArchive() {
  // Skip if archive is already open
  if (archiveOpened) {
    spdlog::warn("Archive is already open");
    return 0;
  }

  a = archive_read_new();
  archive_read_support_filter_lz4(a);
  archive_read_support_format_tar(a);

  int r = archive_read_open_filename(a, SECURE_ARCHIVE, BLOCK_SIZE);
  if (r != ARCHIVE_OK) {
    spdlog::error("Failed to open archive: {}", archive_error_string(a));
    return -1;
  }

  archiveOpened = true;
  return 0;
}

void AssetManager::closeArchive() {
  if (archiveOpened) {
    archive_read_free(a);
    a = nullptr;
    archiveOpened = false;
    spdlog::info("Archive closed");
  }
}

unsigned char* AssetManager::getAsset(std::string assetPath, size_t* dataSize) {
  std::lock_guard<std::mutex> lock(assetAccessMutex);
  size_t localDataSize;

  // User may not be interested in dataSize
  if (!dataSize) {
    dataSize = &localDataSize;
  }

  auto cacheResult = assetCache.find(assetPath);
  if (cacheResult != assetCache.end()) {
    *dataSize = cacheResult->second.dataSize;
    return cacheResult->second.data.get();
  }

  if (!archiveOpened) {
    int openResult = openArchive();
    if (openResult == -1) {
      spdlog::error("Cannot open archive");
      *dataSize = 0;
      return nullptr;
    }
  }

  *dataSize = 0;

  // Extract assetPath from archive
  // Loop for archive extraction
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char* currentFile = archive_entry_pathname(entry);

    if (assetPath == currentFile) {
      *dataSize = archive_entry_size(entry);
      auto buffer = std::make_unique<unsigned char[]>(*dataSize);
      archive_read_data(a, buffer.get(), *dataSize);
      assetCache[assetPath] = CachedAsset{std::move(buffer), *dataSize};
      break;
    }
  }

  if (*dataSize == 0) {
    spdlog::warn("{} was not found in archive", assetPath);
    return nullptr;
  }

  // Retrieve from cache again after inserting
  return assetCache[assetPath].data.get();
}