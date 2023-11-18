#include "magnetar/AssetManager.hpp"

#include <spdlog/spdlog.h>

AssetManager::AssetManager() = default;

AssetManager::~AssetManager() { closeArchive(); }

void AssetManager::openArchive(std::string archivePath) {
  std::scoped_lock lock(mutex_);
  int error{};
  archive_ = zip_open(archivePath.c_str(), 0, &error);
  if (!archive_) {
    spdlog::error("Could not open archive: {}", archivePath);
    throw std::runtime_error("Could not open archive.");
  }
}

void AssetManager::closeArchive() {
  std::scoped_lock lock(mutex_);
  if (archive_) {
    zip_close(archive_);
    archive_ = nullptr;
  }
}

std::shared_ptr<std::vector<unsigned char>> AssetManager::readAsset(
  std::string_view assetName) {
  std::string assetNameStr{assetName};  // Convert to string for map lookup
  auto cached = cache_.find(assetNameStr);
  if (cached != cache_.end()) {
    return cached->second;
  }

  zip_int64_t index = zip_name_locate(archive_, assetNameStr.c_str(), 0);
  if (index == -1) {
    spdlog::warn("Asset '{}' not found in archive.", assetNameStr);
    return nullptr;
  }

  zip_file_t* file = zip_fopen_index(archive_, index, 0);
  if (!file) {
    spdlog::error("Failed to open asset '{}' in archive.", assetNameStr);
    return nullptr;
  }

  zip_stat_t stat;
  if (zip_stat_index(archive_, index, 0, &stat) != 0) {
    spdlog::error("Failed to stat asset '{}' in archive.", assetNameStr);
    zip_fclose(file);
    return nullptr;
  }

  auto buffer = std::make_shared<std::vector<unsigned char>>(stat.size);
  if (zip_fread(file, buffer->data(), stat.size) != stat.size) {
    spdlog::error("Failed to read asset '{}' from archive.", assetNameStr);
    zip_fclose(file);
    return nullptr;
  }

  zip_fclose(file);
  cache_[assetNameStr] = buffer;  // Cache the data.
  return buffer;
}

void AssetManager::getAsset(std::string_view assetName,
                            std::vector<unsigned char>& buffer) {
  std::shared_ptr<std::vector<unsigned char>> data = readAsset(assetName);
  if (data) {
    buffer = *data;
  }
}

void AssetManager::getAsset(std::string_view assetName, std::string& buffer) {
  std::shared_ptr<std::vector<unsigned char>> data = readAsset(assetName);
  if (data) {
    buffer.assign(data->begin(), data->end());
  }
}