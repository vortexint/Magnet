#include "magnetar/ShaderManager.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

ShaderManager::ShaderManager(AssetManager* assetMgr)
  : assetManager(assetMgr) {}

unsigned int ShaderManager::genShader(const std::string &shaderName,
                                      const std::string &vertexPath,
                                      const std::string &fragmentPath) {
  size_t dataSize;
  assetManager->openArchive();
  unsigned char *vShaderData = assetManager->getAsset(vertexPath, dataSize);
  std::string vShaderCodeStr(reinterpret_cast<char *>(vShaderData), dataSize);
  unsigned char *fShaderData = assetManager->getAsset(fragmentPath, dataSize);
  std::string fShaderCodeStr(reinterpret_cast<char *>(fShaderData), dataSize);
  assetManager->closeArchive();

  const char *vShaderCode = vShaderCodeStr.c_str();
  const char *fShaderCode = fShaderCodeStr.c_str();

  unsigned int vertex, fragment;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, nullptr);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, nullptr);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertex);
  glAttachShader(shaderProgram, fragment);
  glLinkProgram(shaderProgram);
  checkCompileErrors(shaderProgram, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  shaders_.insert({shaderName, shaderProgram});

  return shaderProgram;
}

unsigned int ShaderManager::getShader(const std::string &shaderName) const {
  return shaders_.at(shaderName);
}

void ShaderManager::setBool(const std::string &shaderName,
                            const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(shaders_.at(shaderName), name.c_str()),
              static_cast<int>(value));
}

void ShaderManager::setInt(const std::string &shaderName,
                           const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(shaders_.at(shaderName), name.c_str()),
              value);
}

void ShaderManager::setFloat(const std::string &shaderName,
                             const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(shaders_.at(shaderName), name.c_str()),
              value);
}

void ShaderManager::setMat4(const std::string &shaderName,
                            const std::string &name, const mat4 matrix) const {
  unsigned int shader = getShader(shaderName);
  glUniformMatrix4fv(glGetUniformLocation(shader, name.c_str()), 1, GL_FALSE,
                     (const GLfloat *)matrix);
}

void ShaderManager::deleteShader(const std::string &shaderName) {
  glDeleteProgram(shaders_.at(shaderName));
  shaders_.erase(shaderName);
}

void ShaderManager::checkCompileErrors(unsigned int shader, std::string type) {
  int success;
  char infoLog[1024];

  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) {
      return;  // Shader compilation successful, no need to proceed.
    }

    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    spdlog::error("SHADER_COMPILATION_ERROR of type: {}", type);
    spdlog::error("{}", infoLog);
    spdlog::error("---");

  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success) {
      return;  // Program linking successful, no need to proceed.
    }

    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
    spdlog::error("PROGRAM_LINKING_ERROR of type: {}", type);
    spdlog::error("{}", infoLog);
    spdlog::error("---");
  }
}