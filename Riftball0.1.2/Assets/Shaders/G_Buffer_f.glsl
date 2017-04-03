#version 400

uniform vec4 u_colour;
uniform mat4 u_m;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
	vec3 FragPos;
} vIn;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragNormal;
layout(location = 2) out vec4 FragPosition;

void main()
{
	FragColor = vec4(u_colour.rgb, 1.0);
	FragNormal = vec4(normalize(vIn.normal),1.0);
	FragPosition = vec4(normalize(vIn.FragPos + vIn.texCoord),1.0);
}