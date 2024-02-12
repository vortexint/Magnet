#include "Pipeline.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/Application.hpp>
#include <magnet/Components.hpp>
#include <magnet/Scene.hpp>
#include <magnet/Shaders.hpp>
#include <magnet/Rendering.hpp>

#include "Viewport.hpp"
#include "UI/UserInterface.hpp"

namespace Magnet::Rendering {
using namespace Components;

GLuint baseID;

std::queue<Command>& getCmdBuffer() {
  static std::queue<Command> cmdBuffer;
  return cmdBuffer;
}

void setupPipeline(ArchiveManager& archiveMgr) {
  /* OpenGL state configuration */
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  /* Generate default Shader(s) */

  std::string vertexSource, fragmentSource;
  archiveMgr.loadFile("shaders/base.vert", vertexSource);
  archiveMgr.loadFile("shaders/base.frag", fragmentSource);

  baseID = Shaders::generate(vertexSource, fragmentSource);
}

void drawFrame() {
  Viewport::updateFrustum(baseID);

  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render scene */
  glUseProgram(baseID);

  UI::draw();
}

}  // namespace Magnet::Rendering