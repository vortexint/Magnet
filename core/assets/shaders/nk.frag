#version 440 core
#extension GL_ARB_bindless_texture : require

precision mediump float;
layout(bindless_sampler) uniform sampler2D Texture;
in vec2 Frag_UV;
in vec4 Frag_Color;
out vec4 Out_Color;

void main(){
    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);
}
