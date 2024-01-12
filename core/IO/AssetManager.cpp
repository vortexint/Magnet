#include "magnet/AssetManager.hpp"

#define LIBARCHIVE_STATIC
#include <archive.h>
#include <archive_entry.h>

#include <spdlog/spdlog.h>

const size_t BLOCK_SIZE = 10240;

// An instance of AssetManager is dedicated to a single archive.
AssetManager::AssetManager(const char* archivePath,
                           const char* key) : aPath(archivePath), a(nullptr)
{
  if (key) {
    // TODO: implement decryption...?
  }
}

AssetManager::~AssetManager() {
  
}

// Returns the asset's data in a generic type that can be converted.
std::optional<std::vector<std::uint8_t>> AssetManager::readAsset(
  std::string_view assetName) {
  std::scoped_lock lock(mutex); // declared in AssetManager.hpp

  struct archive* a = archive_read_new();
  archive_read_support_format_tar(a);

  if (archive_read_open_filename(a, aPath, BLOCK_SIZE) != ARCHIVE_OK) {
    spdlog::error("Failed to open {}: {}", aPath, archive_error_string(a));
    archive_read_free(a);
    throw std::runtime_error("Failed to open archive");
  }

  std::unique_ptr<struct archive, decltype(&archive_read_free)> archivePtr(
    a, archive_read_free);

  archive_entry* entry;
  std::vector<std::uint8_t> buffer;
  bool found = false;

  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    spdlog::debug("Reading {}", archive_entry_pathname(entry));
    if (archive_entry_pathname(entry) == assetName) {
      size_t size = archive_entry_size(entry);
      buffer.resize(size);
      if (static_cast<la_ssize_t>(archive_read_data(a, buffer.data(), size)) != static_cast<la_ssize_t>(size)) {
        spdlog::error("Failed to read {}: {}", assetName,
                      archive_error_string(a));
        throw std::runtime_error("Failed to read asset");
      }
      found = true;
      break;
    }
  }

  if (!found) {
    spdlog::error("Asset {} not found", assetName);
    throw std::runtime_error("Asset not found");
  }

  return buffer;
}

void AssetManager::getAsset(std::string_view assetName,
                            std::vector<uint8_t>& buffer) {
  auto optBuffer = readAsset(assetName);
  if (!optBuffer) {
    throw std::runtime_error("Failed to read asset");
  }
  buffer = std::move(*optBuffer);
}

void AssetManager::getAsset(std::string_view assetName, std::string& buffer) {
  auto optBuffer = readAsset(assetName);
  if (!optBuffer) {
    throw std::runtime_error("Failed to read asset");
  }
  buffer.assign(optBuffer->begin(), optBuffer->end());
}