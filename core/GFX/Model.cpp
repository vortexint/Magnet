#include <string>
#include <glad/glad.h>
#include <GFX/Model.hpp>
#include <magnet/Shaders.hpp>

#include <spdlog/spdlog.h>
#include "tiny_gltf.h"

namespace Magnet {

static constexpr int VERTEX_ATTRIB_LOC = 0;
static constexpr int NORMAL_ATTRIB_LOC = 1;
static constexpr int TEX_COORD_0_ATTRIB_LOC = 2;
void createMesh(Model &magnetModel, tinygltf::Model& model,
                tinygltf::Mesh& mesh) {
  
   
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive& primitive = mesh.primitives[i];
    
    std::vector<unsigned> vbos;
    GLuint vao = 0;
    GLuint ebo = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
    tinygltf::BufferView &indexBufferView =
      model.bufferViews[indexAccessor.bufferView];
    tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
    assert(model.bufferViews[indexAccessor.bufferView].target ==
            GL_ELEMENT_ARRAY_BUFFER);
    glGenBuffers(1, &ebo);
    glBindBuffer(indexBufferView.target, ebo);
    assert(indexBufferView.byteOffset + indexBufferView.byteLength <=
            indexBuffer.data.size());
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



      GLuint vbo = 0;
      glGenBuffers(1, &vbo);
      glBindBuffer(bufferView.target, vbo);
      assert(bufferView.byteOffset + bufferView.byteLength <= buffer.data.size());
      glBufferData(bufferView.target, bufferView.byteLength,
                   &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW); 

      vbos.push_back(vbo);

      int byteStride =
        accessor.ByteStride(bufferView);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }
      
      int vaa = -1;
      if (attribName == "POSITION") {
        vaa = VERTEX_ATTRIB_LOC;
      } else if (attribName == "NORMAL") {
        vaa = NORMAL_ATTRIB_LOC;
      } else if (attribName == "TEXCOORD_0") {
        vaa = TEX_COORD_0_ATTRIB_LOC;
      }
      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, (void*)(accessor.byteOffset));
      }


    }
    
    magnetModel.meshes.push_back(Model::Mesh{std::move(vbos), ebo, vao, primitive.mode, indexAccessor.count, indexAccessor.componentType, indexAccessor.byteOffset });

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

}

void traverseNodes(Model &magnetModel, tinygltf::Model& model,
                   tinygltf::Node& node) {
  if (0 <= node.mesh && node.mesh < model.meshes.size()) {
    createMesh(magnetModel, model, model.meshes[node.mesh]);
  }

  for (auto nodeIndex : node.children) {
    assert(0 <= nodeIndex && nodeIndex < node.children.size());

    traverseNodes(magnetModel, model, model.nodes[nodeIndex]);
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

  tinygltf::Scene defaultScene = model.scenes[model.defaultScene];
  for (auto& nodeIndex : defaultScene.nodes) {
    assert(0 <= nodeIndex && nodeIndex < model.nodes.size());

    traverseNodes(magnetModel, model, model.nodes[nodeIndex]);
  }

  return magnetModel;
}

void Model::destroy() {
  for (auto& mesh : meshes) {
    for (auto vbo : mesh.vbo) {
      glDeleteBuffers(1, &vbo);
    }
    glDeleteBuffers(1, &mesh.ebo);
    glDeleteVertexArrays(1, &mesh.vao);
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
uniform sampler2D tex;\n\
\n\
out vec4 color;\n\
void main() {\n\
	color = vec4(1.0, 1.0, 1.0, 1.0);\n\
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

    for (auto& mesh : model.model.meshes) {
      glBindVertexArray(mesh.vao);
      glDrawElements(mesh.mode, mesh.count, mesh.componentType,
                     (void*)mesh.byteOffset);
    }
  }
  glUseProgram(0);
}

}  // namespace Magnet