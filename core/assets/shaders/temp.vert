#version 330 core
layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexcoord0;

// Color can be either a vec3 or a vec4, the correct one will be bound
layout(location = 4) in vec4 inVec4Color0;
layout(location = 4) in vec3 inVec3Color0;

layout(location = 5) in vec4 inJoints0;
layout(location = 6) in vec4 inWeights0;

uniform mat4 MVP;

uniform bool isColorVec3ElseVec4;
vec4 getColor() {
	if (isColorVec3ElseVec4) {
		return vec4(inVec3Color0, 1.0);
	} else {
		return inVec4Color0;
	}
}

out vec3 normal;
out vec3 position;
out vec2 texcoord;


void main(){
	gl_Position = MVP * vec4(inVertex, 1);
	position = gl_Position.xyz;
	normal = normalize(mat3(MVP) * inNormal);
	position = inVertex;
	texcoord = inTexcoord0;
}