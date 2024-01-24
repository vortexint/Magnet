#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <magnet/ApplicationContext.hpp>
#include <magnet/InputManager.hpp>
#include <magnet/Time.hpp>
#include <magnet/Widgets.hpp>

#include "imgui.h"

namespace Magnet::Widgets {

bool show_console = false;
bool show_debug_info = false;
class DevToolsObserver : public Observer {
 public:
  void onKeyEvent(int key, int action, int) override {
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
      show_debug_info = !show_debug_info;
    }
  }
};

void DevTools() {
  static DevToolsObserver observer;

  ImGuiIO& io = ImGui::GetIO();

  /* Debug Info overlay*/
  if (show_debug_info) {
    ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    const float PAD = 10.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImGui::SetNextWindowPos(
      {
        (work_pos.x + PAD),
        (work_pos.y + PAD),
      },
      ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGui::Begin("Debug Info", &show_debug_info, window_flags);
    {
      ImGui::Text("Debug Info - F3");
      ImGui::Separator();
      ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
      ImGui::Separator();
      ImGui::Text("Cursor: (%d,%d)", (int)io.MousePos.x, (int)io.MousePos.y);
      ImGui::Separator();
      static const GLubyte* renderer = glGetString(GL_RENDERER);
      static const GLubyte* version = glGetString(GL_VERSION);
      ImGui::Text("%s", renderer);
      ImGui::Text("GL %s", version);
    }
    ImGui::End();
  }
}
}  // namespace Magnet::Widgets