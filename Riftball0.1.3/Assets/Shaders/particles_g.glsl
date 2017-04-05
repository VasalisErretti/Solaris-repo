// Geometry shader which turns points into billboarded quads

#version 400

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform mat4 u_mp;

// Input from Vertex shader
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 eyePos;
} vIn[]; // array size = num vertices in primitve

// Output from geometry shader
out VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 eyePos;
} vOut; // array size depends on max_vertices


// Creates a quad of specified size around point p
void PointToQuadBillboarded(vec4 p, float size)
{
	float halfSize = size * 0.5;

	vec4 pEye = u_mv * p;

	vec4 TopLeft = vec4(pEye.xy + vec2(-halfSize, halfSize), pEye.z, 1.0);
	vec4 BotLeft = vec4(pEye.xy + vec2(-halfSize, -halfSize), pEye.z, 1.0);
	vec4 TopRight =vec4(pEye.xy + vec2(halfSize, halfSize), pEye.z, 1.0);
	vec4 BotRight =vec4(pEye.xy + vec2(halfSize, -halfSize), pEye.z, 1.0);

	gl_Position = u_mp * BotLeft;
	vOut.texCoord.xy = vec2(0.0, 0.0);
	EmitVertex();
	gl_Position = u_mp * TopLeft;
	vOut.texCoord.xy = vec2(1.0, 0.0);
	EmitVertex();
	gl_Position = u_mp * BotRight;
	vOut.texCoord.xy = vec2(0.0, 1.0);
	EmitVertex();
	gl_Position = u_mp * TopRight;
	vOut.texCoord.xy = vec2(1.0, 1.0);
	EmitVertex();


	EndPrimitive();
}

void main()
{
	PointToQuadBillboarded(gl_in[0].gl_Position, 1.0);
}




