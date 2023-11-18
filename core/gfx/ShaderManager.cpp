#include "magnetar/ShaderManager.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <cstring>

ShaderManager::ShaderManager(AssetManager* assetMgr) : assetManager(assetMgr) {}

unsigned int ShaderManager::genShader(const char* shaderName,
                                      const char* vertexPath,
                                      const char* fragmentPath) {
  size_t vShaderSize, fShaderSize;

  assetManager->openArchive(SECURE_ASSETS_ARCHIVE);

  // Retrieve the vertex shader source

  unsigned char* vShaderData =
    assetManager->getAsset(vertexPath, &vShaderSize);
  if (!vShaderData) {
    throw std::runtime_error("Vertex shader asset could not be loaded.");
  }
  // Safely convert the data to a const GLchar* for OpenGL
  const GLchar* vShader = reinterpret_cast<const GLchar*>(vShaderData);

  // Retrieve the fragment shader source
  unsigned char* fShaderData =
    assetManager->getAsset(fragmentPath, &fShaderSize);
  if (!fShaderData) {
    delete[] vShaderData;
    throw std::runtime_error("Fragment shader asset could not be loaded.");
  }
  // Safely convert the data to a const GLchar* for OpenGL
  const GLchar* fShader = reinterpret_cast<const GLchar*>(fShaderData);

  assetManager->closeArchive();

  unsigned int vertex, fragment;

  // Create and compile the vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShader, nullptr);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  // Create and compile the fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShader, nullptr);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  // Link shaders into a program
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertex);
  glAttachShader(shaderProgram, fragment);
  glLinkProgram(shaderProgram);
  checkCompileErrors(shaderProgram, "PROGRAM");

  // Clean up shaders now that they are linked
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  // Free the loaded shader data
  delete[] vShaderData;
  delete[] fShaderData;

  // Store and return the shader program ID
  shaders.insert({shaderName, shaderProgram});

  return shaderProgram;
}

unsigned int ShaderManager::getShader(const char* shaderName) const {
  return shaders.at(shaderName);
}

void ShaderManager::setBool(const char* shaderName, const char* name,
                            bool value) const {
  glUniform1i(glGetUniformLocation(shaders.at(shaderName), name),
              static_cast<int>(value));
}

void ShaderManager::setInt(const char* shaderName, const char* name,
                           int value) const {
  glUniform1i(glGetUniformLocation(shaders.at(shaderName), name), value);
}

void ShaderManager::setFloat(const char* shaderName, const char* name,
                             float value) const {
  glUniform1f(glGetUniformLocation(shaders.at(shaderName), name), value);
}

void ShaderManager::setMat4(const char* shaderName, const char* name,
                            const mat4 matrix) const {
  unsigned int shader = getShader(shaderName);
  glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE,
                     (const GLfloat*)matrix);
}

void ShaderManager::deleteShader(const char* shaderName) {
  glDeleteProgram(shaders.at(shaderName));
  shaders.erase(shaderName);
}

void ShaderManager::checkCompileErrors(unsigned int shader, const char* type) {
  int success;
  char infoLog[1024];

  if (strcmp(type, "PROGRAM") != 0) {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success) return;  // Compilation successful, no need to proceed.

    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    spdlog::error("{0} SHADER: {1}", type, infoLog);
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success) return;  // Linking successful, no need to proceed.

    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
    spdlog::error("SHADER PROGRAM: {0}", infoLog);
  }
}