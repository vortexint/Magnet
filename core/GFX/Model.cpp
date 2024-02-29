#include <string>
#include <memory>

#include <glad/glad.h>
#include <GFX/Model.hpp>
#include <magnet/Shaders.hpp>

#include <spdlog/spdlog.h>
#include <cglm/mat4.h>

// Usefull tools: 
// https://github.khronos.org/glTF-Validator/
// https://github.com/KhronosGroup/glTF-Sample-Assets
// https://gltf-viewer.donmccurdy.com/
// https://sandbox.babylonjs.com/
// 
// TODO: Implement more of PBR data https://gltf-viewer-tutorial.gitlab.io/physically-based-materials/
// PBR Implementation https://github.com/Nadrin/PBR/tree/master

// TODO: There are two TINYGLTF_IMPLEMENTATION's in this
// and Loaders.cpp, we should unify them in some way

// Don't #define STBI_ONLY_PNG because some models use jpeg images inside of them
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

namespace Magnet {

static constexpr int VERTEX_ATTRIB_LOC = 0;
static constexpr int NORMAL_ATTRIB_LOC = 1;
static constexpr int TEX_COORD_0_ATTRIB_LOC = 2;

bool validIndex(int index, size_t size) {
  return (0 <= index && index < size);
}

void createMesh(Model &magnetModel, tinygltf::Model& model,
                tinygltf::Mesh& mesh, int meshIndex) {
  
  std::vector<Model::MeshPrimitive> primitives;

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive& primitive = mesh.primitives[i];
    
    std::vector<unsigned> vbos;
    GLuint vao = 0;
    GLuint ebo = 0;

    if (primitive.indices != -1 && !validIndex(primitive.indices, model.accessors.size())) {
      spdlog::error("primitive.indices is out of bounds");
      continue;
    }
    

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    
    tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
    tinygltf::BufferView& indexBufferView =
      model.bufferViews[indexAccessor.bufferView];

    tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

    assert(indexBufferView.target == GL_ELEMENT_ARRAY_BUFFER);
    assert(indexBufferView.byteOffset + indexBufferView.byteLength <=
           indexBuffer.data.size());


    glGenBuffers(1, &ebo);
    glBindBuffer(indexBufferView.target, ebo);
    glBufferData(indexBufferView.target, indexBufferView.byteLength,
                  &indexBuffer.data[indexBufferView.byteOffset],
                  GL_STATIC_DRAW);


    // All primitive attributes wiil never be element array buffers
    for (auto& [attribName, attribAccessorIndex] : primitive.attributes) {
      // TODO: Implement the other attributes
      // The gltf spec lists alot TEXCOORD_n COLOR_n JOINTS_n


      tinygltf::Accessor& accessor = model.accessors[attribAccessorIndex];

      tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
      tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

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

      int byteStride =
        accessor.ByteStride(bufferView);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      auto beginsWith = [](const std::string& view, const std::string& prefix) {
        if (prefix.size() > view.size()) {
          return false;
        }

        for (size_t i = 0; i < prefix.size(); ++i) {
          if (view[i] != prefix[i]) {
            return false;
          }
        }

        return true;
      };
      
      int vaa = -1;
      if (attribName == "POSITION") {
        vaa = VERTEX_ATTRIB_LOC;
      } else if (attribName == "NORMAL") {
        vaa = NORMAL_ATTRIB_LOC;
      } else if (beginsWith(attribName, "TEXCOORD_")) {
        vaa = TEX_COORD_0_ATTRIB_LOC;
      }
      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, (void*)(accessor.byteOffset));
      } else {
        spdlog::error("Unkown attribute");
      }
    }

    Model::MeshPrimitive meshPrimitive{std::move(vbos),
                     ebo,
                     vao,
                     primitive.mode,
                     indexAccessor.count,
                     indexAccessor.componentType,
                     indexAccessor.byteOffset,
                     {1.f, 1.f, 1.f, 1.f},
                     std::nullopt};

    if (0 <= primitive.material &&
        primitive.material < model.materials.size()) {
      auto& material = model.materials[primitive.material];

      auto& baseColorFactor = material.pbrMetallicRoughness.baseColorFactor;

      assert(baseColorFactor.size() == 4);
      
      meshPrimitive.baseColorFactor[0] = baseColorFactor[0];
      meshPrimitive.baseColorFactor[1] = baseColorFactor[1];
      meshPrimitive.baseColorFactor[2] = baseColorFactor[2];
      meshPrimitive.baseColorFactor[3] = baseColorFactor[3];
    
      auto& baseColorTexture = material.pbrMetallicRoughness.baseColorTexture;
      if (0 <= baseColorTexture.index && baseColorTexture.index < model.textures.size()) {
        meshPrimitive.baseColorTexture =
          magnetModel.textures.at(baseColorTexture.index);
      }
    }
   

    primitives.push_back(meshPrimitive);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  magnetModel.meshes.insert({meshIndex, Model::Mesh{primitives}});
}

void traverseNodes(Model& magnetModel, tinygltf::Model& model,
                   tinygltf::Node& node, int nodeIndex) {
  if (0 <= node.mesh && node.mesh < model.meshes.size()) {
    createMesh(magnetModel, model, model.meshes[node.mesh], node.mesh);
  } else if (node.mesh != -1) { // tinygltf uses -1 a flag for none
    spdlog::error("Invalid mesh index {}", node.mesh);
  }

  for (auto childNodeIndex : node.children) {
    if (0 <= childNodeIndex && childNodeIndex < model.nodes.size()) {
      traverseNodes(magnetModel, model, model.nodes[childNodeIndex],
                    childNodeIndex);
    } else {
      spdlog::error("Model Error: Invalid node index {}", childNodeIndex);
    }              
  }

  Model::Node magnetNode{ node.mesh, node.children };
  if (node.matrix.size() == 16) {
    // node.matrix column major order
    mat4 mat = {
      {node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3]},
      {node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7]},
      {node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11]},
      {node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]}
    };

    // TODO: Create a function that maps mat4 -> Transform and back

    vec4 translation; // This has to be vec4 for glm_decompose
    mat4 rotMatrix;
    vec3 scale;
    glm_decompose(mat, translation, rotMatrix, scale);

    versor rotation = {};
    glm_mat4_quat(rotMatrix, rotation);

    glm_vec3_copy(translation, magnetNode.pos);
    glm_vec3_copy(scale, magnetNode.scale);
    glm_vec4_copy(rotation, magnetNode.rot);
    
  } else if (node.translation.size() == 3 && node.scale.size() == 3 && node.rotation.size() == 4) {
    vec3 translation = {node.translation[0], node.translation[1],
                        node.translation[2]};
    versor rotation = {node.rotation[0], node.rotation[1], node.rotation[2],
                     node.rotation[3]};
    vec3 scale = {node.scale[0], node.scale[1], node.scale[2]};

    glm_vec3_copy(translation, magnetNode.pos);
    glm_vec3_copy(scale, magnetNode.scale);
    glm_vec4_copy(rotation, magnetNode.rot);
  }

  magnetModel.nodes.insert({nodeIndex, magnetNode});
}

struct TextureOptions {
  GLenum minFilter = GL_LINEAR, magFilter = GL_LINEAR;
  GLenum wrapS = GL_REPEAT, wrapP = GL_REPEAT;
};

std::optional<GLuint> loadTexture(std::span<const uint8_t> mem, TextureOptions opts) {
  return std::nullopt;
}

// TODO: Go through all model.xxx[index] and add a check to make sure that index is valid
void loadTextures(Model& magnetModel, tinygltf::Model& model) {
  for (size_t i = 0; i < model.textures.size(); ++i) {
    auto& texture = model.textures[i];
    auto& image = model.images[texture.source];
    auto& sampler = model.samplers[texture.sampler];

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

        // TODO: Implement sampler parameters
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                     format, type, &image.image.at(0));
        
        magnetModel.textures.insert({i, tex});

        glBindTexture(GL_TEXTURE_2D, 0);
      } else {
        spdlog::error("Unsupported mimetype '{}'. Texture will not be loaded.",
                      mimetype);
      }
    }
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

  tinygltf::Scene defaultScene = model.scenes[model.defaultScene];
  for (auto& nodeIndex : defaultScene.nodes) {
    assert(0 <= nodeIndex && nodeIndex < model.nodes.size());

    traverseNodes(magnetModel, model, model.nodes[nodeIndex], nodeIndex);
  }

  return magnetModel;
}

void Model::destroy() {
  for (auto& [meshIndex, mesh] : meshes) {
    for (auto& meshPrimitive : mesh.primitives) {
      for (auto vbo : meshPrimitive.vbo) {
        glDeleteBuffers(1, &vbo);
      }
      glDeleteBuffers(1, &meshPrimitive.ebo);
      glDeleteVertexArrays(1, &meshPrimitive.vao);
    }
  }

  for (auto& [texIndex, tex] : textures) {
    glDeleteTextures(1, &tex);
  }
}

TempModelRenderer::ModelAndTransform::ModelAndTransform(Model model) {
  this->model = model;
}
std::string fragmentShaderCode =
  "#version 330 core\n\
in vec3 normal;\n\
in vec3 position;\n\
in vec2 texcoord;\n\
\n\
uniform sampler2D baseColorTexture;\n\
uniform vec4 baseColorFactor;\n\
\n\
const float GAMMA = 2.2;\n\
const float INV_GAMMA = 1. / GAMMA;\n\
const float M_PI = 3.141592653589793;\n\
const float M_1_PI = 1.0 / M_PI;\n\
\n\
// We need some simple tone mapping functions\n\
// Basic gamma = 2.2 implementation\n\
// Stolen here:\n\
// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/src/shaders/tonemapping.glsl\n\
\n\
// linear to sRGB approximation\n\
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html\n\
vec3 LINEARtoSRGB(vec3 color) { return pow(color, vec3(INV_GAMMA)); }\n\
\n\
// sRGB to linear approximation\n\
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html\n\
vec4 SRGBtoLINEAR(vec4 srgbIn) {\n\
  return vec4(pow(srgbIn.xyz, vec3(GAMMA)), srgbIn.w);\n\
}\n\
\n\
\n\
\n\
out vec4 color;\n\
void main() {\n\
	color = SRGBtoLINEAR(texture(baseColorTexture, texcoord) * baseColorFactor);\n\
}\n\
";

std::string vertexShaderCode =
  "#version 330 core\n\
layout(location = 0) in vec3 in_vertex;\n\
layout(location = 1) in vec3 in_normal;\n\
layout(location = 2) in vec2 in_texcoord;\n\
\n\
uniform mat4 MVP;\n\
\n\
out vec3 normal;\n\
out vec3 position;\n\
out vec2 texcoord;\n\
\n\
void main(){\n\
	gl_Position = MVP * vec4(in_vertex, 1);\n\
	position = gl_Position.xyz;\n\
	normal = normalize(mat3(MVP) * in_normal);\n\
	position = in_vertex;\n\
	texcoord = in_texcoord;\n\
}";
TempModelRenderer::TempModelRenderer() { 
  this->init();
  if (this->shader == 0) {
    spdlog::error("Failed to load shader");
    assert(false);
  }
}

TempModelRenderer::~TempModelRenderer() {
  for (auto& model : models) {
    model.model.destroy();
  }
}
void TempModelRenderer::init() {

  shader = Shaders::generate(vertexShaderCode, fragmentShaderCode);
}
void TempModelRenderer::update() {

}
void TempModelRenderer::draw() {
  /*glUseProgram(learn_opengl_shader);
  glBindVertexArray(learn_opengl_vao);
  glBindBuffer(GL_ARRAY_BUFFER, learn_opengl_vbo);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  
  glUseProgram(0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);*/

  

  glUseProgram(shader);


  for (auto& model : models) {

    mat4 mvp = {};
    glm_mat4_identity(mvp);
    
    mat4 rot = {};
    glm_quat_mat4(model.rot, rot);
    glm_mat4_mul(rot, mvp, mvp);
    
    mat4 scale = {};
    glm_scale_make(scale, model.scale);
    glm_mat4_mul(scale, mvp, mvp);

    
    mat4 translate = {};
    glm_translate_make(translate, model.pos);
    glm_mat4_mul(translate, mvp, mvp);
    
    
    mat4 projection = {};
    glm_perspective(glm_rad(45.0), width / height, 0.1f, 100.f, projection);
    glm_mat4_mul(projection, mvp, mvp);
    
    

    Shaders::setMat4(shader, "MVP", mvp);

    for (auto& [meshIndex, mesh] : model.model.meshes) {
      for (auto& meshPrimitive : mesh.primitives) {
        Shaders::setVec4(this->shader, "baseColorFactor",
                         meshPrimitive.baseColorFactor);
        glBindVertexArray(meshPrimitive.vao);
        if (meshPrimitive.baseColorTexture) {
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, *meshPrimitive.baseColorTexture);
        }
        glDrawElements(meshPrimitive.mode, meshPrimitive.count,
                       meshPrimitive.componentType,
                       (void*)meshPrimitive.byteOffset);
      }
    }
  }
  glUseProgram(0);
}

}  // namespace Magnet