
#version 330

layout(location = 0) in vec3 vIn_vertex;
layout(location = 1) in vec2 vIn_uv;
layout(location = 2) in vec3 vIn_normal;
layout(location = 3) in vec3 vIn_colour;

out VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 eyePos;
	vec3 vertex;
} vOut;

uniform mat4 u_mvp;
uniform mat4 u_mv;

uniform mat4 mvm; // modelview matrix
uniform mat4 prm; // projection matrix
uniform mat4 localTransform;

void main()
{
	vOut.colour = vec4(vIn_colour, 1.0);
	vOut.texCoord = vIn_uv;
	vOut.normal = (mat3(mvm) * mat3(localTransform) * vIn_normal); //real
	vOut.eyePos = (u_mv * vec4(vIn_vertex, 1.0)).xyz; //real
	
	vOut.vertex = vIn_vertex;
	//vOut.normal = (u_mv * vec4(vIn_normal, 0.0)).xyz;
	//vOut.eyePos = mat3(mvm) * mat3(localTransform) * vIn_vertex;

	vec4 localSpace = localTransform * vec4(vIn_vertex, 1.0);
	vec4 modelViewSpace = mvm * localSpace;
	vec4 worldSpace = prm * modelViewSpace;
	gl_Position = worldSpace;	//every vert shader must set gl_Position
}



