#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

// forward declarations
  struct archive;
  struct archive_entry;

class AssetManager {
 public:
  struct CachedAsset {
    std::unique_ptr<unsigned char[]> data;
    size_t dataSize;
  };

 private:
  const char *SECURE_ARCHIVE;
  const size_t BLOCK_SIZE = 10240;

  struct archive *a = nullptr;
  struct archive_entry *entry = nullptr;
  std::unordered_map<std::string, CachedAsset> assetCache;
  std::mutex assetAccessMutex;

  bool archiveOpened = false;

 public:
  AssetManager(const char *secure_archive);
  ~AssetManager();

  // Returns -1 if could not open
  int openArchive();

  void closeArchive();

  unsigned char *getAsset(std::string assetPath, size_t *dataSize = nullptr);
};