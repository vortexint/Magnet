#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#include <string>
#include <unordered_set>

namespace Magnet::Shaders {

// Generate a shader program from the given vertex and fragment sources
// And returns it's ID.
GLuint generate(const std::string& vertexSource,
                 const std::string& fragmentSource);

// Delete all shader programs
void cleanup();

/* Uniform Buffer Operations */

GLuint createUniformBuffer(GLsizeiptr size);
void updateUniformBuffer(GLuint ubo, GLintptr offset, GLsizeiptr size,
                         const void* data);
void bindUniformBuffer(GLuint ubo, GLuint bindingPoint);

/* Setters - These are SLOW and should only be used for debugging.
   Prefer uniform buffers, very superior in terms of performance. */

void setBool(GLuint shaderID, const char* name, bool value);
void setInt(GLuint shaderID, const char* name, int value);
void setFloat(GLuint shaderID, const char* name, float value);
void setMat4(GLuint shaderID, const char* name, const mat4 matrix);

}  // namespace Magnet::Shaders