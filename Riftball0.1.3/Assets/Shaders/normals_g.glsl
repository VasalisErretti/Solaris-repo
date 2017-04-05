// Geometry shader which draws vertex and face normals and wireframe.

#version 400

layout(triangles) in;
layout(line_strip, max_vertices = 64) out;

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

// Outputs vertex normals
void VertexNormals()
{
	float lineLength = 0.2;
	for (int i = 0; i < 3; i++){
		gl_Position = u_mvp * gl_in[i].gl_Position;
		vOut.colour = vec4(0.0, 1.0, 0.0, 1.0);
		EmitVertex();

		vec3 lineEnd = gl_in[i].gl_Position.xyz + vIn[i].normal * lineLength;
		gl_Position = u_mvp * vec4(lineEnd, 1.0);
		vOut.colour = vec4(0.0, 1.0, 0.0, 1.0);
		EmitVertex();

		EndPrimitive();
	}
}

void FaceNormals();
void WireFrame();

void main()
{
	VertexNormals();
	FaceNormals();
	WireFrame();
}

// Outputs face normals normals
void FaceNormals()
{
	
}

void WireFrame()
{
	gl_Position = u_mvp * vec4((gl_in[0].gl_Position.xyz + vIn[0].normal.xyz * 0.003), 1.0);
	vOut.colour = vec4(0.0);
	EmitVertex();
	gl_Position = u_mvp * vec4((gl_in[1].gl_Position.xyz + vIn[1].normal.xyz * 0.003), 1.0);
	vOut.colour = vec4(0.0);
	EmitVertex();
	EndPrimitive();


	gl_Position = u_mvp * vec4((gl_in[0].gl_Position.xyz + vIn[0].normal.xyz * 0.003), 1.0);
	vOut.colour = vec4(0.0);
	EmitVertex();
	gl_Position = u_mvp * vec4((gl_in[2].gl_Position.xyz + vIn[2].normal.xyz * 0.003), 1.0);
	vOut.colour = vec4(0.0);
	EmitVertex();
	EndPrimitive();


	gl_Position = u_mvp * vec4((gl_in[1].gl_Position.xyz + vIn[1].normal.xyz * 0.003), 1.0);
	vOut.colour = vec4(0.0);
	EmitVertex();
	gl_Position = u_mvp * vec4((gl_in[2].gl_Position.xyz + vIn[2].normal.xyz * 0.003), 1.0);
	vOut.colour = vec4(0.0);
	EmitVertex();
	EndPrimitive();
}

