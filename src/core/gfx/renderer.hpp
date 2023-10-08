#pragma once

#include "shader.hpp"

/* Game renderer */

class Renderer {

public:
  Renderer() {
    /* OpenGL configuration */
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
    glEnable(GL_DEPTH_TEST); // for 3D
    // Add other config like stencil, alpha blending etc. as needed
  }

  void Clear() const {
    /* Assuming that we'll always render something to all pixels, this isn't
     * necessary but is considered good practice */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void Render() {

  }
};