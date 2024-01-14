#include "magnet/Renderer.hpp"

#include <spdlog/spdlog.h>

#include <magnet/ShaderManager.hpp>
#include <magnet/UserInterface.hpp>
#include <magnet/Time.hpp>

namespace Magnet {

void SetupState() {
  /* OpenGL State configuration */
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
} 

Renderer::Renderer(flecs::world* scene, ShaderManager* shaderMgr) : ecs(scene), shaderManager(shaderMgr) {
  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  SetupState();

  Magnet::nk_initialize();
}

void Renderer::render() const {
  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Renderer::resize(int width, int height) {
  this->width = width;
  this->height = height;

  /* Set the viewport to be the entire window */
  glViewport(0, 0, width, height);
}

}  // namespace Magnet