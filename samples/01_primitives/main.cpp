#include <../GFX/Model.hpp>
#include <magnet/Application.hpp>
#include <magnet/Input.hpp>
#include <memory>

#include "imgui.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace Magnet;

const char* const TITLE = "Primitives Example";

class App : public Magnet::Context, public Magnet::Input::Observer {
  struct InputState {
    bool cursorGrabbed = false;
    vec2 mousePos = {0.f, 0.f};
  } input = {};

 public:
  ~App() { Magnet::Input::removeObserver(this); }
  virtual void onKeyEvent(int key, int scancode, int action,
                          int mods) override {
    if (ImGui::GetIO().WantCaptureKeyboard) {
      return;
    }
    auto& camera = TempModelRenderer::get().camera;

    vec3 delta = {0.f, 0.f, 0.f};
    const float MOVE_SPEED = 0.1f;

    switch (key) {
      case GLFW_KEY_W: {
        vec3 forward = {};
        camera.getForward(forward);
        glm_vec3_scale(forward, MOVE_SPEED, forward);
        glm_vec3_add(camera.pos, forward, camera.pos);
      } break;
      case GLFW_KEY_A: {
        vec3 right = {};
        camera.getRight(right);
        glm_vec3_scale(right, -MOVE_SPEED, right);
        glm_vec3_add(camera.pos, right, camera.pos);
      } break;
      case GLFW_KEY_S: {
        vec3 forward = {};
        camera.getForward(forward);
        glm_vec3_scale(forward, -MOVE_SPEED, forward);
        glm_vec3_add(camera.pos, forward, camera.pos);
      } break;
      case GLFW_KEY_D: {
        vec3 right = {};
        camera.getRight(right);
        glm_vec3_scale(right, MOVE_SPEED, right);
        glm_vec3_add(camera.pos, right, camera.pos);
      } break;
      case GLFW_KEY_UP: {
        vec3 up = {};
        camera.getUp(up);
        glm_vec3_scale(up, 1 / 60.f, up);
        glm_vec3_add(camera.pos, up, camera.pos);

      } break;
      case GLFW_KEY_DOWN: {
        vec3 up = {};
        camera.getUp(up);
        glm_vec3_scale(up, -1 / 60.f, up);
        glm_vec3_add(camera.pos, up, camera.pos);

      } break;
    }
  }
  virtual void onMouseButtonEvent(int button, int action, int mods) override {
    if (ImGui::GetIO().WantCaptureMouse) {
      return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      input.cursorGrabbed = (action == GLFW_PRESS);
      if (input.cursorGrabbed) {
        glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      } else {
        glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }
    }
  }
  virtual void onMouseMoveEvent(double xpos, double ypos) override {
    if (ImGui::GetIO().WantCaptureMouse) {
      return;
    }
    if (input.cursorGrabbed) {
      vec2 deltaMousePos = {};
      deltaMousePos[0] = static_cast<float>(xpos) - input.mousePos[0];
      deltaMousePos[1] = -(static_cast<float>(ypos) - input.mousePos[1]);

      glm_vec2_normalize(deltaMousePos);

      rotateCameraWithDeltaMouse(deltaMousePos);
    }

    input.mousePos[0] = static_cast<float>(xpos);
    input.mousePos[1] = static_cast<float>(ypos);
  }
  virtual void onMouseScrollEvent(double xoffset, double yoffset) override {
    if (ImGui::GetIO().WantCaptureMouse) {
      return;
    }
  }
  void init() override {
    // ArchiveManager archiveMgr(ARCH_assets);

    ArchiveManager archiveMgr("assets.magnet");

    std::vector<uint8_t> buffer;

    buffer.clear();
    archiveMgr.loadFile("sphere.glb", buffer);
    Library::ID modelId = Library::enqueueLoad(Library::Mimetype::GLB, buffer);
    TempModelRenderer::get().models.push_back(modelId);
    TempModelRenderer::get().camera.pos[2] = 12;

    {
      auto* window = this->getWindow();
      int width = 1.f, height = 1.f;
      glfwGetWindowSize(window, &width, &height);
      TempModelRenderer::get().width = static_cast<float>(width);
      TempModelRenderer::get().height = static_cast<float>(height);
    }
  }

  void update() override {
    ImGui::Begin("Primitives");
    {
      auto* cameraRot = TempModelRenderer::get().camera.rot;
      ImGui::DragFloat4("camera rot", cameraRot, 0.01f);
      glm_vec4_divs(cameraRot, glm_vec4_norm(cameraRot), cameraRot);
      ImGui::DragFloat3("camera pos", TempModelRenderer::get().camera.pos,
                        0.01f);
      vec3 forward = {0.f, 0.f, -1.f};
      glm_quat_rotatev(cameraRot, forward, forward);
      ImGui::DragFloat3("camera dir", forward);
    }

    ImGui::Text("Click and hold on the screen to start rotating the camera");
    ImGui::Text(
      "Use WASD to move around, use Up arrow and Down arrow to ascend and "
      "descend");

    static size_t selectedModelIndex = 0;
    for (size_t i = 0; i < TempModelRenderer::get().models.size(); ++i) {
      std::string name = "Model " + std::to_string(i) + "##Primitives_Model_" +
                         std::to_string(i);
      if (ImGui::Selectable(name.c_str(), selectedModelIndex == i)) {
        selectedModelIndex = i;
      }
      if (selectedModelIndex == i) {
        auto& model = TempModelRenderer::get().models[i];
        ImGui::DragFloat3("pos##Primitive_ModelPos", model.pos, 0.01f);
        ImGui::DragFloat3("scale##Primitive_ModelScale", model.scale, 0.01f);
        ImGui::DragFloat4("rot##Primitive_ModelRot", model.rot, 0.01f);
        glm_vec4_divs(model.rot, glm_vec4_norm(model.rot), model.rot);
      }
    }
    ImGui::End();
  }

  void rotateCameraWithDeltaMouse(vec2 rot) {
    const float ROT_SPEED = 1 / 100.f;
    auto& camera = TempModelRenderer::get().camera;

    vec3 cameraUp = {};
    camera.getUp(cameraUp);
    glm_vec3_scale(cameraUp, ROT_SPEED * rot[1], cameraUp);

    vec3 cameraRight = {};
    camera.getRight(cameraRight);
    glm_vec3_scale(cameraRight, ROT_SPEED * rot[0], cameraRight);

    vec3 cameraForward = {};
    camera.getForward(cameraForward);

    vec3 newOffset = {0.f, 0.f, 0.f};
    glm_vec3_add(newOffset, cameraForward, newOffset);
    glm_vec3_add(newOffset, cameraUp, newOffset);
    glm_vec3_add(newOffset, cameraRight, newOffset);
    glm_vec3_normalize(newOffset);

    versor deltaCameraRot = {};
    glm_quat_from_vecs(cameraForward, newOffset, deltaCameraRot);
    glm_quat_mul(deltaCameraRot, camera.rot, camera.rot);
  }
};

int main(void) {
  App application;

  Application::registerContext(application, TITLE);
  Application::initialize();

  return 0;
}