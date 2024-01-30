#include <magnet/ArchiveManager.hpp>

#define LIBARCHIVE_STATIC
#include <archive.h>
#include <archive_entry.h>
#include <spdlog/spdlog.h>

namespace Magnet {

const size_t BLOCK_SIZE = 10240;

// An instance of ArchiveManager is dedicated to a single archive.
ArchiveManager::ArchiveManager(const char* archivePath, const char* key)
  : aPath(archivePath) {
  if (key) {
    // TODO: implement decryption...?
  }
}

ArchiveManager::~ArchiveManager() {
  if (a) {
    archive_read_free(a);
  }
}

// Returns the asset's data in a generic type that can be converted.
std::unique_ptr<std::vector<uint8_t>> ArchiveManager::readFile(
  const std::string& assetName) {
  std::string assetNameCopy =
    "./" + assetName;  // leading ./ is required for archive_entry_pathname

  struct archive* a = archive_read_new();
  archive_read_support_format_tar(a);

  if (archive_read_open_filename(a, aPath, BLOCK_SIZE)) {
    spdlog::critical("Failed to open {}: {}", aPath, archive_error_string(a));
    archive_read_free(a);
    return nullptr;
  }

  std::unique_ptr<struct archive, decltype(&archive_read_free)> archivePtr(
    a, archive_read_free);

  archive_entry* entry;
  auto buffer = std::make_unique<std::vector<uint8_t>>();
  bool found = false;

  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    spdlog::debug("Reading {}", archive_entry_pathname(entry));
    if (archive_entry_pathname(entry) == assetNameCopy) {
      size_t size = archive_entry_size(entry);
      buffer->resize(size);
      if (static_cast<la_ssize_t>(archive_read_data(a, buffer->data(), size)) !=
          static_cast<la_ssize_t>(size)) {
        spdlog::error("Failed to read {}: {}", assetNameCopy,
                      archive_error_string(a));
        return nullptr;
      }
      found = true;
      break;
    }
  }

  if (!found) {
    spdlog::error("Asset {} not found", assetNameCopy);
  }

  return buffer;
}

void ArchiveManager::loadFile(const std::string& assetName,
                            std::vector<uint8_t>& buffer) {
  auto optBuffer = readFile(assetName);
  if (!optBuffer) {
    spdlog::error("Failed to read asset");
    return;
  }
  buffer = std::move(*optBuffer);
}

void ArchiveManager::loadFile(const std::string& assetName, std::string& buffer) {
  auto optBuffer = readFile(assetName);
  if (!optBuffer) {
    spdlog::error("Failed to read asset");
    return;
  }
  buffer.assign(optBuffer->begin(), optBuffer->end());
}

}  // namespace Magnet