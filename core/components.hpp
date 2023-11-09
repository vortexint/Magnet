#pragma once

#include "cglm/cglm.h"

namespace magnetar {

/* Components */

// TRANSFORM

struct Transform{
  vec3 translation;
  versor rotation;
  vec3 scale;
};

// RIGIDBODY 

struct Rigidbody {

};

} // namespace magnetar