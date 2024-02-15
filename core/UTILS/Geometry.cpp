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

void GET_FORWARD(vec3 v) {
  vec3 forward = {1.f, 0.f, 0.f};
  glm_vec3_copy(forward, v);
}
void GET_UP(vec3 v) { 
  vec3 up = {0.f, 1.f, 0.f};
  glm_vec3_copy(up, v);
}
void GET_RIGHT(vec3 v) {
  vec3 right = {0.f, 0.f, 1.f};
  glm_vec3_copy(right, v);
}

}