#include "magnet/Renderer.hpp"

#include <spdlog/spdlog.h>

#include "magnet/ApplicationContext.hpp"
#include "magnet/ShaderManager.hpp"
#include "magnet/GUI.hpp"
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

struct nk_context* ctx;

void SetupState() {
  /* OpenGL State configuration */
  glClearColor(0.f, 0.f, 0.f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  // Add other config like stencil, alpha blending etc. as needed
} 

Renderer::Renderer(flecs::world* scene, ShaderManager* shaderMgr) : ecs(scene), shaderManager(shaderMgr) {
  // Can't do much here, since GLFW has not been initialized yet
}

void Renderer::initialize() {
  WindowManager* windowManager = ApplicationContext::getWindowManager();

  /* GLAD manages function pointers for OpenGL so initialize GLAD
   * before any OpenGL function is called */
  gladLoadGL();

  /* Set up UI */
  ctx = nk_glfw3_init(windowManager->getWindow(), NK_GLFW3_INSTALL_CALLBACKS,
                      MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void Renderer::render() const {
  SetupState();
  /* Assuming that we'll always render something to all pixels, this
   * is not necessary but is considered good practice */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  enum { EASY, HARD };
  static int op = EASY;
  static float value = 0.6f;
  static int i = 20;

  if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                 NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
    /* fixed widget pixel width */
    nk_layout_row_static(ctx, 30, 80, 1);
    if (nk_button_label(ctx, "button")) {
      /* event handling */
    }

    /* fixed widget window ratio width */
    nk_layout_row_dynamic(ctx, 30, 2);
    if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
    if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

    /* custom widget pixel width */
    nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
    {
      nk_layout_row_push(ctx, 50);
      nk_label(ctx, "Volume:", NK_TEXT_LEFT);
      nk_layout_row_push(ctx, 110);
      nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
    }
    nk_layout_row_end(ctx);
  }
  nk_end(ctx);

  nk_glfw3_render(NK_ANTI_ALIASING_ON);
}

void Renderer::resize(int width, int height) {
  this->width = width;
  this->height = height;

  /* Set the viewport to be the entire window */
  glViewport(0, 0, width, height);
}