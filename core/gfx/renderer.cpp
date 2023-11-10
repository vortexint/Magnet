#include "renderer.hpp"

// TESTING
float vertices[18] = {
    // positions         // colors
    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
};

float texCoords[] = {
    0.0f, 0.0f, // lower-left corner
    1.0f, 0.0f, // lower-right corner
    0.5f, 1.0f  // top-center corner
};

unsigned int ID, VBO, VAO;
//

Renderer::Renderer(AssetManager &assetManager)
    : assetMgr_(assetManager), shaderMgr_(assetManager) {
  /* OpenGL configuration */
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  // Add other config like stencil, alpha blending etc. as needed

  // TESTING
  ID = shaderMgr_.genShader("myShader", "shaders/vert.glsl",
                                 "shaders/frag.glsl");

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s),
  // and then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //
}

void Renderer::Clear() const {
  /* Assuming that we'll always render something to all pixels, this isn't
   * necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Render() const {

  glUseProgram(ID);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::SetCamera(const magnetar::Camera &camera) {
  // Maybe?
  // shaderMgr_.SetCameraParameters(camera.fieldOfView, camera.aspectRatio,
  //                               camera.nearPlane, camera.farPlane);
}