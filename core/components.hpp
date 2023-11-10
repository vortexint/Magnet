#pragma once

#include "cglm/cglm.h"

namespace magnetar {

/* Components */

struct Camera {
  float fieldOfView; // The angle that defines the extent of the scene that is
                     // visible to the camera.
  float aspectRatio; // The ratio of the camera's width to its height.
  float nearPlane;   // The distance to the near clipping plane, defining the
                     // closest point at which objects are visible.
  float farPlane;    // The distance to the far clipping plane, defining the
                     // farthest point at which objects are visible.
};


struct Rigidbody {
  bool fixed;
};

struct Transform {
  vec3 translation;
  versor rotation;
  vec3 scale;
};


} // namespace magnetar