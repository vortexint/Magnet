#pragma once

#include "app.hpp"

class AssetManager {
  struct archive *a;
  struct archive_entry *entry;
  int r;

  // Caching
  std::unordered_map<std::string, std::unique_ptr<unsigned char[]>> assetCache;

public:
  AssetManager();
  ~AssetManager();

  unsigned char* LoadAsset(const std::string &assetPath, size_t &dataSize);
};
