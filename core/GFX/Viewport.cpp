#include "Viewport.hpp"
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include <magnet/Components.hpp>
#include <magnet/Shaders.hpp>

#include <spdlog/spdlog.h> 

namespace Magnet {
using namespace Components;

flecs::entity Viewport::activeCamera;
int Viewport::width, Viewport::height;

void Viewport::updateFrustum(GLuint baseID) {
  /* Camera */
  Camera* camera = activeCamera.get_mut<Camera>();
  Transform* transform = activeCamera.get_mut<Transform>();
  mat4 model, view, projection;
  glm_mat4_identity(model);
  glm_mat4_identity(view);
  glm_mat4_identity(projection);

  /* Update view matrix */
  vec3 cameraPos;
  versor cameraRot;
  vec3 direction = {0.0f, 0.0f, -1.0f};

  glm_vec3_copy(transform->position, cameraPos);
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

  glm_perspective(
    glm_rad(camera->fieldOfView),
    static_cast<float>(Viewport::width) / static_cast<float>(Viewport::width),
    camera->clippingPlanes[0], camera->clippingPlanes[1], projection);

  /* Update shader uniforms */
  Shaders::setMat4(baseID, "model", model);
  Shaders::setMat4(baseID, "view", view);
  Shaders::setMat4(baseID, "projection", projection);
}

void Viewport::setSize(GLFWwindow*, int width, int height) {
  Viewport::width = width;
  Viewport::height = height;
  glViewport(0, 0, width, height);
}

void Viewport::setActiveCamera(flecs::entity camera) {
  activeCamera = camera;
}

}  // namespace Magnet