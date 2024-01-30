#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct archive;
struct archive_entry;

namespace Magnet {

class ArchiveManager {
  archive* a = nullptr;
  const char* aPath;

  std::unique_ptr<std::vector<uint8_t>> readFile(const std::string& assetName);

 public:
  ArchiveManager(const char* archivePath, const char* key = nullptr);
  ~ArchiveManager();

  void loadFile(const std::string&, std::vector<uint8_t>& buffer);
  void loadFile(const std::string&, std::string& buffer);
};

}  // namespace Magnet