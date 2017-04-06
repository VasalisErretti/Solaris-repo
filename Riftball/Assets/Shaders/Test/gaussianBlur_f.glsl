#version 400

uniform sampler2D u_tex;

uniform vec4 u_texelSize; // (1.0 / windowWidth, 1.0 / windowHeight)

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;

vec3 boxBlur(sampler2D tex)
{
	//////////////////////////////////////////////////////////////////////////
	// CODE HERE
	//////////////////////////////////////////////////////////////////////////
	return vec3(1.0, 0.0, 0.0);1
}

void main()
{
	vec3 blurred = boxBlur(u_tex);
	FragColor = vec4(blurred, 1.0);
}