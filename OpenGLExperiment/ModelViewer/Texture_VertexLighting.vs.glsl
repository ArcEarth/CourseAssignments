#version 330 core
 
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
 
// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

// Values that stay constant for the whole mesh.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelViewProjection;
uniform mat4 ModelViewMatrix;
uniform vec3 SpotLightPosition_1;


 
void main(){
 
    // Output position of the vertex, in clip space : ModelViewProjection * position
    gl_Position =  ModelViewProjection * vec4(vertexPosition,1);
 
    // UV of the vertex. No special space for this one.
    UV = vertexUV;

	// Position of the vertex, in worldspace : M * position
	Position_worldspace = (ModelMatrix * vec4(vertexPosition,1)).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( ViewMatrix * vec4(Position_worldspace,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition_cameraspace = ( ViewMatrix * vec4(SpotLightPosition_1,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
	// Normal of the the vertex, in camera space
	Normal_cameraspace = mat3(ModelViewMatrix) * vertexNormal; 
	//Normal_cameraspace = normalize(Normal_cameraspace);
	// Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

}
