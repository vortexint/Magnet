#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/UserInterface.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// This is a small wrapper over Dear ImGui to detach backend
// functionality from the WindowManager and Renderer

namespace Magnet::UI {

void init() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
 
  ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), false);
  ImGui_ImplOpenGL3_Init("#version 450");

  /* Load default fonts */

  /* Set theme */
}

void destroy() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void newFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void draw() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace Magnet::UI