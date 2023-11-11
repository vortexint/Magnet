#include "renderer.hpp"
#include "components.hpp"

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

Renderer::Renderer(AssetManager &assetManager, SceneManager &sceneManager)
    : assetMgr_(assetManager), shaderMgr_(assetManager), sceneMgr_(sceneManager) {
  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  /* OpenGL configuration */
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  // Add other config like stencil, alpha blending etc. as needed

  // TESTING
  ID = shaderMgr_.genShader("myShader", "shaders/vert.glsl",
                            "shaders/frag.glsl");

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  /* bind Vertex Array Object first then bind and set vertex buffer(s),
   * then configure vertex attributes(s). */
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

  mat4 projection, view;

  // glm_perspective(
  //   glm_rad(activeCamera_.fieldOfView),
  //   activeCamera_.aspectRatio,
  //   activeCamera_.nearPlane,
  //   activeCamera_.farPlane,
  //   projection
  //);

  // glm_lookat(float *eye, float *center, float *up, vec4 *dest)

  // update the matrices in the vertex shader

  glUseProgram(ID);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}