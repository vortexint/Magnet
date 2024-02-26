#include "Window.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/Input.hpp>

#include "Viewport.hpp"

namespace Magnet::Window {

void errorCallback(int error, const char* desc) {
  spdlog::error("GLFW Error {}: {}", error, desc);
}
// Reference: https://www.khronos.org/opengl/wiki/Debug_Output
void GLAPIENTRY glErrorMessageCallback(GLenum source, GLenum type, GLuint id,
                                       GLenum severity, GLsizei length,
                                       const GLchar* message,
                                       const void* userParam);

void initialize(Context& context, const char* title) {
  GLFWwindow* window;
  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, title, nullptr, nullptr);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glErrorMessageCallback, nullptr);

  /* Setup callbacks */
  glfwSetFramebufferSizeCallback(window, Viewport::setSize);
  glfwSetErrorCallback(errorCallback);

  /* Initialize Input */
  Input::initialize(window);
  context.setWindow(window);
}

const char* sourceToStr(GLenum source);
const char* severityToStr(GLenum severity);
const char* typeToStr(GLenum type);

void GLAPIENTRY glErrorMessageCallback(GLenum source, GLenum type, GLuint id,
                                       GLenum severity, GLsizei length,
                                       const GLchar* message,
                                       const void* userParam) {
  /*fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);*/

  if (severity == GL_DEBUG_SEVERITY_LOW ||
      severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
    return;
  }

  const char* sourceStr = sourceToStr(source);
  const char* typeStr = typeToStr(type);
  const char* severityStr = severityToStr(severity);
  unsigned int idCopy = static_cast<unsigned int>(id);
  const char* messageCopy = static_cast<const char*>(message);
  spdlog::error("Type:{} Id:{} Severity:{} Message:{}", typeStr, idCopy,
                severityStr, messageCopy);
}
const char* sourceToStr(GLenum source) {
  const char* sourceStr = "Unkown Source";
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      sourceStr = "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      sourceStr = "Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      sourceStr = "Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      sourceStr = "Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      sourceStr = "Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      sourceStr = "Other";
      break;
    default:
      break;
  }
  return sourceStr;
}
const char* severityToStr(GLenum severity) {
  const char* severityStr = "Unkown Severity";
  switch (severity) {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      severityStr = "Notification Severity";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      severityStr = "Low Severity";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      severityStr = "Medium Severity";
      break;
    case GL_DEBUG_SEVERITY_HIGH:
      severityStr = "High Severity";
      break;
  }
  return severityStr;
}
const char* typeToStr(GLenum type) {
  const char* typeStr = "Unkown Type";
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      typeStr = "Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      typeStr = "Deprecated Behavior";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      typeStr = "Undefined Behavior";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      typeStr = "Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      typeStr = "Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      typeStr = "Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      typeStr = "Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      typeStr = "Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      typeStr = "Other";
      break;
  }
  return typeStr;
}
}  // namespace Magnet::Window