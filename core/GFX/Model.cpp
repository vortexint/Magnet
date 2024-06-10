#include <cglm/mat4.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <GFX/Model.hpp>
#include <magnet/Library.hpp>
#include <magnet/Shaders.hpp>
#include <magnet/Geometry.hpp>
#include <memory>
#include <string>

namespace Magnet {
static constexpr int VERTEX_ATTRIB_LOC = 0;
static constexpr int NORMAL_ATTRIB_LOC = 1;
static constexpr int TANGENT_ATTRIB_LOC = 2;
static constexpr int TEX_COORD_0_ATTRIB_LOC = 3;
static constexpr int TEX_COLOR_0_ATTRIB_LOC = 4;
static constexpr int TEX_JOINTS_0_ATTRIB_LOC = 5;
static constexpr int TEX_WEIGHTS_0_ATTRIB_LOC = 6;

// Given a buffer size and a mode return the count of elements there are for
// drawing
int glModeCount(int mode, size_t bufferSize) {
  switch (mode) {
    case GL_POINTS:
      return bufferSize / (sizeof(float) * 3);
      break;
    case GL_LINE_STRIP:
      return (bufferSize / (sizeof(float) * 3)) - 1;
      break;
    case GL_LINE_LOOP:
      return bufferSize / (sizeof(float) * 3);
      break;
    case GL_LINES:
      return bufferSize / (sizeof(float) * 3 * 2);
      break;
    case GL_TRIANGLE_STRIP:
      return (bufferSize / (sizeof(float) * 3)) - 2;
      break;
    case GL_TRIANGLE_FAN:
      return ((bufferSize / (sizeof(float) * 3)) - 1) / 2;
      break;
    case GL_TRIANGLES:
      return bufferSize / (sizeof(float) * 3 * 3);
      break;
    default:
      assert(false);
  }

  return 1;
}
bool validIndex(int index, size_t size) { return (0 <= index && index < size); }

void createMesh(Model& magnetModel, const tinygltf::Model& model,
                const tinygltf::Mesh& mesh, int meshIndex) {
  std::vector<MeshPrimitive> primitives;

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    const tinygltf::Primitive& primitive = mesh.primitives[i];

    std::vector<unsigned> vbos;
    GLuint vao = 0;

    // the model might not have indices so we have to consider that case
    if (primitive.indices != -1 &&
        !validIndex(primitive.indices, model.accessors.size())) {
      spdlog::error("primitive.indices is out of bounds");
      continue;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::optional<DrawArrays> drawArrays;
    std::optional<DrawElements> drawElements;

    if (primitive.indices != -1) {
      GLuint ebo = 0;
      const tinygltf::Accessor& indexAccessor =
        model.accessors[primitive.indices];
      const tinygltf::BufferView& indexBufferView =
        model.bufferViews[indexAccessor.bufferView];

      const tinygltf::Buffer& indexBuffer =
        model.buffers[indexBufferView.buffer];

      assert(indexBufferView.target == GL_ELEMENT_ARRAY_BUFFER);
      assert(indexBufferView.byteOffset + indexBufferView.byteLength <=
             indexBuffer.data.size());

      glGenBuffers(1, &ebo);
      glBindBuffer(indexBufferView.target, ebo);
      glBufferData(indexBufferView.target, indexBufferView.byteLength,
                   &indexBuffer.data[indexBufferView.byteOffset],
                   GL_STATIC_DRAW);

      drawElements =
        DrawElements{ebo, primitive.mode, indexAccessor.count,
                     indexAccessor.componentType, indexAccessor.byteOffset};
    } else if (primitive.attributes.size() > 0 &&
               primitive.attributes.contains("POSITION") &&
               validIndex(primitive.attributes.at("POSITION"),
                          model.accessors.size())) {
      // Case case where there is not EBO
      assert(primitive.attributes.size() > 0);

      const tinygltf::Accessor& accessor =
        model.accessors[primitive.attributes.at("POSITION")];
      drawArrays = DrawArrays{
        primitive.mode,
        glModeCount(primitive.mode, accessor.byteOffset),
        accessor.count,
      };
    }
    // All primitive attributes wiil never be element array buffers
    for (auto& [attribName, attribAccessorIndex] : primitive.attributes) {
      const tinygltf::Accessor& accessor = model.accessors[attribAccessorIndex];

      const tinygltf::BufferView& bufferView =
        model.bufferViews[accessor.bufferView];
      const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

      if (bufferView.target != GL_ARRAY_BUFFER) {
        spdlog::warn("Unkown target, skipping mesh");
        continue;
      }

      if (!validIndex(bufferView.byteOffset, buffer.data.size())) {
        spdlog::error(
          "Invalid bufferView.byteOffset {} when the buffer length is {}",
          bufferView.byteOffset, buffer.data.size());
        continue;
      }
      if (bufferView.byteOffset + bufferView.byteLength > buffer.data.size()) {
        spdlog::error(
          "bufferView out of bounds byteOffset {} + byteLength {}, actual "
          "buffer size {}",
          bufferView.byteOffset, bufferView.byteLength, buffer.data.size());
        continue;
      }

      GLuint vbo = 0;
      glGenBuffers(1, &vbo);
      glBindBuffer(bufferView.target, vbo);
      glBufferData(bufferView.target, bufferView.byteLength,
                   &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);

      vbos.push_back(vbo);

      int byteStride = accessor.ByteStride(bufferView);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      auto matchesPrefix = [](const std::string& view,
                              const std::string& prefix, int& index) {
        if (prefix.size() > view.size()) {
          return false;
        }

        if (!view.starts_with(prefix)) {
          return false;
        }

        if (prefix.size() == view.size()) {
          return false;
        }

        int totalIndex = 0;
        for (size_t i = prefix.size(); i < view.size(); ++i) {
          totalIndex *= 10;
          if ('0' <= view[i] && view[i] <= '9') {
            totalIndex += (view[i] - '0');
          } else {
            spdlog::error("Invalid integer on {}", view);
            return false;
          }
        }

        index = totalIndex;
        return true;
      };

      bool colorFormatIsVec3ElseVec4;

      int vaa = -1;
      int index = -1;
      if (attribName == "POSITION") {
        vaa = VERTEX_ATTRIB_LOC;
      } else if (attribName == "NORMAL") {
        vaa = NORMAL_ATTRIB_LOC;
      } else if (attribName == "TANGENT") {
        vaa = TANGENT_ATTRIB_LOC;
      } else if (matchesPrefix(attribName, "TEXCOORD_", index)) {
        if (index == 0) {
          vaa = TEX_COORD_0_ATTRIB_LOC;
        } else {
          spdlog::warn(
            "Color index {} greater than 0, only COLOR_0 is currently "
            "implemented",
            index);
        }
      } else if (matchesPrefix(attribName, "COLOR_", index)) {
        colorFormatIsVec3ElseVec4 = (byteStride == sizeof(GLfloat) * 3);

        // COLOR_n can be either vec3 or vec4 we need two shaders to handle this
        // case
        if (index == 0) {
          vaa = TEX_COLOR_0_ATTRIB_LOC;

        } else {
          spdlog::warn(
            "Color index {} greater than 0, only COLOR_0 is currently "
            "implemented",
            index);
        }
      } else if (matchesPrefix(attribName, "JOINTS_", index)) {
        if (index == 0) {
          vaa = TEX_JOINTS_0_ATTRIB_LOC;
        } else {
          spdlog::warn(
            "Joint index {} greater than 0, only JOINTS_0 is currently "
            "implemented",
            index);
        }
      } else if (matchesPrefix(attribName, "WEIGHTS_", index)) {
        if (index == 0) {
          vaa = TEX_WEIGHTS_0_ATTRIB_LOC;
        } else {
          spdlog::warn(
            "Weight index {} greater than 0, only Weights_0 is currently "
            "implemented",
            index);
        }
      }
      if (vaa > -1) {
        GL_BYTE;
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, (void*)(accessor.byteOffset));
      } else {
        spdlog::warn("Unkown attribute {}", attribName);
      }
    }

    MeshPrimitive meshPrimitive{std::move(vbos),      vao,
                                       drawElements,         drawArrays,
                                       {1.f, 1.f, 1.f, 1.f}, std::nullopt};

    if (0 <= primitive.material &&
        primitive.material < model.materials.size()) {
      auto& material = model.materials[primitive.material];

      auto& baseColorFactor = material.pbrMetallicRoughness.baseColorFactor;

      if (baseColorFactor.size() == 4) {
        meshPrimitive.baseColorFactor[0] = baseColorFactor[0];
        meshPrimitive.baseColorFactor[1] = baseColorFactor[1];
        meshPrimitive.baseColorFactor[2] = baseColorFactor[2];
        meshPrimitive.baseColorFactor[3] = baseColorFactor[3];
      }

      auto& baseColorTexture = material.pbrMetallicRoughness.baseColorTexture;
      if (0 <= baseColorTexture.index &&
          baseColorTexture.index < model.textures.size()) {
        meshPrimitive.baseColorTexture =
          magnetModel.textures.at(baseColorTexture.index);
      }

      auto& metallicRoughnessTexture =
        material.pbrMetallicRoughness.metallicRoughnessTexture;
      if (0 <= metallicRoughnessTexture.index &&
          metallicRoughnessTexture.index < model.textures.size()) {
        meshPrimitive.metallicRoughnessTexture =
          magnetModel.textures.at(metallicRoughnessTexture.index);
      }

      if (material.emissiveFactor.size() == 3) {
        meshPrimitive.emissiveFactor[0] = material.emissiveFactor[0];
        meshPrimitive.emissiveFactor[1] = material.emissiveFactor[1];
        meshPrimitive.emissiveFactor[2] = material.emissiveFactor[2];
      }
      auto& emissiveTexture = material.emissiveTexture;
      if (0 <= emissiveTexture.index &&
          emissiveTexture.index < model.textures.size()) {
        meshPrimitive.emissiveTexture =
          magnetModel.textures.at(emissiveTexture.index);
      }

      auto& occlussionTexture = material.occlusionTexture;
      if (0 <= occlussionTexture.index &&
          occlussionTexture.index < model.textures.size()) {
        meshPrimitive.occlussionTexture =
          magnetModel.textures.at(occlussionTexture.index);
      }

      auto& normalTexture = material.normalTexture;
      if (0 <= normalTexture.index &&
          normalTexture.index < model.textures.size()) {
        meshPrimitive.normalTexture =
          magnetModel.textures.at(normalTexture.index);
      }
    }

    primitives.push_back(meshPrimitive);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  magnetModel.meshes.insert({meshIndex, Mesh{primitives}});
}

void traverseNodes(Model& magnetModel, tinygltf::Model& model,
                   tinygltf::Node& node, int nodeIndex, std::vector<bool> &hasTraversedNode) {

  if (hasTraversedNode[nodeIndex]) {
    return;
  } else {
    hasTraversedNode[nodeIndex] = true;
  }

  if (0 <= node.mesh && node.mesh < model.meshes.size()) {
    createMesh(magnetModel, model, model.meshes[node.mesh], node.mesh);
  } else if (node.mesh != -1) {  // tinygltf uses -1 a flag for none
    spdlog::error("Invalid mesh index {}", node.mesh);
  }


  for (auto childNodeIndex : node.children) {
    if (0 <= childNodeIndex && childNodeIndex < model.nodes.size()) {
      traverseNodes(magnetModel, model, model.nodes[childNodeIndex],
                    childNodeIndex, hasTraversedNode);
    } else {
      spdlog::error("Model Error: Invalid node index {}", childNodeIndex);
    }
  }

  ModelNode magnetNode{node.mesh, node.children};
  if (node.matrix.size() == 16) {
    // node.matrix column major order
    mat4 mat = {
      {node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3]},
      {node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7]},
      {node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11]},
      {node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]}};

    vec4 translation;  // This has to be vec4 for glm_decompose
    mat4 rotMatrix;
    vec3 scale;
    glm_decompose(mat, translation, rotMatrix, scale);

    versor rotation = {};
    glm_mat4_quat(rotMatrix, rotation);

    glm_vec3_copy(translation, magnetNode.trs.translation);
    glm_vec3_copy(scale, magnetNode.trs.scale);
    glm_vec4_copy(rotation, magnetNode.trs.scale);

  } else {
    // TODO: Assert this if (node.translation.size() == 3 && node.scale.size()
    // == 3 && node.rotation.size() == 4)
    if (node.translation.size() == 3) {
      vec3 translation = {node.translation[0], node.translation[1],
                          node.translation[2]};

      glm_vec3_copy(translation, magnetNode.trs.translation);
    }
    if (node.rotation.size() == 4) {
      versor rotation = {node.rotation[0], node.rotation[1], node.rotation[2],
                         node.rotation[3]};
      glm_vec4_copy(rotation, magnetNode.trs.rotation);
    }
    if (node.scale.size() == 3) {
      vec3 scale = {node.scale[0], node.scale[1], node.scale[2]};
      glm_vec3_copy(scale, magnetNode.trs.scale);
    }
    if (0 <= node.mesh && node.mesh < model.meshes.size()) {
      magnetNode.meshIndex = node.mesh;
    } else {
      magnetNode.meshIndex = -1;
    }
  }

  magnetModel.nodes.insert({nodeIndex, magnetNode});
}

struct TextureOptions {
  GLenum minFilter = GL_LINEAR, magFilter = GL_LINEAR;
  GLenum wrapS = GL_REPEAT, wrapP = GL_REPEAT;
};
// TODO: Go through all model.xxx[index] and add a check to make sure that index
// is valid
void loadTextures(Model& magnetModel, const tinygltf::Model& model) {
  for (size_t i = 0; i < model.textures.size(); ++i) {
    const auto& texture = model.textures[i];
    const auto& image = model.images[texture.source];

    if (image.uri != "") {
      spdlog::error(
        "Currently not supporting textures using uris. The image {} will not "
        "be loaded",
        image.uri);
      continue;
    } else {
      assert(0 <= image.bufferView &&
             image.bufferView < model.bufferViews.size());
      if (!validIndex(image.bufferView, model.bufferViews.size())) {
        spdlog::error("image.bufferView out of bounds {} size:",
                      image.bufferView, model.bufferViews.size());
        continue;
      }

      auto& bufferView = model.bufferViews[image.bufferView];
      auto& mimetype = image.mimeType;
      auto& buffer = model.buffers[bufferView.buffer];

      if (mimetype == "image/png" || mimetype == "image/jpeg") {
        GLenum format = GL_RGBA;

        if (image.component == 1) {
          format = GL_RED;
        } else if (image.component == 2) {
          format = GL_RG;
        } else if (image.component == 3) {
          format = GL_RGB;
        } else if (image.component == 4) {
          format = GL_RGBA;
        } else {
          spdlog::error("Invalid image format {}", image.component);
          continue;
        }

        GLenum type = GL_UNSIGNED_BYTE;
        if (image.bits == 8) {
          // ok
        } else if (image.bits == 16) {
          type = GL_UNSIGNED_SHORT;
        } else {
          spdlog::error("Invalid image bits {}", image.bits);
          continue;
        }

        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        int minFilter = GL_LINEAR, magFilter = GL_LINEAR, wrapS = GL_REPEAT,
            wrapT = GL_REPEAT;
        if (texture.sampler != -1 &&
            validIndex(texture.sampler, model.samplers.size())) {
          const auto& sampler = model.samplers[texture.sampler];

          if (sampler.minFilter != -1) {
            minFilter = sampler.minFilter;
          }
          if (sampler.magFilter != -1) {
            magFilter = sampler.magFilter;
          }
          if (sampler.wrapS != -1) {
            wrapS = sampler.wrapS;
          }
          if (sampler.wrapT != -1) {
            wrapT = sampler.wrapT;
          }
        }

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                     format, type, &image.image.at(0));
        // don't remove, needed because some gltf minFilter's require having
        // mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        magnetModel.textures.insert({i, tex});

        glBindTexture(GL_TEXTURE_2D, 0);
      } else {
        spdlog::error("Unsupported mimetype '{}'. Texture will not be loaded.",
                      mimetype);
      }
    }
  }
}

void loadAnimations(Model& magnetModel, const tinygltf::Model& model) {

  int i = 0;
  for (auto& animation : model.animations) {
    ModelAnimation magnetAnimation = {};
    magnetAnimation.name = animation.name;

    magnetAnimation.samplers.reserve(animation.samplers.size());
    for (auto& sampler : animation.samplers) {
      ModelAnimation::Sampler magnetSampler = {};

      magnetSampler.input = sampler.input;
      if (sampler.interpolation == "LINEAR") {
        magnetSampler.interpolation = ModelAnimation::LINEAR;
      } else if (sampler.interpolation == "STEP") {
        magnetSampler.interpolation = ModelAnimation::STEP;
      } else if (sampler.interpolation == "CUBICSPLINE") {
        magnetSampler.interpolation = ModelAnimation::CUBICSPLINE;
      } else {
        spdlog::error(
          "Invalid sampler interpolation type '{}'. Using LINEAR instead",
          sampler.interpolation);

        magnetSampler.interpolation = ModelAnimation::LINEAR;
      } 

      magnetAnimation.samplers.push_back(magnetSampler);
    }

    magnetAnimation.channels.reserve(animation.channels.size());
    for (auto& channel : animation.channels) {
      ModelAnimation::Channel magnetChannel = {};

      magnetChannel.sampler = channel.sampler;

      magnetChannel.target.node = channel.target_node;
      if (0 > channel.target_node ||
          channel.target_node >= model.nodes.size()) {
        spdlog::error(
          "Invalid target node {}. This may be due to a unsupported extension "
          "being used. Ignoring channel",
          channel.target_node);
        continue;
      }
      if (channel.target_path == "translation") {
        magnetChannel.target.path = ModelAnimation::ChannelPath::TRANSLATION;
      } else if (channel.target_path == "rotation") {
        magnetChannel.target.path = ModelAnimation::ChannelPath::ROTATION;
      } else if (channel.target_path == "scale") {
        magnetChannel.target.path = ModelAnimation::ChannelPath::SCALE;
      } else if (channel.target_path == "weights") {
        magnetChannel.target.path = ModelAnimation::ChannelPath::WEIGHTS;
      } else {
        spdlog::error("Invalid channel target path. Skipping channel");
      }

      magnetAnimation.channels.push_back(magnetChannel);
    }

    magnetModel.animations[i] = magnetAnimation;
    ++i;
  }
}
std::optional<Model> Model::create(std::span<const uint8_t> bytes) {
  tinygltf::TinyGLTF loader;

  std::string err;
  std::string warn;

  tinygltf::Model model;

  if (!loader.LoadBinaryFromMemory(&model, &err, &warn, bytes.data(),
                                   bytes.size())) {
    if (!warn.empty()) {
      spdlog::warn("{}", warn);
    }
    if (!err.empty()) {
      spdlog::error("{}", err);
    }
    spdlog::error(
      "Tinygltf could not load binary file. File should end with glb.");
    return std::nullopt;
  }

  if (!warn.empty()) {
    spdlog::warn("{}", warn);
  }
  if (!err.empty()) {
    spdlog::error("{}", err);
  }

  Magnet::Model magnetModel;

  loadTextures(magnetModel, model);
  loadAnimations(magnetModel, model);

  if (model.scenes.size() == 0) {
    spdlog::warn("Loaded model has no scenes");
  }

  std::vector<int> scenes;
  if (0 <= model.defaultScene && model.defaultScene < model.scenes.size()) {
    scenes.push_back(model.defaultScene);
  } else {
    // Some gltf files don't specify a default scene
    // in that case we render them all
    for (int i = 0; i < model.scenes.size(); ++i) {
      scenes.push_back(i);
    }
  }

  std::vector<bool> hasTraversedNode;
  hasTraversedNode.resize(model.nodes.size(), false);


  for (int nodeIndex = 0; nodeIndex < model.nodes.size(); ++nodeIndex) {
    traverseNodes(magnetModel, model, model.nodes[nodeIndex], nodeIndex,
                  hasTraversedNode);
    magnetModel.parentNodeIndices.push_back(nodeIndex);
  }

  return magnetModel;
}

void Model::destroy() {
  for (auto& [meshIndex, mesh] : meshes) {
    for (auto& meshPrimitive : mesh.primitives) {
      for (auto vbo : meshPrimitive.vbo) {
        glDeleteBuffers(1, &vbo);
      }
      if (meshPrimitive.drawElements) {
        glDeleteBuffers(1, &meshPrimitive.drawElements->ebo);
      }

      glDeleteVertexArrays(1, &meshPrimitive.vao);
    }
  }

  for (auto& [texIndex, tex] : textures) {
    glDeleteTextures(1, &tex);
  }
}


ModelAnimationState::ModelAnimationState(const Model& magnetModel) {
  for (auto& [nodeIndex, node] : magnetModel.nodes) {
    this->states[nodeIndex] = {};
    for (const auto& childNodexIndex : node.childIndexes) {
      this->states[childNodexIndex].parent = nodeIndex;
    }
  }
}

TempModelRenderer::ModelData::ModelData(Model model) :
  animation(model) {
  this->model = model;
}

TempModelRenderer::TempModelRenderer()
  : archiveManager(ARCH_core, ARCH_core_KEY) {
  this->init();
  if (this->shader == 0) {
    spdlog::error("Failed to load shader");
    assert(false);
  }

  whiteTexture = generateWhiteTexture();
}

TempModelRenderer::~TempModelRenderer() {
  for (auto& model : models) {
    model.model.destroy();
  }
  glDeleteTextures(1, &whiteTexture);
}
unsigned TempModelRenderer::generateWhiteTexture() {
  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLubyte texData[] = {255, 255, 255, 255};

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               texData);
  // don't remove, needed because some gltf minFilter's require having
  // mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  return tex;
}

void TempModelRenderer::init() {
  std::string vertexShaderCode, fragmentShaderCode;
  this->archiveManager.loadFile("shaders/temp.vert", vertexShaderCode);
  this->archiveManager.loadFile("shaders/temp.frag", fragmentShaderCode);

  shader = Shaders::generate(vertexShaderCode, fragmentShaderCode);
}
void TempModelRenderer::update() {}
void TempCamera::getForward(vec3 newForward) {
  vec3 forward = {};
  Magnet::GET_FORWARD(forward);
  glm_quat_rotatev(trs.rotation, forward, newForward);
}
void TempCamera::getRight(vec3 newRight) {
  vec3 right = {};
  Magnet::GET_RIGHT(right);
  glm_quat_rotatev(trs.rotation, right, newRight);
}
void TempCamera::getUp(vec3 newUp) {
  vec3 up = {};
  Magnet::GET_UP(up);
  glm_quat_rotatev(trs.rotation, up, newUp);
}

void TempCamera::lookAt(vec3 target) {
  vec3 up = {};
  Magnet::GET_UP(up);
  glm_quat_forp(trs.translation, target, up, trs.rotation);
}
void TempCamera::setPos(vec3 pos) { glm_vec3_copy(pos, pos); }

void TempModelRenderer::calculateAndSetJointTransforms(
  std::span<GlmObj<mat4>> jointTransforms,
  const Model& model,
  const ModelAnimationState& animationState) {

  mat4 transform = {};
  glm_mat4_identity(transform);

  size_t i = 0;

  for (auto nodeIndex : model.parentNodeIndices) {
    calculateAndSetJointTransforms(jointTransforms, model, animationState, transform, nodeIndex);
  }
}
void TempModelRenderer::calculateAndSetJointTransforms(
  std::span<GlmObj<mat4>> jointTransforms, const Model& model,
  const ModelAnimationState& animationState, mat4 transform, int nodeIndex) {

  // The jointTransforms is a matrix of transforms corresponding 
  // to the joint id which is the same as the node id
  assert(jointTransforms.size() >= model.nodes.size());
  assert(0 <= nodeIndex && nodeIndex < model.nodes.size());
  assert(nodeIndex < jointTransforms.size());

  const auto& state = animationState.states.at(nodeIndex);

  mat4 localTransform = {};
  state.trs.toMat4(localTransform);

  mat4 parentTransform = {};
  if (0 <= state.parent && state.parent < jointTransforms.size()) {
    glm_mat4_copy(*jointTransforms[state.parent], parentTransform);
  } else {
    if (state.parent != -1) {
      spdlog::error(
        "state.parent({}) is out of bounds and is not -1. This indicates that "
        "there may be a bug where incorrent indices have been introduced", state.parent);
    }

    glm_mat4_identity(parentTransform);
  }

  // TODO:
  // - [ ] CREATE TEST CASES FOR VERY SIMPLE ANIMATION STATES
  // AND VERY SIMPLE MODEL AND PRINT OUT THE MATRICES EMITTED FROM IT
  // COMPARE THAT TO THE MATRICES THAT ARE OUTPUTED BY THIS ALGORITHM
  //
  // - [ ] WRITE SOME CODE THAT TAKES THE BASE64 BINARY OF JOINTS_N 
  // AND PRINT IT OUT TO A FILE AND INSPECT THE HEX VALUES

  // The order of multiplication matters it is always Parent * local
  glm_mat4_mul(parentTransform, localTransform, *jointTransforms[nodeIndex]);

  for (auto& nodeChildIndex: model.nodes.at(nodeIndex).childIndexes) {
    calculateAndSetJointTransforms(jointTransforms, model, animationState,
                                   parentTransform, nodeChildIndex);
  }
}
void TempModelRenderer::draw() {
  glUseProgram(shader);

  for (auto& modelAndTransform : models) {
    mat4 model = {};
    modelAndTransform.trs.toMat4(model);

    mat4 view = {};
    {
      vec3 up = {};
      camera.getUp(up);

      vec3 forward = {};
      camera.getForward(forward);

      glm_look(camera.trs.translation, forward, up, view);
    }

    mat4 projection = {};
    glm_perspective(glm_rad(45.0), width / height, 0.1f, 500.f, projection);

    mat4 mvp = {};
    glm_mat4_identity(mvp);
    glm_mat4_mul(model, mvp, mvp);
    glm_mat4_mul(view, mvp, mvp);
    glm_mat4_mul(projection, mvp, mvp);

    
    std::vector<GlmObj<mat4>> jointTransforms;
    jointTransforms.resize(modelAndTransform.model.nodes.size());
    for (size_t i = 0; i < jointTransforms.size(); ++i) {
      glm_mat4_identity(*jointTransforms[i]);
    }

    calculateAndSetJointTransforms(jointTransforms, modelAndTransform.model,
                                   modelAndTransform.animation);

    for (size_t i = 0; i < jointTransforms.size(); ++i) {
      std::string uniformName =
        "globalBoneTransforms[" + std::to_string(i) + "]";
      Shaders::setMat4(shader, uniformName.c_str(), *(jointTransforms[i]));
    }

    for (auto& model : models) {
      for (auto nodeIndex : model.model.parentNodeIndices) {
        recursivelyDrawNodes(mvp, nodeIndex, model.model);
      }
    }
    glUseProgram(0);
  }
}
void TempModelRenderer::recursivelyDrawNodes(const mat4 parentMVP,
                                             int nodeIndex,
                                             Magnet::Model& model) {
  if (!model.nodes.contains(nodeIndex)) {
    return;
  }

  auto& node = model.nodes.at(nodeIndex);

  mat4 modelMat = {};
  node.trs.toMat4(modelMat);

  mat4 mvp = {};
  {
    mat4 parentMVPCopy = {};
    memcpy(parentMVPCopy, parentMVP, sizeof(mat4));

    glm_mat4_mul(parentMVPCopy, modelMat, mvp);
  }

  Shaders::setMat4(shader, "MVP", mvp);
  bool oldColorFormatBool = true;
  Shaders::setBool(shader, "isColorVec3ElseVec4", oldColorFormatBool);

  if (model.meshes.contains(node.meshIndex)) {
    auto& mesh = model.meshes.at(node.meshIndex);
    for (auto& meshPrimitive : mesh.primitives) {
      if (meshPrimitive.colorFormatIsVec3ElseVec4 != oldColorFormatBool) {
        Shaders::setBool(shader, "isColorVec3ElseVec4",
                         meshPrimitive.colorFormatIsVec3ElseVec4);
        oldColorFormatBool = meshPrimitive.colorFormatIsVec3ElseVec4;
      }

      Shaders::setVec4(this->shader, "baseColorFactor",
                       meshPrimitive.baseColorFactor);

      Shaders::setFloat(shader, "metalicFactor", meshPrimitive.metallicFactor);

      Shaders::setFloat(shader, "roughnessFactor",
                        meshPrimitive.roughnessFactor);

      Shaders::setVec3(shader, "emissiveFactor", meshPrimitive.emissiveFactor);

      glBindVertexArray(meshPrimitive.vao);

      if (meshPrimitive.baseColorTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *meshPrimitive.baseColorTexture);
      } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->whiteTexture);
      }

      if (meshPrimitive.metallicRoughnessTexture) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, *meshPrimitive.metallicRoughnessTexture);
      }

      if (meshPrimitive.emissiveTexture) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, *meshPrimitive.emissiveTexture);
      }

      if (meshPrimitive.occlussionTexture) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, *meshPrimitive.occlussionTexture);
      }

      if (meshPrimitive.normalTexture) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, *meshPrimitive.normalTexture);
      }

      if (meshPrimitive.drawElements) {
        glDrawElements(meshPrimitive.drawElements->mode,
                       meshPrimitive.drawElements->count,
                       meshPrimitive.drawElements->componentType,
                       (void*)meshPrimitive.drawElements->byteOffset);
      }
      if (meshPrimitive.drawArrays) {
        glDrawArrays(meshPrimitive.drawArrays->mode,
                     meshPrimitive.drawArrays->first,
                     meshPrimitive.drawArrays->count);
      }
    }
  }


  for (auto nodeChildIndex : node.childIndexes) {
    recursivelyDrawNodes(mvp, nodeChildIndex, model);
  }
}

}  // namespace Magnet