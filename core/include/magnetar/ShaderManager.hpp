#pragma once

#include <cglm/cglm.h>

#include "magnetar/AssetManager.hpp"

class ShaderManager {
  AssetManager *assetManager;
  std::unordered_map<std::string, unsigned int> shaders;

  // Utility function for checking shader compilation/linking errors.
  void checkCompileErrors(unsigned int shader, std::string type);

 public:
  ShaderManager(AssetManager *assetManager);

  unsigned int genShader(const std::string &shaderName,
                         const std::string &vertexPath,
                         const std::string &fragmentPath);

  unsigned int getShader(const std::string &shaderName) const;

  void setBool(const std::string &shaderName, const std::string &name,
               bool value) const;
  void setInt(const std::string &shaderName, const std::string &name,
              int value) const;
  void setFloat(const std::string &shaderName, const std::string &name,
                float value) const;
  void setMat4(const std::string &shaderName, const std::string &name,
               const mat4 matrix) const;

  void deleteShader(const std::string &shaderName);
};