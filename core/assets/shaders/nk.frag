#version 450 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

precision mediump float;
uniform sampler2D smp;
in vec2 Frag_UV;
in vec4 Frag_Color;
out vec4 Out_Color;

void main() {
  Out_Color = Frag_Color * texture(smp, Frag_UV.st);
}