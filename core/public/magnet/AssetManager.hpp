#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct archive;
struct archive_entry;

namespace Magnet {

class AssetManager {
  archive* a = nullptr;
  const char* aPath;

  std::unique_ptr<std::vector<uint8_t>> readAsset(const std::string& assetName);

 public:
  AssetManager(const char* archivePath, const char* key = nullptr);
  ~AssetManager();

  void getAsset(const std::string&, std::vector<uint8_t>& buffer);
  void getAsset(const std::string&, std::string& buffer);
};

} // namespace Magnet