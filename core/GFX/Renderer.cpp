#include <spdlog/spdlog.h>

#include <magnet/Renderer.hpp>
#include <magnet/ShaderManager.hpp>
#include <magnet/UserInterface.hpp>

namespace Magnet::Renderer {

void initialize() {
  /* OpenGL State configuration */
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_MULTISAMPLE);
}

void terminate() {
  UI::destroy();
}

void drawFrame() {
  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render scene */

  /* Render UI */
  UI::draw();
}

void setSize(int width, int height) {
  /* Set the viewport to be the entire window */
  glViewport(0, 0, width, height);
}

}  // namespace Magnet::Renderer