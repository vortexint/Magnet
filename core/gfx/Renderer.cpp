#include <magnetar/Renderer.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Different variable name for initializer disambiguation
Renderer::Renderer(flecs::world* scene) : ecs(scene) {

}

void Renderer::resizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}