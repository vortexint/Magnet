#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include "UserInterface.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// This is a small wrapper over Dear ImGui to detach backend
// functionality from the WindowManager and Renderer

namespace Magnet {

UI::UI() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;

  ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), false);
  ImGui_ImplOpenGL3_Init("#version 450");

  /* Load fonts */
  {
    AssetManager* assetManager = ApplicationContext::getAssetManager();
    ImFontConfig cfg;

    cfg.OversampleH = 1;
    cfg.OversampleV = 1;
    cfg.PixelSnapH = 1;

    assetManager->getAsset("fonts/Roboto-Regular.ttf", fontData);
    spdlog::info("Loaded {} bytes of font data", fontData.size());

    io.Fonts->ClearFonts();
    io.Fonts->AddFontFromMemoryTTF(fontData.data(), fontData.size(), 16.0f,
                                   &cfg);
  }
  /* Set theme */
}

UI::~UI() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void UI::newFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void UI::draw() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace Magnet