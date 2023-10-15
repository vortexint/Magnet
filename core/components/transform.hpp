#pragma once

#include "glm/glm.hpp"
struct Transform {
  glm::vec3 position;
  glm::vec3 rotation; // Euler Angles
  glm::vec3 scale;
};