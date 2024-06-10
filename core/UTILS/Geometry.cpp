#include <array>

#include <magnet/Geometry.hpp>

#include <magnet/Components.hpp>

namespace Magnet {
  
AABB AABB::fromPoints2(vec3 a, vec3 b) {
  AABB aabb;

  glm_vec3_minv(a, b, aabb.box[0]);
  glm_vec3_maxv(a, b, aabb.box[1]);

  return aabb;
}
bool AABB::contains(vec3 a) {
  return glm_aabb_point(box, a);
}

AABB AABB::fromTransform(const Magnet::Components::Transform& hTransform) {
  auto transform = hTransform;

  vec3 corners[2];
  corners[0][0] = -0.5f;
  corners[0][1] = -0.5f;
  corners[0][2] = -0.5f;
  corners[1][0] = 0.5f;
  corners[1][1] = 0.5f;
  corners[1][2] = 0.5f;

  for (size_t i = 0; i < 2; ++i) {
    glm_quat_rotatev(transform.rotation, corners[i], corners[i]);
    glm_vec3_mul(transform.scale, corners[i], corners[i]);
    glm_vec2_add(transform.position, corners[i], corners[i]);
  }

  return AABB::fromPoints2(corners[0], corners[1]);
}
AABB AABB::fromPositionAndSize(vec3 pos, vec3 scale) {
  vec3 corners[2];
  corners[0][0] = -0.5f;
  corners[0][1] = -0.5f;
  corners[0][2] = -0.5f;
  corners[1][0] = 0.5f;
  corners[1][1] = 0.5f;
  corners[1][2] = 0.5f;

  
  for (size_t i = 0; i < 2; ++i) {
    glm_vec3_mul(scale, corners[i], corners[i]);
    glm_vec2_add(pos, corners[i], corners[i]);
  }

  return AABB::fromPoints2(corners[0], corners[1]);
}
bool AABB::rotatedAABBContains(versor hRot, vec3 hPoint) {
  // Instead of rotating the aabb and see if that contains the point
  // We can instead rotate the point with the inverse rotation around
  // the center of the ABB, and check if the original AABB contains
  // the new rotated point

  versor rot = {hRot[0], hRot[1], hRot[2], hRot[3]};
  vec3 point = {hPoint[0], hPoint[1], hPoint[2]};

  glm_quat_inv(rot, rot); // Inverts the quaterion
  // If the original quaternion rotate x degrees around an axis
  // the inverse will rotate -x degrees around the same axis

  vec3 center = {};
  this->getCenter(center);
  glm_vec3_sub(point, center, point);
  glm_quat_rotatev(rot, point, point);
  glm_vec3_add(point, center, point);

  return contains(point);
}


std::array<std::array<float, 3>, 8> AABB::corners() {
  std::array<std::array<float, 3>, 8> boxCorners = {
    std::array<float, 3>{box[0][0], box[1][1], box[1][2]},
    std::array<float, 3>{box[0][0], box[1][1], box[0][2]},
    std::array<float, 3>{box[1][0], box[1][1], box[0][2]},
    std::array<float, 3>{box[1][0], box[1][1], box[1][2]},
    std::array<float, 3>{box[0][0], box[0][1], box[1][2]},
    std::array<float, 3>{box[0][0], box[0][1], box[0][2]},
    std::array<float, 3>{box[1][0], box[0][1], box[0][2]},
    std::array<float, 3>{box[1][0], box[0][1], box[1][2]}
  };

  return boxCorners;
}
void AABB::getCenter(vec3 center) {
  vec3 hBox[2] = {{box[0][0], box[0][1], box[0][2]},
                  {box[1][0], box[1][1], box[1][2]}};

  glm_aabb_center(hBox, center);
}


void TRS::toMat4(mat4 m) const {
  glm_mat4_identity(m);
  mat4 rot = {};

  versor this_rotation = {this->rotation[0], this->rotation[1], this->rotation[2]};
  glm_quat_mat4(this_rotation, rot);
  glm_mat4_mul(rot, m, m);

  vec3 this_scale = {this->scale[0], this->scale[1], this->scale[2]};
  mat4 scale = {};
  glm_scale_make(scale, this_scale);
  glm_mat4_mul(scale, m, m);

  vec3 this_translation = {this->translation[0], this->translation[1],
                           this->translation[2]};
  mat4 translate = {};
  glm_translate_make(translate, this_translation);
  glm_mat4_mul(translate, m, m);
}


void GET_FORWARD(vec3 v) {
  vec3 forward = {0.f, 0.f, -1.f};
  glm_vec3_copy(forward, v);
}
void GET_UP(vec3 v) { 
  vec3 up = {0.f, 1.f, 0.f};
  glm_vec3_copy(up, v);
}
void GET_RIGHT(vec3 v) {
  vec3 right = {1.f, 0.f, 0.f};
  glm_vec3_copy(right, v);
}
}