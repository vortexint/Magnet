#pragma once

#include "shader.hpp"

class Renderer {

public:
  Renderer() {
    /* OpenGL configuration */
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
    glEnable(GL_DEPTH_TEST); // for 3D
    // Add other config like stencil, alpha blending etc. as needed
  }

  void Clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
};