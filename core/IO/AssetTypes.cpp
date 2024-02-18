#include <magnet/Library.hpp>

namespace Magnet::Asset {

/* Deconstructors  */

Texture::~Texture() { glDeleteTextures(1, &id); }
Model::~Model() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

}  // namespace Magnet::Asset