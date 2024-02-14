#include "app.hpp"

#include <GLFW/glfw3.h>

#include <magnet/Time.hpp>
#include <magnet/Widgets.hpp>

void App::init() {
  // create default logger
  spdlog::logger logger("app");
  
}

void App::update() {
  static float value = 0.0f;
  static bool show_demo_window = true;
  if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

  if (value > 100.0f) value = 0.0f;
  value += Time::getDelta(getTimeState()) * 10.0f;

  ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Window Test")) {
    ImGui::SliderFloat("Value", &value, 0.0f, 100.0f, "%.1f");

    ImGui::Text("Animated Progress bar");
    ImGui::ProgressBar(value / 100.0f, ImVec2(-1.0f, 0.0f));

    if (ImGui::Button("No-op")) {
      // No operation
    }

    ImGui::SameLine();

    if (ImGui::Button("Button")) {
      spdlog::info("Button clicked");
    }
  }
  ImGui::End();

  Widgets::DevTools(getECS());
}