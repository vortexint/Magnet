#pragma once

#include "stdafx.hpp"

class AssetManager {
  struct archive *a;
  struct archive_entry *entry;
  int r;

  // Caching
  std::unordered_map<std::string, std::unique_ptr<unsigned char[]>> assetCache;

public:
  AssetManager() = default;
  ~AssetManager() = default;

  void openArchive();
  void closeArchive();

  /* Assets */

  unsigned char* getAsset(std::string assetPath, size_t &dataSize);
};
