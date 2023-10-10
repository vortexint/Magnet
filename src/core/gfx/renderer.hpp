#pragma once

#include "shader_manager.hpp"

/* Game renderer */

class Renderer {

  AssetManager &asset_manager_;
  ShaderManager shader_manager_;

  //
  float vertices[18] = {
      // positions         // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
  };
  unsigned int ID, VBO, VAO;
  //

public:
  Renderer(AssetManager &assetManager)
      : asset_manager_(assetManager), shader_manager_(assetManager) {
    /* OpenGL configuration */
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    // Add other config like stencil, alpha blending etc. as needed

    //
    ID = shader_manager_.genShader("myShader", "shaders/vert.glsl",
                                   "shaders/frag.glsl");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //
  }

  void Clear() const {
    /* Assuming that we'll always render something to all pixels, this isn't
     * necessary but is considered good practice */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void Render() {

    glUseProgram(ID);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
  }
};