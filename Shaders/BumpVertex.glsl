#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	float visibility;
} OUT;

const float fogDensity = 0.007f;
const float fogGradient = 1.5f;

void main ( void ) {
	OUT.colour = colour;
	OUT.texCoord = texCoord;
	
	mat3 normalMatrix = transpose (inverse(mat3(modelMatrix)));

	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = wNormal ;
	OUT.tangent = wTangent ;
	OUT.binormal = cross (wTangent , wNormal ) * tangent.w;

	vec4 worldPos = (modelMatrix * vec4 (position ,1));
	vec4 relativeCamPos = viewMatrix * worldPos;
	OUT.worldPos = worldPos.xyz;

	gl_Position = projMatrix * relativeCamPos;

	float distance = length(relativeCamPos.xyz);
	OUT.visibility = exp(-pow((distance* fogDensity), fogGradient));
}