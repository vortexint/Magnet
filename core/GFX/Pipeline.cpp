#include "Pipeline.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/Application.hpp>
#include <magnet/Components.hpp>
#include <magnet/Rendering.hpp>
#include <magnet/Scene.hpp>
#include <magnet/Shaders.hpp>

#include "GFX/Window.hpp"
#include "UI/UserInterface.hpp"
#include "Viewport.hpp"

namespace Magnet::Rendering {
using namespace Components;

GLuint baseID;

void setupPipeline() {
  /* OpenGL state configuration */
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  /* Generate default Shader(s) */
  {
    ArchiveManager& archiveMgr = Application::getArchiveManager();
    std::string vertexSource, fragmentSource;
    archiveMgr.loadFile("shaders/base.vert", vertexSource);
    archiveMgr.loadFile("shaders/base.frag", fragmentSource);

    baseID = Shaders::generate(vertexSource, fragmentSource);
  }
}

void setupPipeline(const PipelineConfig& config) {
  setupPipeline();
  if (config.VSync) {
    glfwSwapInterval(1);
  } else {
    glfwSwapInterval(0);
  }
}

void updatePipeline(GLFWwindow* window) {
  Viewport::updateFrustum(baseID);

  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render scene */
  glUseProgram(baseID);

  /* Render Native UI*/
  UI::draw();

  glfwSwapBuffers(window);
}

}  // namespace Magnet::Rendering