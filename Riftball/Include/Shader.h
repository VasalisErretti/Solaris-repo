#pragma once
#include <string>
#include <GL\glew.h>

class Shader
{
private:
	unsigned int handle;
	GLenum shaderType;

public:

	// It's a good idea to use the same attribute location layouts for each shader
	// Not required, but it makes VBO creation easier
	enum AttributeLocations
	{
		VERTEX = 0,
		TEXCOORD = 1, //normal
		NORMAL = 2, //texcord
		COLOUR = 3
		// ... any other attributes...
	};

	Shader();
	~Shader();

	// Returns shader handle
	unsigned int loadShaderFromFile(std::string fileName, GLenum type);

	unsigned int getHandle() { return handle; }

	void destroy();
};
