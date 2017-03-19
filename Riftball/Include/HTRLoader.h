#pragma once

#include <string>
#include "GameObject.h"
#include "JointTypes.h"

class HTRLoader
{
public:
	HTRLoader();

	// Loads the specified HTR file. 
	// Returns true if successful
	bool loadHTR(std::string fileName);

	// Loop through each joint descriptor until the input jointName is found
	// If not found, will return a nullptr
	JointDescriptor* getJointDescriptorByName(std::string jointName);

	// Returns pointer to specific joint
	GameObject* getGameObjectByName(std::string jointName);

	// Turns the HTR file into usable game objects
	void createGameObjects();

	// Returns root node (usually the hip if a humanoid skeleton)
	GameObject* getRootGameObject();

private:

	// Functions used in parsing process
	bool processHeaderSection(FILE* fp, char* loc);
	bool processHeader(std::string header, std::string value);
	bool processSegementNameSection(FILE* fp, char* loc);
	bool processBasePositionSection(FILE* fp, char* loc);
	bool processAnimationSection(FILE* fp, char* loc);
	
	// Increments the file pointer to the next line in the file
	// and copies it into buffer
	void goToNextValidLineInFile(FILE* fp, char* buffer);

	// Takes in 3 euler angles and returns a quaternion
	glm::quat createQuaternion(float rx, float ry, float rz);

	// Describes the data in the file

	std::string fileType;		// File extension
	std::string dataType;		// What kind of data is stored in file. 
								// Ie. HTRS means Hierarchical translations followed by rotations and scale
	
	int			fileVersion;	// Useful if you are generating binary object files on load, can check the version of an existing binary file, 
								// check version of text, if text version > binary version then reparse, otherwise just use binary

	int			numSegments;	// Number of bones in skeleton
	int			numFrames;		// number of frames in the animation
	int			fps;			// FPS of the animation

	RotationOrder rotationOrder;	// Order to apply rotations
	std::string	calibrationUnits;// 
	char		upAxis;			// X, Y, Z (usually Y)
	std::string rotationUnits;	// "Degrees" or "Radians"
	char		boneLengthAxis;	// Axis that is aligned with bone

	// Actual animation data
	std::vector<JointDescriptor> jointDescriptors;	// Stores each joint and its parent (could be an array since we know number of segments)

	std::vector<GameObject> jointGameObjects;

	GameObject* rootGameObject;
};