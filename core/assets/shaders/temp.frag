#version 330 core
in vec3 normal;
in vec3 position;
in vec2 texcoord;
flat in int error;

uniform sampler2D baseColorTexture;
uniform vec4 baseColorFactor;

uniform sampler2D metallicRoughnessTexture;
uniform float metalicFactor;
uniform float roughnessFactor;

uniform sampler2D emissiveTexture;
uniform vec3 emissiveFactor;

uniform sampler2D occlussionTexture;
uniform sampler2D normalTexture;

const float GAMMA = 2.2;
const float INV_GAMMA = 1. / GAMMA;
const float M_PI = 3.141592653589793;
const float M_1_PI = 1.0 / M_PI;

 //We need some simple tone mapping functions
 //Basic gamma = 2.2 implementation
 //Stolen here:
 //https:github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/src/shaders/tonemapping.glsl

 //linear to sRGB approximation
 //see http:chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 LINEARtoSRGB(vec3 color) { return pow(color, vec3(INV_GAMMA)); }

 //sRGB to linear approximation
 //see http:chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec4 SRGBtoLINEAR(vec4 srgbIn) {
  return vec4(pow(srgbIn.xyz, vec3(GAMMA)), srgbIn.w);
}



out vec4 color;
void main() {
	// DEBUG Make sure to swap the texture with baseColorTexture 
	if (error == 0) {
		color = SRGBtoLINEAR(texture(baseColorTexture, texcoord) * baseColorFactor);
	} else {
		color = vec4(1,0,0,0);
	}
}