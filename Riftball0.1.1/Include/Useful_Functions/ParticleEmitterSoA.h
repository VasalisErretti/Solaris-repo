#pragma once

#include "Useful_Functions\VertexBufferObject.h"
#include "glm/glm/vec3.hpp"
#include <memory>
#include <math.h>
#include "Shader_Functions\Material.h"
#include "Useful_Functions\ExtraFunctions.cpp"
//#include "TTK/Camera.h"

class ParticleEmitterSoA
{
	struct Particle
	{
		glm::vec3* positions;
		glm::vec3* velocities;
		glm::vec3* accelerations;
		float*	remainingLives;
		float* masses;
		// Other particle properties...
	} particles;

public:
	ParticleEmitterSoA();
	~ParticleEmitterSoA();

	// Initialize memory for particles
	void initialize(unsigned int numParticles);

	// playback control
	inline void play() { playing = true; }
	inline void pause() { playing = false; }

	// Updates each particle in system
	void update(float dt);

	// Draws particles to screen
	void draw(Camera* camera);

	void freeMemory();

	// Emitter position in world space
	// aka where the particles spawn
	glm::vec3 initialPosition;

	// locomotion properties
	// Ranges store the min (x) and max(y) bound of random values for the property
	glm::vec3 lifeRange;
	glm::vec3 initialForceMin;
	glm::vec3 initialForceMax;


	VertexBufferObject vbo;

	bool playing;	// false is update is paused

	std::shared_ptr<Material> material;

	GLuint texture;
private:
	unsigned int numParticles; // Number of particles passed into initialize()
	bool allocated; // false if memory not allocated
};