#pragma once

#include <mutex>

// forward declarations
struct archive;
struct archive_entry;

class AssetManager {

  std::mutex mutex_;
 public:

  unsigned char* getAsset(const char* secure_archive, const char* assetPath,
                          size_t* dataSize = nullptr);
};