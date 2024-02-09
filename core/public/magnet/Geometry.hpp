#pragma once

#include <cglm/cglm.h>

namespace Magnet {
namespace Components {
  struct Transform;
}
  

struct AABB {
  vec3 box[2];

  static AABB fromPoints2(vec3 a, vec3 b);
  static AABB fromTransform(const Components::Transform& transform);
  bool contains(vec3 a);
};
}