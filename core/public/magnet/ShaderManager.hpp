#pragma once

#include <cglm/cglm.h>
#include <unordered_map>

#include "magnet/AssetManager.hpp"

namespace Magnet {

class ShaderManager {
  AssetManager *assetManager;
  std::unordered_map<const char*, unsigned int> shaders;

  // Utility function for checking shader compilation/linking errors.
  void checkCompileErrors(unsigned int shader, const char* type);

 public:
  ShaderManager(AssetManager *assetManager);

  unsigned int genShader(const char* shaderName,
                         const char* vertexPath,
                         const char* fragmentPath);

  /* Getters */
  unsigned int getShader(const char* shaderName) const;

  /* Setters */
  void setBool(const char* shaderName, const char* name,
               bool value) const;
  void setInt(const char* shaderName, const char* name,
              int value) const;
  void setFloat(const char* shaderName, const char* name,
                float value) const;
  void setMat4(const char* shaderName, const char* name,
               const mat4 matrix) const;

  void deleteShader(const char* shaderName);
};

}  // namespace Magnet