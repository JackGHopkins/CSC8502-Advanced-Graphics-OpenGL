#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	//float visibility;
} IN;

out vec4 fragColour;

void main(void) {
	vec3 skyColour = vec3(0.5,0.5,0.5);

	vec3 incident = normalize(vec3(255.0f, 255.0f, 0.0f));
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);
	vec3 lightDir = normalize(-lightPos);

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
	
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 bumpNormal = texture(bumpTex, IN.texCoord).rgb;
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));
	
	float lambert = max(dot(incident, bumpNormal), 0.0f);
	float distance = length(lightPos - IN.worldPos);
	float attenuation = 1.0f - clamp(distance / lightRadius, 0.0, 1.0);
	float specFactor = clamp(dot(halfDir, bumpNormal), 0.0, 1.0);
	specFactor = pow(specFactor, 60.0);
	
	vec3 surface = (diffuse.rgb * lightColour.rgb);
	fragColour.rgb = surface * lambert;
	fragColour.rgb += (lightColour.rgb * specFactor) * 0.33;
	fragColour.rgb += surface * 0.1f;
	fragColour.a = diffuse.a;

	//fragColour = mix(vec4(skyColour, 1.0), fragColour, IN.visibility);
}
