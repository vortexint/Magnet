#include <spdlog/spdlog.h>

#include <magnet/Shaders.hpp>

namespace Magnet::Shaders {

// Utility function for checking shader compilation/linking errors.
void checkCompileErrors(unsigned int shader, const char* type) {
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

std::unordered_set<GLuint>& getShaders() {
  static std::unordered_set<GLuint> shaders;
  return shaders;
}

GLuint generate(const std::string& vertexSource,
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

  // Add to shaders unordered_set
  getShaders().emplace(shaderProgram);

  return shaderProgram;
}

void cleanup() {
  size_t count = 0;
  for (GLuint shader : getShaders()) {
    glDeleteProgram(shader);
    count++;
  }
  spdlog::info("{} Shader program(s) deleted", count);
}

/* Setters */

GLuint createUniformBuffer(GLsizeiptr size) {
  GLuint ubo;
  glGenBuffers(1, &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
  return ubo;
}

void updateUniformBuffer(GLuint ubo, GLintptr offset, GLsizeiptr size,
                         const void* data) {
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void bindUniformBuffer(GLuint ubo, GLuint bindingPoint) {
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
}

void setBool(GLuint shaderID, const char* name, bool value) {
  glUniform1i(glGetUniformLocation(shaderID, name), (int)value);
}

void setInt(GLuint shaderID, const char* name, int value) {
  glUniform1i(glGetUniformLocation(shaderID, name), value);
}

void setFloat(GLuint shaderID, const char* name, float value) {
  glUniform1f(glGetUniformLocation(shaderID, name), value);
}

void setMat4(GLuint shaderID, const char* name, const mat4 matrix) {
  glUniformMatrix4fv(glGetUniformLocation(shaderID, name), 1, GL_FALSE,
                     (const GLfloat*)matrix);
}
void setVec4(GLuint shaderID, const char* name, const vec4 value) {
  glUniform4f(glGetUniformLocation(shaderID, name), value[0], value[1],
              value[2], value[3]);
}
void setVec3(GLuint shaderID, const char* name, const vec3 value) {
  glUniform3f(glGetUniformLocation(shaderID, name), value[0], value[1],
              value[2]);
}

}  // namespace Magnet::Shaders