
#version 330

layout(location = 0) in vec3 vIn_vertex;
layout(location = 1) in vec3 vIn_uv;
layout(location = 2) in vec3 vIn_normal;
layout(location = 3) in vec3 vIn_colour;

out VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 eyePos;
} vOut;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform mat4 u_mp;

uniform mat4 localTransform;

void main()
{
	vOut.texCoord = vIn_uv;
	vOut.colour = vec4(vIn_colour, 1.0);
	vOut.normal = vIn_normal;
	gl_Position = vec4(vIn_vertex, 1.0) * localTransform;
}



