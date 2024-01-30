#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/Components.hpp>
#include <magnet/Renderer.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/ShaderManager.hpp>

#include "Viewport.hpp"

namespace Magnet {
using namespace Components;

GLuint baseID;

// TESTING
float vertices[18] = {
  // positions         // colors
  0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f   // top
};

float texCoords[] = {
  0.0f, 0.0f,  // lower-left corner
  1.0f, 0.0f,  // lower-right corner
  0.5f, 1.0f   // top-center corner
};

unsigned int ID, VBO, VAO;
//

Renderer::Renderer() {
  /* OpenGL state configuration */
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  /* Generate default Shader(s)*/
  ApplicationContext& appCtx = ApplicationContext::getInstance();
  ArchiveManager& archiveMgr = appCtx.getArchiveManager();

  std::string vertexSource, fragmentSource;
  archiveMgr.loadFile("shaders/base.vert", vertexSource);
  archiveMgr.loadFile("shaders/base.frag", fragmentSource);

  ShaderManager& shaderMgr = ShaderManager::getInstance();
  baseID = shaderMgr.genShader(vertexSource, fragmentSource);

  // TESTING
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  /* bind Vertex Array Object first then bind and set vertex buffer(s),
   * then configure vertex attributes(s). */
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //
}

void Renderer::drawFrame() const {
  Viewport::updateFrustum(baseID);

  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render scene */
  glUseProgram(baseID);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

}  // namespace Magnet