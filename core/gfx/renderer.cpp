#include "magnet/Renderer.hpp"
#include "magnet/ShaderManager.hpp"

#include <spdlog/spdlog.h>

// TESTING
#include <GLFW/glfw3.h>
static const GLfloat cubeVertices[] = {
  -1.0, -1.0, 1.0,  1.0, -1.0, 1.0,  -1.0, 1.0, 1.0,  1.0, 1.0, 1.0,
  -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0,
};

static const GLushort cubeIndices[] = {0, 1, 2, 3, 7, 1, 5,
                                       4, 7, 6, 2, 4, 0, 1};

float vertices[18] = {
  // positions         // colors
  0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,       // bottom right
  -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,     // bottom left
  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
};

float texCoords[] = {
  0.0f, 0.0f,  // lower-left corner
  1.0f, 0.0f,  // lower-right corner
  0.5f, 1.0f   // top-center corner
};

unsigned int TriangleProgramID, TriVBO, TriVAO;
//

// Different variable name for initializer disambiguation
Renderer::Renderer(flecs::world* scene, ShaderManager* shaderMgr) : ecs(scene), shaderManager(shaderMgr) {
  // Can't do much here, since GLFW has not been initialized yet
}

void Renderer::initialize() {
  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  /* OpenGL configuration */
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  // Add other config like stencil, alpha blending etc. as needed

  // TESTING

  TriangleProgramID = shaderManager->genShader("myShader", "shaders/base.vert",
                                               "shaders/base.frag");

  glGenVertexArrays(1, &TriVAO);
  glGenBuffers(1, &TriVBO);
  /* bind Vertex Array Object first then bind and set vertex buffer(s),
   * then configure vertex attributes(s). */
  glBindVertexArray(TriVAO);

  glBindBuffer(GL_ARRAY_BUFFER, TriVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //
}

void Renderer::render() const {
  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TESTING

  // Draw skybox first
  glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when
                           // values are equal to depth buffer's content

  mat4 model, view, projection;
  glm_mat4_identity(model);
  glm_mat4_identity(view);
  glm_mat4_identity(projection);

  // Create basic transformations
  glm_rotate(model, (float)glfwGetTime(), (vec3){0.0f, 1.0f, 0.0f});

  // Or for static triangle without animation replace line above with just
  // glm_mat4_identity(model);

  glm_lookat(
    (vec3){0.0f, 0.0f, 3.0f},  // Camera is at (0,0,3), in World Space
    (vec3){0.0f, 0.0f, 0.0f},  // and looks at the origin
    (vec3){0.0f, 1.0f, 0.0f},  // Head is up (set to 0,-1,0 to look upside-down)
    view);

  // 45 degrees field of view, 4:3 aspect ratio, display range: 0.1 unit <-> 100
  // units
  glm_perspective(glm_rad(45.0f), (float)this->width / (float)this->height, 0.1f, 100.0f,
                  projection);

  // Pass the matrices to the shader
  shaderManager->setMat4("myShader", "model", model);
  shaderManager->setMat4("myShader", "view", view);
  shaderManager->setMat4("myShader", "projection", projection);

  // Draw triangle
  glUseProgram(TriangleProgramID);
  glBindVertexArray(TriVAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  // TESTING
}

void Renderer::resize(int width, int height) {
  this->width = width;
  this->height = height;

  /* Set the viewport to be the entire window */
  glViewport(0, 0, width, height);
}