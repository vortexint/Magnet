#include "window.hpp"

WindowManager::WindowManager() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, GAME_TITLE, NULL, NULL);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Failed to create a GLFW window");
  }

  /* Icon loading code
  size_t dataSize;
  //auto imageData = assetMgr.LoadAsset("textures/your_image.png", dataSize);

  GLFWimage icons[1];
  icon[0].pixels = stbi_load_from_memory(const stbi_uc *buffer, int len, int *x, int *y, int *comp, int req_comp) glfwSetWindowIcon(window, 1, icons);
  */

  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, resize_callback);

  /* GLAD manages function pointers for OpenGL so we want to initialize GLAD
   * before we call any OpenGL function */
  gladLoadGL();

  /* Sample triangle shader code

  Shader ourShader("vert.glsl", "frag.glsl");

  float vertices[] = {
      // positions         // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
  };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).
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
  /*/
}

WindowManager::~WindowManager() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}