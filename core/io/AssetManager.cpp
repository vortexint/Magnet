#include "magnetar/AssetManager.hpp"

#include <spdlog/spdlog.h>

#include <iostream>
#include <stdexcept>

AssetManager::AssetManager() : zipArchive(nullptr) {}

AssetManager::~AssetManager() { closeArchive(); }

bool AssetManager::openArchive(const std::string& archivePath) {
  int error;
  zipArchive = zip_open(archivePath.c_str(), ZIP_RDONLY, &error);

  if (!zipArchive) {
    // Handle error or convert it to a C++ exception:
    zip_error_t zipError;
    zip_error_init_with_code(&zipError, error);
    std::string errorMessage = zip_error_strerror(&zipError);
    zip_error_fini(&zipError);
    throw std::runtime_error(errorMessage);
  }

  return true;
}

void AssetManager::closeArchive() {
  if (zipArchive) {
    zip_close(zipArchive);
    zipArchive = nullptr;
  }
}

std::vector<unsigned char> AssetManager::getAsset(
  const std::string& assetName) {
  // Check if the asset is already in the cache
  auto it = cache.find(assetName);
  if (it != cache.end()) {
    return it->second;
  }

  // Locate the file inside the ZIP archive
  zip_int64_t index = zip_name_locate(zipArchive, assetName.c_str(), 0);
  if (index == -1) {
    throw std::runtime_error("File not found in the archive");
  }

  // Open the file inside the ZIP archive
  zip_file_t* file = zip_fopen_index(zipArchive, index, ZIP_FL_ENC_CP437);
  if (!file) {
    throw std::runtime_error("Unable to open file within the archive");
  }

  // Get file information
  zip_stat_t file_stat;
  if (zip_stat_index(zipArchive, index, ZIP_FL_ENC_CP437, &file_stat) != 0) {
    zip_fclose(file);
    throw std::runtime_error(
      "Unable to retrieve file information from the archive");
  }

  // Read file contents
  std::vector<unsigned char> buffer(file_stat.size);
  zip_int64_t bytesRead = zip_fread(file, buffer.data(), file_stat.size);
  if (bytesRead < 0) {
    zip_fclose(file);
    throw std::runtime_error("Error reading file from the archive");
  }
  zip_fclose(file);

  // If the file was successfully read, add it to the cache
  if (bytesRead == file_stat.size) {
    cache[assetName] = buffer;
  }

  return buffer;
}