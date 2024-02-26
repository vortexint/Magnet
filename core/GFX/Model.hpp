#pragma once

#include <optional>
#include <span>
#include <cstdint>
#include <unordered_map>
#include <array>
#include <vector>

#include <cglm/cglm.h>

namespace Magnet {

// Reference: https://github.khronos.org/glTF-Tutorials/gltfTutorial/

struct Model {
  struct Mesh {
    std::vector<unsigned> vbo;
    unsigned ebo, vao;
    int mode;
    size_t count;
    int componentType;
    size_t byteOffset;
  };

  std::vector<Mesh> meshes;



  static std::optional<Model> create(std::span<const uint8_t> mem);

  // TODO: Implement destructor with rule of 5 later
  void destroy();
};

// This a temporary model renderer, once the full rendering system is implemented
// This will be removed
class TempModelRenderer {
  unsigned shader;

 public:
  struct ModelAndTransform {
    Model model;
    vec3 pos = {0.f, 1.5f, -4.f};
    vec3 scale = {1.f, 1.f, 1.f};
    versor rot = {0.f, 0.f, 0.f, 1.f};

    ModelAndTransform(Model);
  };

  float width = 800.f, height = 600.f;

  std::vector<ModelAndTransform> models;

  TempModelRenderer();
  ~TempModelRenderer();
  

  static TempModelRenderer& get() { 
    static TempModelRenderer temp;
    return temp;
  }

  void init();
  void update();
  void draw();
};


}  // namespace Magnet
