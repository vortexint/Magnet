#include <spdlog/spdlog.h>

#define QOI_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include <magnet/Library.hpp>
#include <magnet/Rendering.hpp>

#include "qoi.h"

namespace Magnet::Library {

// TODO: Move to rendering pipeline...
void genTexture(unsigned int& id, int width, int height, uint8_t* data) {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
}

/* Loaders */

void Texture::load(Mimetype mimetype, const uint8_t* inputData, size_t size) {
  if (mimetype == Mimetype::PNG) {
    data =
      stbi_load_from_memory(inputData, size, &width, &height, &channels, 0);
  } else if (mimetype == Mimetype::QOI) {
    qoi_desc desc;
    data = static_cast<unsigned char*>(qoi_decode(&inputData, size, &desc, 4));
  }
  if (data == nullptr) {
    spdlog::error("Failed to load Texture!");
    return;
  }
}

void Model::load(Mimetype mimetype, const uint8_t* inputData, size_t size) {
  tinygltf::TinyGLTF loader;
  std::string err, warn;

  bool ret = false;

  if (mimetype == Mimetype::GLB) {
    ret = loader.LoadBinaryFromMemory(&model, &err, &warn, inputData, size);
  } else if (mimetype == Mimetype::GLTF) {
    // TODO: this
  }

  if (!ret) {
    spdlog::error("Failed to load Model: {}", err);
    return;
  }

  if (!warn.empty()) {
    spdlog::warn("Model warning: {}", warn);
  }
}

/* Destructors  */

Texture::~Texture() {
  free(data);
  // glDeleteTextures(1, &id);
}

Model::~Model() {
  // glDeleteVertexArrays(1, &vao);
  // glDeleteBuffers(1, &vbo);
  // glDeleteBuffers(1, &ebo);
}

}  // namespace Magnet::Library