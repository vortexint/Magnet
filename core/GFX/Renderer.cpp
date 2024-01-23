#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/AssetManager.hpp>
#include <magnet/Renderer.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/ShaderManager.hpp>

namespace Magnet {

GLuint baseID;

Renderer::Renderer() {
  /* OpenGL state configuration */
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  /* Generate default Shader(s)*/
  AssetManager* assetMgr = ApplicationContext::getAssetManager();

  std::string vertexSource, fragmentSource;
  assetMgr->getAsset("shaders/base.vert", vertexSource);
  assetMgr->getAsset("shaders/base.frag", fragmentSource);

  ShaderManager& shaderMgr = ShaderManager::getInstance();
  baseID = shaderMgr.genShader(vertexSource, fragmentSource);
}

void Renderer::drawFrame() const {
  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render scene */

}

void Renderer::setViewportSize(int width, int height) {
  glViewport(0, 0, width, height);
}

}  // namespace Magnet::Renderer