#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <vector>

struct archive;
struct archive_entry;

class AssetManager {
  std::mutex mutex;

  archive* a = nullptr;
  const char* aPath;

  std::optional<std::vector<std::uint8_t>> readAsset(std::string_view assetName);

 public:
  AssetManager(const char* archivePath, const char* key = nullptr);
  ~AssetManager();

  void getAsset(std::string_view assetName, std::vector<uint8_t>& buffer);
  void getAsset(std::string_view assetName, std::string& buffer);
};