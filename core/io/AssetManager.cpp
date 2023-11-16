#include "magnetar/AssetManager.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <spdlog/spdlog.h>

const size_t BLOCK_SIZE = 10240;

unsigned char* AssetManager::getAsset(const char* secure_archive,
                                      const char* assetPath, size_t* dataSize) {
  std::scoped_lock lock(mutex_);  // RAII-based mutex locking

  struct archive* a = archive_read_new();
  archive_read_support_filter_lz4(a);
  archive_read_support_format_tar(a);

  // Open the archive file
  if (archive_read_open_filename(a, secure_archive, BLOCK_SIZE) != ARCHIVE_OK) {
    spdlog::error("Could not open archive {}: {}", secure_archive,
                  archive_error_string(a));
    archive_read_free(a);
    throw std::runtime_error(archive_error_string(a));
  }

  struct archive_entry* entry;
  size_t size;
  unsigned char* buffer = nullptr;

  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char* currentFile = archive_entry_pathname(entry);
    if (strcmp(currentFile, assetPath) == 0) {
      size = archive_entry_size(entry);
      buffer = new unsigned char[size];
      if (archive_read_data(a, buffer, size) != static_cast<ssize_t>(size)) {
        delete[] buffer;
        spdlog::error("Failed to read asset data from {}: {}",
                      secure_archive, archive_error_string(a));
        archive_read_free(a);
      }
      if (dataSize) {
        *dataSize = size;
      }
      break;
    }
  }

  archive_read_free(a);

  if (!buffer) {
    spdlog::warn("'{}' not found in archive '{}'.", assetPath,
                 secure_archive);
  }

  spdlog::info("'{}' successfully loaded from '{}'.", assetPath,
               secure_archive);
  return buffer;
}