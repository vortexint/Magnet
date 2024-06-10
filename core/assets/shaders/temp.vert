#version 330 core
layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexcoord0;

// Color can be either a vec3 or a vec4, the correct one will be bound
layout(location = 4) in vec4 inVec4Color0;
layout(location = 4) in vec3 inVec3Color0;

// inJoints0 are list of 4 node ids that the current vertex is attached to
// the gltf references joint ids is some areas, they are the node indices
// inWeights0 is a list of the weight of each of the corresponding joint ids
layout(location = 5) in ivec4 inJoints0;
layout(location = 6) in vec4 inWeights0;

uniform mat4 MVP;


// TODO: Convert this to a uniform buffer or shader storage buffer
uniform mat4 globalBoneTransforms[48];

uniform bool isColorVec3ElseVec4;
vec4 getColor() {
	if (isColorVec3ElseVec4) {
		return vec4(inVec3Color0, 1.0);
	} else {
		return inVec4Color0;
	}
}

/*

  "bufferView": 1,
  "componentType": 5123,
  "count": 1728,
  "type": "VEC4",
  "byteOffset": 13824

	bufferView: 1 
    {
      "buffer": 0,
      "byteOffset": 20736,
      "byteLength": 27648,
      "byteStride": 8,
      "target": 34962
    }
	TODO:
	 - [ ] Get the transformation matricies correct
		- [X] Ignore the matrices at first and check if it correctly loads
		- [ ] Inspect the binary data in joints_n
		- [ ] Keep modifying the boneTransformation until it works
	On inspection, all the matrices for Fox.glb are identity,
	so when I get rid of inWeights0[i] the Fox should look normal.

	I replaced inJoints0[i] with 0, so I only select the first matrix.
	This caught the trap. inJoints0[i] is out of bounds for some reason
	I really need to inspect the binary data contained in inJoints0[i]
 */

vec3 applyBoneTransform(vec3 vertex) {
    vec3 position = vec3(0.0);
	
	for (int i = 0; i < 4; ++i) {
		if (inJoints0[i] > 48) {
			continue;
		}
		mat4 boneTransform = globalBoneTransforms[inJoints0[i]];
		// When I set the boneTransform to the indentity matrix everything works
		// this indicates that the problem is how the globalBoneTransforms is used
		// Reading the gltf 2 spec it says that: Note that per-joint inverse bind matrices (when present) MUST be applied before the base node transforms.
		//mat4 boneTransform = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		position += vec3(/*inWeights0[i] */ (boneTransform * vec4(vertex, 1.0)));
		//position += 0.25 * vertex;
	}

	return position;
}


out vec3 normal;
out vec3 position;
out vec2 texcoord;
out int error;

void main(){
	vec4 vertex = vec4(applyBoneTransform(inVertex), 1.0);

	gl_Position = MVP * vertex;
	position = gl_Position.xyz;
	normal = normalize(mat3(MVP) * applyBoneTransform(inNormal));
	texcoord = inTexcoord0;
	
	error = 0;
}