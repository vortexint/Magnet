#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/Components.hpp>
#include <magnet/Renderer.hpp>
#include <magnet/SceneManager.hpp>
#include <magnet/ShaderManager.hpp>

namespace Magnet {

using namespace Magnet::Components;

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
  AssetManager& assetMgr = appCtx.getAssetManager();

  std::string vertexSource, fragmentSource;
  assetMgr.getAsset("shaders/base.vert", vertexSource);
  assetMgr.getAsset("shaders/base.frag", fragmentSource);

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
  static ShaderManager& shaderMgr = ShaderManager::getInstance();
  /* Camera */
  {
    Camera* camera = activeCameraEntity.get_mut<Camera>();
    Transform* transform = activeCameraEntity.get_mut<Transform>();
    mat4 model, view, projection;
    glm_mat4_identity(model);
    glm_mat4_identity(view);
    glm_mat4_identity(projection);

    /* Update view matrix */
    vec3 cameraPos;
    versor cameraRot;
    vec3 direction = {0.0f, 0.0f, -1.0f};

    glm_vec3_copy(transform->translation, cameraPos);
    glm_quat_copy(transform->rotation, cameraRot);

    glm_quat_rotatev(cameraRot, direction, direction);
    glm_normalize(direction);

    // up
    vec3 up = {0.0f, 1.0f, 0.0f};
    glm_quat_rotatev(cameraRot, up, up);
    glm_normalize(up);

    // Calculate the target position
    vec3 target;
    glm_vec3_add(cameraPos, direction, target);

    glm_lookat(cameraPos, target, up, view);

    /* Update projection matrix */

    glm_perspective(glm_rad(camera->fieldOfView),
                    static_cast<float>(width) / static_cast<float>(height),
                    camera->clippingPlanes[0], camera->clippingPlanes[1],
                    projection);

    /* Update shader uniforms */
    shaderMgr.setMat4(baseID, "model", model);
    shaderMgr.setMat4(baseID, "view", view);
    shaderMgr.setMat4(baseID, "projection", projection);
  }

  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render scene */
  glUseProgram(baseID);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::setViewportSize(int width, int height) {
  this->width = width;
  this->height = height;
  glViewport(0, 0, width, height);
}

void Renderer::setActiveCamera(flecs::entity camera) {
  activeCameraEntity = camera;
}

}  // namespace Magnet