#include <magnet/Application.hpp>

#include <../GFX/Model.hpp>

#include "imgui.h"

using namespace Magnet;

const char* const TITLE = "Primitives Example";

class App : public Magnet::Context {
  Model cone, sphere, cube;
public:
  void init() override {
    // ArchiveManager archiveMgr(ARCH_assets);

    ArchiveManager archiveMgr("assets.magnet");

    std::vector<uint8_t> buffer;

    buffer.clear();
    archiveMgr.loadFile("cone.glb", buffer);
    cone = Model::create(buffer).value();
    TempModelRenderer::get().models.push_back(cone);

    buffer.clear();
    archiveMgr.loadFile("sphere.glb", buffer);
    sphere = Model::create(buffer).value();
    TempModelRenderer::get().models.push_back(sphere);

    buffer.clear();
    archiveMgr.loadFile("cube.glb", buffer);
    cube = Model::create(buffer).value();
    TempModelRenderer::get().models.push_back(cube);
  }

  void update() override {
    ImGui::Begin("Primitives");
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
        
      }
    }
    ImGui::End();
  }
};

int main(void) {
  App application;

  Application::registerContext(application, TITLE);
  Application::initialize();

  return 0;
}