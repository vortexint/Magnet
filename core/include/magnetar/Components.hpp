#pragma once

#include <cglm/cglm.h>

namespace magnetar::components {

/* Components */

// An entity's transformation relative to it's parent.
struct Transform {
  vec3 translation{0.0f, 0.0f, 0.0f};
  versor rotation{0.0f, 0.0f, 0.0f};
  vec3 scale{1.0f, 1.0f, 1.0f};
};

// RENDERING //

// The parameters and properties of a virtual camera in a scene.
// Requires a Transform component.
struct Camera {
  float fieldOfView = 60.0f;        // Angle defining visible scene extent.
  float aspectRatio = 16.0f / 9.0f; // Width-to-height ratio.
  float nearPlane = 0.1f;           // Distance to near clipping plane.
  float farPlane = 100.0f;          // Distance to far clipping plane.
};

struct MeshRenderer {};

// PHYSICS //

struct BoxCollider {
  float center, size;
};

struct SphereCollider {
  float center, size;
};

struct CapsuleCollider {
  float center, direction, height, radius;
};


// TODO: This.
// Requires Transform, Collider.
struct Rigidbody {
  bool fixed;
};

} // namespace magnetar