#pragma once

#include <zip.h>

#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class AssetManager {
 public:
  AssetManager(std::string archivePath, const std::string& password);
  ~AssetManager();

  void getAsset(std::string_view assetName, std::vector<unsigned char>& buffer);
  void getAsset(std::string_view assetName, std::string& buffer);

 private:
  std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<std::vector<unsigned char>>>
    cache_;
  zip_t* archive_{nullptr};

  std::shared_ptr<std::vector<unsigned char>> readAsset(
    std::string_view assetName);
};