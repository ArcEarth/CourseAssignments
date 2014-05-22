#version 330 core
 
// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

 
// Ouput data
out vec3 color;
 
// Values that stay constant for the whole mesh.
uniform sampler2D ColorTextureSampler;
uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform vec4 AmbientLightColor;
uniform vec4 DirectionalLightColor;
uniform vec3 DirectionalLightDirection;
uniform vec4 SpotLightColor_1;
uniform vec3 SpotLightPosition_1;
uniform vec4 SpotLightDirection_1;
uniform vec4 FogColor; // the w component represent the invert of visiable distance

void main(){
 
	// Material properties
	vec3 MaterialDiffuseColor = texture2D( ColorTextureSampler, UV ).rgb;

	vec3 MaterialAmbientColor = AmbientLightColor.xyz * MaterialDiffuseColor;
	//vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// Diffuese colors 
	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	//vec3 n = mat3(ModelViewMatrix) * vec3(0.0f,1.0f,0.0f);
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );

	float cosTheta = dot( n,l );
	cosTheta = clamp( dot( n,l ), 0,1 );
	// // Distance to the spot light light
	vec3 displacememt = Position_worldspace - SpotLightPosition_1;
	float distance = length( displacememt );
	// displacememt = displacememt / distance;
	// displacememt = mat3(ViewMatrix) * displacememt;
	// float angle = clamp(dot (displacememt,-SpotLightDirection_1.xyz),0,1);
	// float radius = SpotLightDirection_1.w;
	// if (angle <= radius) angle = 0;

	//vec3 SpotLightDiffuseColor = SpotLightColor_1.xyz * angle;
	vec3 SpotLightDiffuseColor = SpotLightColor_1.xyz * SpotLightColor_1.w / (distance*distance) * cosTheta;
	SpotLightDiffuseColor *= MaterialDiffuseColor;

	vec3 DirectionalLightDirection_CameraSpace = mat3(ViewMatrix) * -DirectionalLightDirection;
	l = normalize(DirectionalLightDirection_CameraSpace);
	cosTheta = clamp( dot( n,l ), 0,1 );
	vec3 DirectionalLightDiffuseColor = DirectionalLightColor.xyz * cosTheta;
	DirectionalLightDiffuseColor *= MaterialDiffuseColor;

	// // Eye vector (towards the camera)
	// vec3 E = normalize(EyeDirection_cameraspace);
	// // Direction in which the triangle reflects the light
	// vec3 R = reflect(-l,n);
	// // Cosine of the angle between the Eye vector and the Reflect vector,
	// // clamped to 0
	// //  - Looking into the reflection -> 1
	// //  - Looking elsewhere -> < 1
	// float cosAlpha = clamp( dot( E,R ), 0,1 );
	// // Specular : reflective highlight, like a mirror
	// vec3 MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

	color = MaterialAmbientColor + SpotLightDiffuseColor + DirectionalLightDiffuseColor;

	float alpha =  clamp(distance * FogColor.w,0,1);
	color = FogColor.xyz * alpha + (1.0f-alpha) * color;
}