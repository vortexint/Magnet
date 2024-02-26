#include <magnet/Library.hpp>

#include <glad/glad.h>

namespace Magnet::Library {

/* Destructors  */

Texture::~Texture() { glDeleteTextures(1, &id); }
Mesh::~Mesh() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

}  // namespace Magnet::Library