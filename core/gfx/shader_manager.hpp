#pragma once

#include "io/asset_manager.hpp"

class ShaderManager {
  AssetManager &asset_manager_;
  std::unordered_map<std::string, unsigned int> shaders_;

  // Utility function for checking shader compilation/linking errors.
  void checkCompileErrors(unsigned int shader, std::string type);

public:
  ShaderManager(AssetManager &asset_manager);

  unsigned int genShader(const std::string &shaderName,
                         const std::string &vertexPath,
                         const std::string &fragmentPath);

  unsigned int getShader(const std::string &shaderName);

  // Modify the utility function signatures to include 'shaderName'.
  void setBool(const std::string &shaderName, const std::string &name,
               bool value) const;
  void setInt(const std::string &shaderName, const std::string &name,
              int value) const;
  void setFloat(const std::string &shaderName, const std::string &name,
                float value) const;

  // Added function to delete a shader.
  void deleteShader(const std::string &shaderName);
};