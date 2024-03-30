#pragma once

#include <cglm/cglm.h>

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

#include <magnet/ArchiveManager.hpp>
#include <magnet/Geometry.hpp>


// Usefull tools:
// Use glTF: Import from GLB command to convert a glb file to gltf for debugging
// https://marketplace.visualstudio.com/items?itemName=cesium.gltf-vscode <--
// MAKE SURE TO USE THIS https://github.khronos.org/glTF-Validator/
// https://github.com/KhronosGroup/glTF-Sample-Assets
// https://gltf-viewer.donmccurdy.com/
// https://sandbox.babylonjs.com/
// https://lisyarus.github.io/blog/graphics/2023/07/03/gltf-animation.html
//
// TODO: Implement more of PBR data
// https://gltf-viewer-tutorial.gitlab.io/physically-based-materials/ PBR
// Implementation https://github.com/Nadrin/PBR/tree/master

/*
Make sure to always test OrientationTest.glb whenever you make modifications to
this file

Manually tested 90 sample models from
https://github.com/KhronosGroup/glTF-Sample-Assets
Models that are still not visible 5
 - [ ] Fix AnimatedMorphCube.glb
 - [ ] AnisotropyDiscTest.glb
 - [ ] BoxAnimated.glb
 - [ ] BoxInterleaved.glb
are oriented
 - [ ] UnlitTest.glb
*/


namespace Magnet {
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

struct MeshPrimitive {
  std::vector<unsigned> vbo;
  unsigned vao = 0;

  std::optional<DrawElements> drawElements;
  std::optional<DrawArrays> drawArrays;

  vec4 baseColorFactor = {1.f, 1.f, 1.f, 1.f};
  std::optional<int> baseColorTexture;

  float metallicFactor = 1.f;
  float roughnessFactor = 1.f;
  std::optional<int> metallicRoughnessTexture;

  vec3 emissiveFactor = {0.f, 0.f, 0.f};
  std::optional<int> emissiveTexture;
  std::optional<int> occlussionTexture;
  std::optional<int> normalTexture;

  bool colorFormatIsVec3ElseVec4 = true;
};
struct Mesh {
  std::vector<MeshPrimitive> primitives;
};
struct ModelNode {
  int meshIndex = -1;
  std::vector<int> childIndexes;

  TRS trs = {};
  bool hasChanged = false;
};
struct ModelAnimation {
  enum SamplerIntepolation { LINEAR, STEP, CUBICSPLINE };
  enum ChannelPath { TRANSLATION, ROTATION, SCALE, WEIGHTS };
  struct Sampler {
    int input;
    SamplerIntepolation interpolation;
    int output;
  };
  struct Target {
    int node;
    ChannelPath path;
  };
  struct Channel {
    int sampler;
    Target target;
  };

  std::string name;
  std::vector<Sampler> samplers;
  std::vector<Channel> channels;
};

struct Model {
  std::vector<int> parentNodeIndices;
  std::unordered_map<int, Mesh> meshes;
  std::unordered_map<int, ModelNode> nodes;
  std::unordered_map<int, unsigned> textures;
  std::unordered_map<int, unsigned> buffers;
  std::unordered_map<int, ModelAnimation> animations;

  static std::optional<Model> create(std::span<const uint8_t> mem);

  // TODO: Implement a destructor model class
  void destroy();
};

struct AnimationState {
  TRS trs;
};
struct ModelAnimationState {
  // Maps a nodeIndex to it's animation state
  std::unordered_map<int, AnimationState> states;
};

// This a temporary model renderer, once the full rendering system is
// implemented This will be removed

struct TempCamera {
  TRS trs;

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

  void recursivelyDrawNodes(const mat4 mvp, int nodeIndex,
                            Magnet::Model& model);

 public:
  TempCamera camera;

  struct ModelAndTransform {
    Model model;
    TRS trs;

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
