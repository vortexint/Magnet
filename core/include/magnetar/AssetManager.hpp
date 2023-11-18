#pragma once

#include <zip.h>

#include <string>
#include <unordered_map>
#include <vector>
class AssetManager {
  zip_t* zipArchive;
  std::unordered_map<std::string, std::vector<unsigned char>> cache;

 public:
  AssetManager();
  ~AssetManager();

  bool openArchive(const std::string& archivePath);
  void closeArchive();

  std::vector<unsigned char> getAsset(const std::string& assetName);
};