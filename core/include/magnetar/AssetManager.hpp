#pragma once

#include <memory>
#include <unordered_map>
#include <cstring>

// forward declarations
struct archive;
struct archive_entry;

class AssetManager {
private:
  const char *SECURE_ARCHIVE;
  const size_t BLOCK_SIZE = 10240;

  struct archive *a = nullptr;
  struct archive_entry *entry = nullptr;
  std::unordered_map<std::string, std::unique_ptr<unsigned char[]>> assetCache;

  bool archiveOpened = false;

public:
  AssetManager(const char *secure_archive);
  ~AssetManager();

  // Return -1 if could not open
  int openArchive();
  void closeArchive();

  // If archive not opened yet, open it
  unsigned char *getAsset(std::string assetPath, size_t &dataSize);
};