#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#include <string>
#include <unordered_map>

namespace Magnet {

namespace ShaderManager {
  GLuint genShader(const std::string& vertexSource,
                          const std::string& fragmentSource);
  void deleteShader(GLuint shaderID);

  /* Setters */

  GLuint createUniformBuffer(GLsizeiptr size);
  void updateUniformBuffer(GLuint ubo, GLintptr offset, GLsizeiptr size,
                                  const void* data);
  void bindUniformBuffer(GLuint ubo, GLuint bindingPoint);

  void setBool(GLuint shaderID, const char* name, bool value);
  void setInt(GLuint shaderID, const char* name, int value);
  void setFloat(GLuint shaderID, const char* name, float value);
  void setMat4(GLuint shaderID, const char* name, const mat4 matrix);
}

}  // namespace Magnet