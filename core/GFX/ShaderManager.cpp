#include <magnet/ShaderManager.hpp>
#include <magnet/ApplicationContext.hpp>

#include <spdlog/spdlog.h>

#include <cstring>

namespace Magnet {

void ShaderManager::checkCompileErrors(unsigned int shader, const char* type) {
  GLint success;
  char infoLog[1024];

  if (strcmp(type, "PROGRAM") != 0) {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) return;

    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    spdlog::error("{0} SHADER: {1}", type, infoLog);
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success) return;

    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
    spdlog::error("SHADER PROGRAM: {0}", infoLog);
  }
}

GLuint ShaderManager::genShader(const std::string& vertexSource,
                                const std::string& fragmentSource) {
  const GLchar* vertData[] = {vertexSource.c_str()};
  const GLchar* fragData[] = {fragmentSource.c_str()};

  GLuint vertex, fragment;

  // Create and compile the vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, vertData, nullptr);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  // Create and compile the fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, fragData, nullptr);
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

  return shaderProgram;
}
/* Setters */

void ShaderManager::setBool(GLuint* shaderID, const char* name, bool value) {
  glUniform1i(glGetUniformLocation(*shaderID, name), (int)value);
}

void ShaderManager::setInt(GLuint* shaderID, const char* name, int value) {
  glUniform1i(glGetUniformLocation(*shaderID, name), value);
}

void ShaderManager::setFloat(GLuint* shaderID, const char* name, float value) {
  glUniform1f(glGetUniformLocation(*shaderID, name), value);
}

void ShaderManager::setMat4(GLuint* shaderID, const char* name,
                            const mat4 matrix) {
  glUniformMatrix4fv(glGetUniformLocation(*shaderID, name), 1, GL_FALSE,
                     (const GLfloat*)matrix);
}

void ShaderManager::deleteShader(GLuint* shaderID) {
  glDeleteProgram(*shaderID);
}

}  // namespace Magnet