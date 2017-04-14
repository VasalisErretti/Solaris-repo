
#version 330
layout(location = 0) in vec3 vIn_vertex;

out VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 eyePos;
} vOut;

uniform mat4 u_mvp; //glm::mat4 lightSpaceMatrix = lightProjection * lightView; //u_mvp
uniform mat4 localTransform; //model

void main()
{
	gl_Position = u_mvp * localTransform * vec4(vIn_vertex, 1.0);
}

