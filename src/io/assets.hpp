#pragma once

class AssetManager {
  struct archive *a;
  struct archive_entry *entry;
  int r;

  // Caching
  std::unordered_map<std::string, std::unique_ptr<unsigned char[]>> assetCache;

public:
  AssetManager();
  ~AssetManager();

  std::unique_ptr<unsigned char[]> LoadAsset(const std::string &assetPath,
                                             size_t &dataSize);
};

AssetManager &getAssetManager();