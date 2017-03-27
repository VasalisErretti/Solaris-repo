
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

	vOut.normal = (u_mv * localTransform * vec4(vIn_normal, 0.0)).xyz; //real
	vOut.eyePos = (u_mv * localTransform * vec4(vIn_vertex, 1.0)).xyz; //real

	//vec4 localSpace = localTransform * vec4(vIn_vertex, 1.0);
	//vec4 modelViewSpace = u_mv * localSpace;
	//vec4 worldSpace = u_mp * modelViewSpace;
	//gl_Position = worldSpace;
	//or
	gl_Position = u_mvp * localTransform * vec4(vIn_vertex, 1.0);
}



