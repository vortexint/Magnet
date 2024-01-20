#include "app.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <magnet/Time.hpp>


void Interface::init() {


}

void Interface::update() {

  static float value = 0.0f;
  static bool show_demo_window = true;
  if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

  if (value > 100.0f) value = 0.0f;
  value += Time::getDelta() * 10.0f;


  if (ImGui::Begin("Window Test")) {
    ImGui::Text("Value:");
    ImGui::SameLine();
    ImGui::SliderFloat("##Value", &value, 0.0f, 100.0f, "%.1f");

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
}