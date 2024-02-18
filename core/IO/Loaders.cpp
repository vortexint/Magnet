#include <spdlog/spdlog.h>

#include <magnet/Library.hpp>

#define QOI_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "qoi.h"
#include "tiny_gltf.h"  // includes stb_image.h, won't question it! :D

namespace Magnet::Asset {

void setTexOptions() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

std::unique_ptr<IAsset> AssetLoader(Mimetype mimetype,
                                    const std::vector<uint8_t>& data) {
  std::unique_ptr<IAsset> asset;

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
    case Mimetype::PNG: { // Portable Network Graphics
      auto texture = std::make_unique<Texture>();
      stbi_set_flip_vertically_on_load(true);
      unsigned char* img =
        stbi_load_from_memory(data.data(), data.size(), &texture->width,
                              &texture->height, &texture->channels, 0);
      if (img == nullptr) {
        spdlog::error("Failed to load PNG data!");
        return nullptr;
      }
      glGenTextures(1, &(texture->id));
      glBindTexture(GL_TEXTURE_2D, texture->id);

      setTexOptions();

      if (texture->channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, img);
      else if (texture->channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, img);

      glGenerateMipmap(GL_TEXTURE_2D);

      free(img);
      asset = std::move(texture);

    } break;
    case Mimetype::QOI: { // Quite OK Image format
      auto texture = std::make_unique<Texture>();
      qoi_desc desc;
      void* img = qoi_decode(data.data(), data.size(), &desc, 4);
      if (img == nullptr) {
        spdlog::error("Failed to load QOI data!");
        return nullptr;
      }

      texture->width = desc.width;
      texture->height = desc.height;
      texture->channels = desc.channels;

      glGenTextures(1, &(texture->id));
      glBindTexture(GL_TEXTURE_2D, texture->id);

      setTexOptions();

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height,
                   0, GL_RGBA, GL_UNSIGNED_BYTE, img);

      glGenerateMipmap(GL_TEXTURE_2D);

      free(img);
      asset = std::move(texture);
    } break;
  }
  return asset;
}

}  // namespace Magnet::Asset