#include "shader.hpp"

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
  int success;
  char infoLog[1024];

  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) {
      return;  // Shader compilation successful, no need to proceed.
    }

    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
              << infoLog
              << "\n --- "
              << std::endl;
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success) {
      return;  // Program linking successful, no need to proceed.
    }

    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
              << infoLog
              << "\n --- "
              << std::endl;
  }
}

Shader::Shader(AssetManager &assetManager_, const std::string &vertexPath,
               const std::string &fragmentPath)
    : assetManager(assetManager_) {

  // 1. Retrieve the vertex/fragment source code from filePath via AssetManager
  size_t dataSize;
  const char *vShaderCode = reinterpret_cast<const char *>(
      assetManager.getAsset(vertexPath, dataSize));
  const char *fShaderCode = reinterpret_cast<const char *>(
      assetManager.getAsset(fragmentPath, dataSize));

  if (!vShaderCode || !fShaderCode) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    return;
  }

  // 2. Compile shaders
  unsigned int vertex, fragment;

  // Vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, nullptr);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  // Fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, nullptr);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  // Shader Program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM");

  // Delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() const { glUseProgram(ID); }

void Shader::setBool(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}