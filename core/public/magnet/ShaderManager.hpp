#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#include <string>
#include <unordered_map>

namespace Magnet {

class ShaderManager {
  ShaderManager() = delete;

  // Utility function for checking shader compilation/linking errors.
  static void checkCompileErrors(unsigned int shader, const char* type);

 public:
  static GLuint genShader(const std::string& vertexSource,
                          const std::string& fragmentSource);
  static void deleteShader(GLuint shaderID);

  /* Setters */

  static GLuint createUniformBuffer(GLsizeiptr size);
  static void updateUniformBuffer(GLuint ubo, GLintptr offset, GLsizeiptr size,
                                  const void* data);
  static void bindUniformBuffer(GLuint ubo, GLuint bindingPoint);

  static void setBool(GLuint shaderID, const char* name, bool value);
  static void setInt(GLuint shaderID, const char* name, int value);
  static void setFloat(GLuint shaderID, const char* name, float value);
  static void setMat4(GLuint shaderID, const char* name, const mat4 matrix);
};

}  // namespace Magnet