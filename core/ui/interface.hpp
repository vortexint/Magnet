#pragma once

#include <memory>

#include "magnet/WindowManager.hpp"
#include "magnet/Renderer.hpp"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"

enum nk_glfw_init_state { NK_GLFW3_DEFAULT = 0, NK_GLFW3_INSTALL_CALLBACKS };

NK_API struct nk_context* nk_init(GLFWwindow* win, enum nk_glfw_init_state,
                                  int max_vertex_buffer,
                                  int max_element_buffer);

NK_API struct nk_context* nk_get_nk_ctx(struct nk_context* nk_dlg_ctx);

NK_API void nk_font_stash_begin(struct nk_font_atlas** atlas);

NK_API void nk_font_stash_end(void);

NK_API void nk_render(enum nk_anti_aliasing);

NK_API void nk_resize(struct nk_context* nk_ctx, unsigned int width,
                      unsigned int height);

NK_API void nk_shutdown(struct nk_context* nk_ctx);

#ifdef NK_GLFW_GL4_IMPLEMENTATION
#undef NK_GLFW_GL4_IMPLEMENTATION

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define NK_GLFW_DOUBLE_CLICK_LO 0.02
#define NK_GLFW_DOUBLE_CLICK_HI 0.2
#define NK_GLFW_TEXT_MAX 256
#define NK_GLFW_MAX_TEXTURES 256

struct nk_glfw {
  GLFWwindow* win;
  int width, height;
  int display_width, display_height;
  struct nk_context ctx;
  struct nk_font_atlas atlas;
  struct nk_vec2 fb_scale;
  unsigned int text[NK_GLFW_TEXT_MAX];
  int text_len;
  struct nk_vec2 scroll;
  double last_button_click;
  int is_double_click_down;
  struct nk_vec2 double_click_pos;
};

#endif
