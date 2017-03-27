#include "ParticleEmitterSoA.h"
#include <algorithm>
#include "glm/glm/gtc/random.hpp"
//#include "TTK/Utilities.h"

ParticleEmitterSoA::ParticleEmitterSoA()
{
	// Initialize memory pointers to null
	particles.positions = nullptr;
	particles.velocities = nullptr;
	particles.remainingLives = nullptr;
	particles.accelerations = nullptr;
	particles.masses = nullptr;
	numParticles = 0;

	playing = false;
}

ParticleEmitterSoA::~ParticleEmitterSoA()
{
	freeMemory();
}


void ParticleEmitterSoA::initialize(unsigned int newNumParticles)
{
	// Clear existing memory
	if (allocated)
		freeMemory();

	if (newNumParticles) // make sure new array size is not 0
	{
		particles.positions = new glm::vec3[newNumParticles];
		particles.velocities = new glm::vec3[newNumParticles];
		particles.accelerations = new glm::vec3[newNumParticles];
		particles.remainingLives = new float[newNumParticles];
		particles.masses = new float[newNumParticles];
		memset(particles.remainingLives, 0, sizeof(float) * newNumParticles);

		numParticles = newNumParticles;
		allocated = true; // mark that memory has been allocated

		// Set up VBO
		AttributeDescriptor posAttrib;
		posAttrib.attributeLocation = VERTEX;
		posAttrib.data = particles.positions;
		posAttrib.elementSize = sizeof(float);
		posAttrib.elementType = GL_FLOAT;
		posAttrib.numElements = numParticles * 3;
		posAttrib.numElementsPerAttrib = 3;

		vbo.addAttributeArray(posAttrib);
		vbo.primitiveType = GL_POINTS;

		vbo.createVBO(GL_DYNAMIC_DRAW);

	}
}

void ParticleEmitterSoA::update(float dt)
{
	// Make sure particle memory is initialized
	if (allocated && playing)
	{
		// loop through each particle
		for (unsigned int i = 0; i < numParticles; i++)
		{
			// Get each property for the particle using pointer arithmetic
			glm::vec3* pos = particles.positions + i;
			glm::vec3* vel = particles.velocities + i;
			glm::vec3* accel = particles.accelerations + i;
			float* life = particles.remainingLives + i;
			float* mass = particles.masses + i;
			// other properties... 

			// check if alive
			if (*life <= 0)
			{
				// if dead respawn
				// could put additional logic here...
				*pos = initialPosition;

				(*vel).x = glm::mix(initialForceMin.x, initialForceMax.x, glm::linearRand(0.0f, 1.0f));
				(*vel).y = glm::mix(initialForceMin.y, initialForceMax.y, glm::linearRand(0.0f, 1.0f));
				(*vel).z = glm::mix(initialForceMin.z, initialForceMax.z, glm::linearRand(0.0f, 1.0f));

				*life = glm::linearRand(lifeRange.x, lifeRange.y);
				*mass = glm::linearRand(0.5f, 1.0f);
				*accel = *vel / *mass;
			}

			// Update position and velocity
			*pos += *vel * dt + *accel * 0.5f * (dt*dt);
			*vel += dt;
			*life -= dt;
		}
	}
}


void ParticleEmitterSoA::draw(Camera* camera)
{
	AttributeDescriptor* attrib = vbo.getAttributeDescriptor(VERTEX);

	glBindVertexArray(vbo.getVAO());
	glEnableVertexAttribArray(attrib->attributeLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.getVBO(VERTEX));
	glBufferSubData(GL_ARRAY_BUFFER, 0, attrib->numElements * attrib->elementSize, attrib->data);

	material->shader->bind();
	material->mat4Uniforms["u_mvp"] = camera->viewProjMatrix;
	material->mat4Uniforms["u_mv"] = camera->viewMatrix;
	material->mat4Uniforms["u_mp"] = camera->projMatrix;
	material->intUniforms["u_tex"] = 0;

	material->sendUniforms();

	if (texture) {
		texture = (GL_TEXTURE0);
	}

	glDepthMask(GL_FALSE);
	vbo.draw();
	glDepthMask(GL_TRUE);
}

void ParticleEmitterSoA::freeMemory()
{
	// Free up all arrays here
	if (allocated)
	{
		delete[] particles.positions;
		delete[] particles.velocities;
		delete[] particles.remainingLives;
		delete[] particles.accelerations;
		delete[] particles.masses;

		particles.positions = nullptr;
		particles.velocities = nullptr;
		particles.remainingLives = nullptr;
		particles.accelerations = nullptr;
		particles.masses = nullptr;

		allocated = false;
		numParticles = 0;
	}
}

