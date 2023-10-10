#pragma once

#include "stdafx.hpp"

class AssetManager {
  const char *SECURE_ARCHIVE;

  struct archive *a;
  struct archive_entry *entry;
  int r;

  // Caching
  std::unordered_map<std::string, std::unique_ptr<unsigned char[]>> assetCache;

public:
  AssetManager(const char* secure_archive);
  ~AssetManager();

  void openArchive();
  void closeArchive();

  /* Assets */

  unsigned char* getAsset(std::string assetPath, size_t &dataSize);
};
