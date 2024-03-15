#pragma once

#include <optional>
#include <span>
#include <cstdint>
#include <unordered_map>
#include <array>
#include <vector>
#include <memory>

#include <cglm/cglm.h>

#include <magnet/ArchiveManager.hpp>

namespace Magnet {
// Reference: https://github.khronos.org/glTF-Tutorials/gltfTutorial/
struct DrawElements {
  unsigned ebo;
  int mode;
  size_t count;
  int componentType;
  size_t byteOffset;
};
struct DrawArrays {
  int mode;
  int first;
  size_t count;
};
struct Model {
  struct MeshPrimitive {
    std::vector<unsigned> vbo;
    unsigned vao = 0;
    
    std::optional<DrawElements> drawElements;
    std::optional<DrawArrays> drawArrays;

    vec4 baseColorFactor = { 1.f, 1.f, 1.f, 1.f };
    std::optional<int> baseColorTexture;
    float metallicFactor = 1.f;
    float roughnessFactor = 1.f;
    std::optional<int> metallicRoughnessTexture;
    vec3 emissiveFactor = {0.f, 0.f, 0.f};
    std::optional<int> emissiveTexture;
    std::optional<int> occlussionTexture;
    std::optional<int> normalTexture;

    struct Config {
      int textures = 1; // TEXCOORD_0 ... TEXCOORD_{textures - 1} its inclusive
      int colors = 0;
      bool colorFormatIsVec3ElseVec4 = true;
      int joints = 0;
      int weights = 0;
    } config;
  };
  struct Mesh {
    std::vector<MeshPrimitive> primitives;
  };
  struct Node {
    int meshIndex = -1;
    std::vector<int> childIndexes;

    vec3 pos = {0.f, 0.f, 0.f};
    vec3 scale = {1.f, 1.f, 1.f};
    vec4 rot = {0.f, 0.f, 0.f, 1.f};
    bool hasChanged = false;
  };
  std::vector<int> parentNodeIndices;
  std::unordered_map<int, Mesh> meshes;
  std::unordered_map<int, Node> nodes;
  std::unordered_map <int, unsigned> textures;
  std::unordered_map <int, unsigned> buffers;

  static std::optional<Model> create(std::span<const uint8_t> mem);


  // TODO: Implement a destructor model class
  void destroy();
};

// This a temporary model renderer, once the full rendering system is implemented
// This will be removed

struct TempCamera {
  vec3 pos = {0.f, 0.f, 0.f};
  versor rot = {0.f, 0.f, 0.f, 1.f};
  
  void getForward(vec3);
  void getRight(vec3);
  void getUp(vec3);

  void lookAt(vec3 other);
  void setPos(vec3 pos);
};

class TempModelRenderer {
  ArchiveManager archiveManager;
  unsigned shader;
  unsigned whiteTexture;

  void recursivelyDrawNodes(const mat4 mvp, int nodeIndex, Magnet::Model &model);
 public:
  TempCamera camera;

  struct ModelAndTransform {
    Model model;
    vec3 pos = {0.f, 1.5f, -4.f};
    vec3 scale = {1.f, 1.f, 1.f};
    versor rot = {0.f, 0.f, 0.f, 1.0f};

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
  static unsigned generateWhiteTexture();

  void init();
  void update();
  void draw();
};


}  // namespace Magnet
