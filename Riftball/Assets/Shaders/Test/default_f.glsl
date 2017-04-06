#version 400

uniform vec4 u_lightPos;
uniform vec4 u_colour;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragNormal;

void main()
{
	FragColor = vec4(vIn.normal * 0.5 + 0.5, 1.0f);
	
	vec3 L = normalize(u_lightPos.xyz - vIn.posEye);
	vec3 N = normalize(vIn.normal);

	float diffuse = max(0.0, dot(N, L));

	if (diffuse <= 0.00) diffuse = 0.00;
	else if (diffuse <= 0.25) diffuse = 0.25;
	else if (diffuse <= 0.50) diffuse = 0.50;
	else if (diffuse <= 0.75) diffuse = 0.75;
	else diffuse = 1.00;

	FragColor = vec4(vec3(0.5, 0.5, 0.5) * (diffuse * 0.8f) + u_colour.rgb, 1.0f);

	FragNormal = vec4(N * 0.5 + 0.5, 1.0);
}