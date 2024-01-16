#include <spdlog/spdlog.h>

#include <magnet/Renderer.hpp>
#include <magnet/ShaderManager.hpp>
#include <magnet/UserInterface.hpp>

namespace Magnet {

void SetupState() {
  /* OpenGL State configuration */
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
}

Renderer::Renderer() {
  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  SetupState();

  Magnet::nk_initialize();
}

void Renderer::render() {
  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render UI (After everything else) */
  nk_impl_render(NK_ANTI_ALIASING_ON);
}

void Renderer::setSize(int width, int height) {
  this->width = width;
  this->height = height;

  /* Set the viewport to be the entire window */
  glViewport(0, 0, width, height);
}

}  // namespace Magnet