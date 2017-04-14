
#version 330

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 eyePos;
} vIn;
layout(location = 0) out vec4 FragColor;



float near = 1.0;
float far = 10000.0;
float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0;
	return ((2.0 * near * far) / ((far + near) - z * (far - near)));
}

void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	FragColor = vec4(vec3(depth), 1.0);
}

