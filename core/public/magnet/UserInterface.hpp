#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"

enum nk_glfw_init_state { NK_GLFW3_DEFAULT = 0, NK_GLFW3_INSTALL_CALLBACKS };

NK_API struct nk_context *nk_glfw3_init(GLFWwindow *win,
                                        enum nk_glfw_init_state,
                                        int max_vertex_buffer,
                                        int max_element_buffer);
NK_API void nk_glfw3_shutdown(void);
NK_API void nk_glfw3_font_stash_begin(struct nk_font_atlas **atlas);
NK_API void nk_glfw3_font_stash_end(void);
NK_API void nk_glfw3_new_frame(void);
NK_API void nk_glfw3_render(enum nk_anti_aliasing);

NK_API void nk_glfw3_device_destroy(void);
NK_API void nk_glfw3_device_create(void);

NK_API void nk_glfw3_char_callback(GLFWwindow *win, unsigned int codepoint);
NK_API void nk_gflw3_scroll_callback(GLFWwindow *win, double xoff, double yoff);
NK_API void nk_glfw3_mouse_button_callback(GLFWwindow *win, int button,
                                           int action, int mods);

NK_API GLuint nk_glfw3_get_tex_ogl_id(int tex_index);
NK_API GLuint64 nk_glfw3_get_tex_ogl_handle(int tex_index);
NK_API int nk_glfw3_create_texture(const void *image, int width, int height);
NK_API void nk_glfw3_destroy_texture(int tex_index);

namespace Magnet {

void nk_initialize();
nk_context* get_nk_context();

void DevTools(nk_context* ctx);

} // namespace Magnet