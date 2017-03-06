#pragma once
// Core Libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

// 3rd Party Libraries
#include <glm\glm\glm.hpp>
#include <GL\glew.h>
#include <GLUT\glut.h>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm\gtc\type_ptr.hpp>

// User Libraries
#include "Shader.h"
#include "JointTypes.h"
#include "Material.h"

struct Face
{
	Face() {}
	// these are arrays of size 3 to represent 3 points of a triangle
	// these values represent the indices of the points
	unsigned int normals[3], vertices[3], textures[3];
};

//will represent the functionality each object in the game will have
class GameObject
{
protected:
	//object position
	glm::vec3 m_Position;
	glm::mat4 m_Rotation;
	glm::vec3 m_Angle;
	glm::vec3 m_Scale;
	//object stats stuff
	glm::vec3 m_Top;
	glm::vec3 m_Bottom;
	glm::vec3 m_Radius;
	glm::vec3 m_Size;
	glm::vec4 m_Colour;
	//Physics
	float m_Mass;
	float m_InvertedMass;
	glm::vec3 m_Velocity;
	glm::vec3 m_Acceleration;
	glm::vec3 m_ForceOnObject;
	float m_Restitution; // Bounciness of this object
	float m_Drag;

	//Modern openGL Object Loader///////////
	GLuint vao;
	GLuint vertbo;
	GLuint normbo;
	GLuint texbo;
	GLuint colorbo;
	float *verts;
	float *norms;
	float *texs;
	float *colors;
	unsigned int numtris; // count number of vertices for data creation

	GLuint textureHandle;

	////
	glm::mat4 m_LocalTransformMatrix;
	glm::mat4 m_LocalToWorldMatrix;
	// Forward Kinematics
	GameObject* m_Parent;
	std::vector<GameObject*> m_Children;
	unsigned int m_CurrentFrame; // HTR animation


	////
	int m_SpecialAttribute = 0;

public:
	bool Viewable = true;
	float SprintSpeed = 1.5f;

	bool inAir = false; float InAirCounter = 0.0f; bool IsJumping = false;
	bool onObject = false; int onObjectNum = 0;
	bool inShock = false; float TimeInShock = 0.0f;
	bool AreIdle = false; float IdleTimeCounter = 0.0f;
	bool textureHandle_hasTransparency = false;
	glm::vec3 ForwardDirection = glm::vec3(1.0f, 1.0f, 1.0f);
	float FaceYRotation = 0.0f;

	GameObject();
	GameObject(glm::vec3 position, std::shared_ptr<Material> _material);
	~GameObject();

	void setTexture(GLuint _textureHandle) { textureHandle = _textureHandle; }

	void setPosition(glm::vec3 newPosition) { m_Position = newPosition; }
	void setRotation(glm::vec3 newRotation) { 
		m_Angle = newRotation;

		glm::mat4 rx = glm::rotate(newRotation.x*(3.14159f / 180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 ry = glm::rotate(newRotation.y*(3.14159f / 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rz = glm::rotate(newRotation.z*(3.14159f / 180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		m_Rotation = rz * ry * rx;
	}
	void setScale(glm::vec3 newScale) { m_Scale = newScale; }
	void setSizeOfHitBox(glm::vec3 newSize) {
		m_Size = (newSize);
		m_Top = (m_Size*0.5f);
		m_Bottom = (m_Size*-0.5f);
		m_Radius = (m_Size);
	}
	void setMass(float newMass)
	{
		if (newMass < 0) { newMass = 0; }
		m_Mass = newMass;
		if (newMass == 0) { m_InvertedMass = 0; }
		else { m_InvertedMass = 1 / m_Mass; }
	}
	void setDrag(float newDrag) { m_Drag = newDrag; }
	void setForceOnObject(glm::vec3 newForceOnObject){ m_ForceOnObject = newForceOnObject; }
	void setVelocity(glm::vec3 newVelocity) { m_Velocity = newVelocity; }
	void setAcceleration(glm::vec3 newAcceleration) { m_Acceleration = newAcceleration; }
	void setColour(glm::vec4 newColour) { m_Colour = newColour; }
	void setColour(glm::vec3 newColour) { m_Colour = glm::vec4(newColour, 1.0f); }
	void setRestitution(float newRestitution) { m_Restitution = newRestitution; }

	void setSpecialAttribute(int newSpecialAttribute) { m_SpecialAttribute = newSpecialAttribute; }
	void setMaterial(std::shared_ptr<Material> newMaterial) { material = newMaterial; }


	glm::vec3 Position() { return m_Position; }
	glm::vec3 Angle() { return m_Angle; }
	glm::mat4 Rotation() { return m_Rotation; }
	glm::vec3 Top() { return (m_Position + m_Top); }
	glm::vec3 Bottom() { return (m_Position + m_Bottom); }
	glm::vec3 Radius() { return m_Radius; }
	glm::vec3 Scale() { return m_Scale; }
	glm::vec3 ForceOnObject() { return m_ForceOnObject; }
	glm::vec3 Velocity() { return m_Velocity; }
	glm::vec3 Acceleration() { return m_Acceleration; }
	float Drag() { return m_Drag; }
	float Mass() { return m_Mass; }
	float InvertedMass() { return m_InvertedMass; }
	float Restitution() { return m_Restitution; }

	int SpecialAttribute() { return m_SpecialAttribute; }



	std::string name;
	JointDescriptor* jointAnimation; // Animation from HTR
	glm::mat4 GameObject::getLocalToWorldMatrix() { return m_LocalToWorldMatrix; }
	void GameObject::setParent(GameObject* newParent) { m_Parent = newParent; }
	void GameObject::addChild(GameObject* newChild) {
		if (newChild) {
			m_Children.push_back(newChild);
			newChild->setParent(this); // tell new child that this game object is its parent
		}
	}
	void GameObject::removeChild(GameObject* rip) {
		for (unsigned int i = 0; i < m_Children.size(); ++i) {
			if (m_Children[i] == rip) { m_Children.erase(m_Children.begin() + i); }
		}
	}
	glm::vec3 GameObject::getWorldPosition() {
		if (m_Parent) { return m_Parent->getLocalToWorldMatrix() * glm::vec4(m_Position, 1.0f); }
		else { return m_Position; }
	}
	glm::mat4 GameObject::getWorldRotation() {
		if (m_Parent) { return m_Parent->getWorldRotation() * m_Rotation; }
		else { return m_Rotation; }
	}
	bool GameObject::isRoot() {
		if (m_Parent) { return false; }
		else { return true; }
	}

	void GameObject::drawHTR(Shader *s);
	void GameObject::updateHTR(float dt);

	virtual void GameObject::update(float deltaT);
	virtual void GameObject::updateP(float deltaT);
	virtual void GameObject::drawObject();

	bool GameObject::objectLoader(const char* filename);
	bool GameObject::objectLoader(std::string filename);
	void GameObject::objectLoader(GameObject * objPath);
	void GameObject::objectHitBox(GameObject * objPath);
	void GameObject::objectLoaderHTR(GameObject * objPath);
	void GameObject::morphTarget(GameObject * objPath, float dt);




	std::shared_ptr<Material> material;
};




