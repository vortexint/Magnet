#include <magnet/Widgets.hpp>

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/Application.hpp>
#include <magnet/Input.hpp>
#include <magnet/Time.hpp>
#include <magnet/Scene.hpp>

#include "imgui.h"
#include "implot.h"

namespace Magnet::Widgets {

bool show_console = false;
bool show_debug_info = false;
bool show_manipulate = false;
class DevToolsObserver : public Input::Observer {
 public:
  void onKeyEvent(int key, int, int action, int mods) override {
    // F3: Debug Info
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
      show_debug_info = !show_debug_info;
    }
    // Shift + F10: Manipulate
    if (key == GLFW_KEY_F10 && mods == GLFW_MOD_SHIFT) {
      show_manipulate = !show_manipulate;
    }
  }
};

void DevTools(Context* ctx) {
  static DevToolsObserver observer;
  static ImGuiIO& io = ImGui::GetIO();

  /* Layout info */
  static const float PAD = 10.0f;
  static const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImVec2 work_pos = viewport->WorkPos;
  ImVec2 work_size = viewport->WorkSize;

  /* Debug overlay*/
  if (show_debug_info) {
    static ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowPos(
      {
        (work_pos.x + PAD),
        (work_pos.y + PAD),
      },
      ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGui::Begin("Debug Info", &show_debug_info, window_flags);
    {
      ImGui::Text("Performance");
      ImGui::Separator();
      ImGui::Text("FPS: %.1f", Time::getFPS(ctx->getTimeState()));
      ImGui::Text("Frame Time: %.5f ms", Time::getDelta(ctx->getTimeState()));

      /* FrameTime graph */
      static ImPlotFlags flags = ImPlotAxisFlags_NoDecorations;
      static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations;
      // circular buffer
      static float values[1000] = {0};
      static int offset = 0;
      values[offset] = Time::getDelta(ctx->getTimeState());
      offset = (offset + 1) % IM_ARRAYSIZE(values);

      if (ImPlot::BeginPlot("##FrameTime Graph", ImVec2(-1, 50), flags)) {
        ImPlot::SetupAxes(nullptr, nullptr, axis_flags, axis_flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, IM_ARRAYSIZE(values), false);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 0.001, false);
        ImPlot::PlotLine("Frame Time", values, IM_ARRAYSIZE(values), offset);
        ImPlot::EndPlot();
      }

      ImGui::Separator();
      ImGui::Text("Cursor: %d,%d", static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));
    }
    ImGui::End();

    // Vendor Info window
    ImGui::SetNextWindowPos(
      {
        (viewport->WorkPos.x + viewport->WorkSize.x - PAD),
        (viewport->WorkPos.y + PAD),
      },
      ImGuiCond_Always, {1.0f, .0f});
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGui::Begin("Vendor Info", &show_debug_info, window_flags);
    {
      static const GLubyte* renderer = glGetString(GL_RENDERER);
      static const GLubyte* version = glGetString(GL_VERSION);
      ImGui::Text("%s", renderer);
      ImGui::Text("GL %s", version);
    }
    ImGui::End();
  }

  /* Manipulate (Entity list and Property Editor) */
  if (show_manipulate) {
    static flecs::entity selectedEntity;

    // static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration;
    ImGui::SetNextWindowPos(
      {
        (work_pos.x + PAD),
        (work_pos.y + work_size.y - PAD),
      },
      ImGuiCond_Always, {.0f, 1.0f});

    ImGui::Begin("Manipulate", &show_manipulate);
    {
      ImGui::BeginChild("Entity List", {200, 200}, true);
      {

      }
      ImGui::EndChild();
      ImGui::BeginChild("Property Editor", {200, 200}, true);
      {
        if (selectedEntity) {
          // get all of the entity's components

        }
      }
      ImGui::EndChild();
    }
    ImGui::End();
  }
}
}  // namespace Magnet::Widgets