#pragma once
#include <iostream>
#include <GL\glew.h>
#include <GLM\glm\glm.hpp>

class Shader
{
public:
	Shader(const char *, const char *);
	~Shader(void);

	void init(const char *, const char *);

	void bind();		// use the shader vert and frag
	void unbind();		// stop using shader vert and frag

	unsigned int getID();

	void uniformVector(const char*, float*);
	void uniformVector(const char*, float, float, float);
	void uniformVector(const char*, glm::vec3*);
	void uniformFloat(const char*, float );
	void uniformTex(const char*, GLuint , unsigned short);
	void uniformMat4x4(const char*, glm::mat4x4*);

	//------------------------------------------------------------------------
	// Variables

	//glsl program handler
	unsigned int programID;

	//vert and fag shaders
	unsigned int vertShader;
	unsigned int fragShader;


};

