#include <spdlog/spdlog.h>

#include <magnet/Library.hpp>

#define QOI_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "qoi.h"
#include "tiny_gltf.h"  // includes stb_image.h, won't question it! :D

namespace Magnet::Library::Loader {

// TODO: Move to rendering pipeline...?
void setTexOptions() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

ID generateAsset(Mimetype mimetype, const std::vector<uint8_t>& data) {
  Library::Asset asset;

  switch (mimetype) {
    case Mimetype::EXR:
      // TODO
      break;
    case Mimetype::GLTF:
      // TODO
      break;
    case Mimetype::GLB:
      // TODO
      break;
    case Mimetype::PNG: {
      Texture texture;

      unsigned char* img =
        stbi_load_from_memory(&data[0], data.size(), &texture.width,
                              &texture.height, &texture.channels, 0);
      if (img == nullptr) {
        spdlog::error("Failed to load PNG!");
        return 0;
      }

      glGenTextures(1, &texture.id);
      glBindTexture(GL_TEXTURE_2D, texture.id);
      setTexOptions();
      glGenerateMipmap(GL_TEXTURE_2D);

      free(img);

      asset.type = AssetType::Texture;
      asset.data = std::move(texture);
      ID id = addAsset(std::move(asset));
      return id;
    }
    case Mimetype::QOI: {
      Texture texture;
      qoi_desc desc;
      void* img = qoi_decode(data.data(), data.size(), &desc, 4);
      
      if (img == nullptr) {
        spdlog::error("Failed to load QOI!");
        return 0;
      }
      
      glGenTextures(1, &texture.id);
      glBindTexture(GL_TEXTURE_2D, texture.id);
      setTexOptions();
      glGenerateMipmap(GL_TEXTURE_2D);

      free(img);

      asset.type = AssetType::Texture;
      asset.data = std::move(texture);
      ID id = addAsset(std::move(asset));
      return id;
    }
  }
  // No asset was generated
  spdlog::error("mimetype not supported!");
  return 0;
}

ID generateAsset(Mimetype mimetype, ArchiveManager& archiveMgr,
                 const std::string& filename) {
  std::vector<uint8_t> data;
  archiveMgr.loadFile(filename, data);
  return generateAsset(mimetype, data);
}

}  // namespace Magnet::Library::Loader