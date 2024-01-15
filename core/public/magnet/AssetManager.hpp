#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct archive;
struct archive_entry;

namespace Magnet {

class AssetManager {
  archive* a = nullptr;
  const char* aPath;

  std::optional<std::vector<uint8_t>> readAsset(std::string_view assetName);

 public:
  AssetManager(const char* archivePath, const char* key = nullptr);
  ~AssetManager();

  void getAsset(std::string_view assetName, std::vector<uint8_t>& buffer);
  void getAsset(std::string_view assetName, std::string& buffer);
};

} // namespace Magnet