#include "HTRLoader.h"
#include <iostream>

#define MAX_LINE_LENGTH 128

HTRLoader::HTRLoader()
{
	rootGameObject = nullptr;
}

bool HTRLoader::loadHTR(std::string fileName)
{
	FILE* file;
	//file = fopen(fileName.c_str(), "r");
	errno_t errorCode = fopen_s(&file, fileName.c_str(), "r");

	char* loc;
	char buffer[MAX_LINE_LENGTH];
	int bufferSize = 0;

	if (file)
	{
		// Find line with header
		// fgets() will read to new line OR MAX_LINE_LENGTH, which ever happens first
		// Returns pointer to buffer OR null pointer for end of file
		while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL)
		{
			// Get location of "Header" in file
			loc = strstr(buffer, "Header");

			// Process headers
			if (loc)
			{
				// CODE HERE A //////////////////////////////////////////////////////////////////////////
				if (!processHeaderSection(file, loc)) { break; }
				if (!processSegementNameSection(file, loc)) { break; }
				if (!processBasePositionSection(file, loc)) { break; }
				if (!processAnimationSection(file, loc)) { break; }
				//if (!(file, loc)) { break; }
			}
		}

		fclose(file);	// Remember to close file

		return true;
	}
	else
	{
		std::cout << "LOAD HTR ERROR: " << fileName << " not found or cannot be opened." << std::endl;
		return false;
	}
}

bool HTRLoader::processHeaderSection(FILE* fp, char* loc)
{
	// CODE HERE B //////////////////////////////////////////////////////////////////////////
	char buffer[MAX_LINE_LENGTH];

	goToNextValidLineInFile(fp, buffer);
	do 
	{
		char idbuffer[64];
		char valuebuffer[64];

		sscanf(buffer, "%s %s", idbuffer, valuebuffer);

		processHeader(std::string(idbuffer), std::string(valuebuffer));

		goToNextValidLineInFile(fp, buffer);

		loc = strstr(buffer, "SegmentNames");

	} while (loc == NULL);

	return true;
}

bool HTRLoader::processHeader(std::string identifier, std::string value)
{
	if (identifier == "FileType")
	{
		fileType = value;
		return true;
	}
	else if (identifier == "DataType")
	{
		dataType = value;
		return true;
	}
	else if (identifier == "FileVersion")
	{
		// remember fileVersion is an int
		// atoi converts a string to an int
		fileVersion = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "NumSegments")
	{
		numSegments = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "NumFrames")
	{
		numFrames = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "DataFrameRate")
	{
		fps = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "EulerRotationOrder")
	{
		if      (value == "XYZ") { rotationOrder = RotationOrder::XYZ; }
		else if (value == "ZYX") { rotationOrder = RotationOrder::ZYX; }

		else if (value == "XZY") { rotationOrder = RotationOrder::XZY; }
		else if (value == "YXZ") { rotationOrder = RotationOrder::YXZ; }
		else if (value == "YZX") { rotationOrder = RotationOrder::YZX; }
		else if (value == "ZXY") { rotationOrder = RotationOrder::ZXY; }

		else
		{
			std::cout << "HTR Parse Warning: File has unsupported rotation order of: " << value << std::endl;
		}

		return true;
		// put conditions for the other rotation orders here...
	}
	else if (identifier == "CalibrationUnits")
	{

		return true;
	}
	else if (identifier == "RotationUnits")
	{
		rotationUnits = value;
		return true;
	}
	else if (identifier == "GlobalAxisofGravity")
	{

		return true;
	}
	else if (identifier == "BoneLengthAxis")
	{

		return true;
	}
	else if (identifier == "ScaleFactor")
	{

		return true;
	}
	else
	{
		std::cout << "HTR Parse Error: Could not identify HEADER: " << identifier << " with value: " << value << std::endl;
		return false;
	}

}

bool HTRLoader::processSegementNameSection(FILE * fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process segment names
	// File currently on line with "SegmentNames"
	goToNextValidLineInFile(fp, buffer);
	do
	{
		// Allocate memory for values
		char childBuff[64];
		char parentBuff[64];

		// Parse values from current line 
		sscanf(buffer, "%s %s", childBuff, parentBuff);

		// Create new joint descriptor
		JointDescriptor jd;

		// Set joint descriptor values
		jd.jointName = childBuff;
		jd.jointParent = parentBuff;

		// Store the new joint
		jointDescriptors.push_back(jd);

		// Increment to next valid line
		goToNextValidLineInFile(fp, buffer);

		// Check if current line is the start of the next section
		// Remember strstr returns null if second param isn't found in first
		loc = strstr(buffer, "BasePosition");

	} while (loc == NULL);

	return true;
}

bool HTRLoader::processBasePositionSection(FILE* fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process base positions
	// File currently on line with "BasePosition"
	// increment to next line with actual data
	goToNextValidLineInFile(fp, buffer);
	do
	{
		char jointNameBuff[64];
		float tx, ty, tz;
		float rx, ry, rz;
		float boneLength;

		sscanf(buffer, "%s %f %f %f %f %f %f %f", jointNameBuff, &tx, &ty, &tz, &rx, &ry, &rz, &boneLength);

		// Find the joint descriptor by name
		JointDescriptor* jd = getJointDescriptorByName(jointNameBuff);

		// Make sure we got it
		if (!jd)
		{
			std::cout << "HTR Parse ERROR: Could not find JointDescriptor with name: " << std::string(jointNameBuff) << std::endl;
			return false;
		}

		// Store values
		jd->jointBasePosition = glm::vec3(tx, ty, tz);
		jd->jointBaseRotation = createQuaternion(rx, ry, rz);
		jd->boneLength = boneLength;

		// Increment to next valid line
		goToNextValidLineInFile(fp, buffer);

		// Check if current line is the start of the next section
		// Remember strstr returns null if second param isn't found in first
		loc = strstr(buffer, jointDescriptors[0].jointName.c_str());
	} while (loc == NULL);
	return true;
}

bool HTRLoader::processAnimationSection(FILE* fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process each joint's poses at each frame
	for (unsigned int i = 0; i < jointDescriptors.size(); i++)
	{
		goToNextValidLineInFile(fp, buffer);
		do
		{
			numFrames++;
			int frame;
			float tx, ty, tz;
			float rx, ry, rz;
			float scale;

			sscanf(buffer, "%d %f %f %f %f %f %f %f", &frame, &tx, &ty, &tz, &rx, &ry, &rz, &scale);

			// CODE HERE C //////////////////////////////////////////////////////////////////////////
			// Store values
			glm::vec3 newJointPos = glm::vec3(tx, ty, tz);
			glm::quat newJointRot = createQuaternion(rx, ry, rz);

			jointDescriptors[i].jointPositions.push_back(newJointPos);
			jointDescriptors[i].jointRotations.push_back(newJointRot);
			jointDescriptors[i].jointScales.push_back(scale);
			jointDescriptors[i].numFrames++;

			// Increment to next valid line
			goToNextValidLineInFile(fp, buffer);

			// Check if current line is the start of the next section
			// Remember strstr returns null if second param isn't found in first
			int nextJointIndex = i + 1;

			if ((unsigned)nextJointIndex < jointDescriptors.size())
				loc = strstr(buffer, jointDescriptors[nextJointIndex].jointName.c_str());
			else
			{
				loc = strstr(buffer, "EndOfFile");
				break;
			}
		} while (loc == NULL);
	}

	return true;
}

void HTRLoader::createGameObjects()
{
	// CODE HERE D //////////////////////////////////////////////////////////////////////////
	//create game objects
	for (unsigned int i = 0; i < jointDescriptors.size(); i++)
	{
		GameObject newJoint;

		newJoint.name = jointDescriptors[i].jointName;
		newJoint.jointAnimation = &jointDescriptors[i];

		jointGameObjects.push_back(newJoint);
	}
	//create hierarchicy
	for (unsigned int i = 0; i < jointDescriptors.size(); i++)
	{
		JointDescriptor* jd = &jointDescriptors[i];
		GameObject* child = getGameObjectByName(jd->jointName);
		GameObject* parent = getGameObjectByName(jd->jointParent);

		if (child == nullptr || parent == nullptr) { continue; }

		parent->addChild(child);
	}
}

GameObject* HTRLoader::getRootGameObject()
{
	if (rootGameObject == nullptr)
	{
		for (unsigned int i = 0; i < jointGameObjects.size(); i++)
		{
			if (jointGameObjects[i].jointAnimation->jointParent == "GLOBAL")
				rootGameObject = &jointGameObjects[i];
		}
	}

	return rootGameObject;
}


void HTRLoader::goToNextValidLineInFile(FILE* fp, char* buffer)
{
	fgets(buffer, MAX_LINE_LENGTH, fp);

	// Move stream to next valid line
	// Remember: fgets will split on tabs
	// Remember: '#' is comment symbol
	while (buffer[0] == '\t' || buffer[0] == '#')
	{ fgets(buffer, MAX_LINE_LENGTH, fp); }
}

glm::quat HTRLoader::createQuaternion(float rx, float ry, float rz)
{
	// If rotation units is degrees, convert to radians
	if (rotationUnits == "Degrees")
	{
		rx = glm::radians(rx);
		ry = glm::radians(ry);
		rz = glm::radians(rz);
	}

	// Construct unit quaternions for each axis
	glm::quat qX = glm::angleAxis(rx, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qY = glm::angleAxis(ry, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qZ = glm::angleAxis(rz, glm::vec3(0.0f, 0.0f, 1.0f));

	// Check for rotation order
	if      (rotationOrder == RotationOrder::XYZ) { return qX * qY * qZ; }
	else if (rotationOrder == RotationOrder::ZYX) { return qZ * qY * qX; }
	// ... put other rotation orders here ...
	else if (rotationOrder == RotationOrder::XZY) { return qX * qZ * qY; }
	else if (rotationOrder == RotationOrder::YXZ) { return qY * qX * qZ; }
	else if (rotationOrder == RotationOrder::YZX) { return qY * qZ * qX; }
	else if (rotationOrder == RotationOrder::ZXY) { return qZ * qX * qY; }

	else
	{
		std::cout << "HTR Parse Warning: File has unsupported rotation order" << std::endl;
	}

	// Return XYZ by default
	return qZ * qY * qX;
}

JointDescriptor* HTRLoader::getJointDescriptorByName(std::string jointName)
{
	int numJoints = jointDescriptors.size();

	for (int i = 0; i < numJoints; i++)
	{
		if (jointName == jointDescriptors[i].jointName)
			return &jointDescriptors[i];
	}

	return nullptr;
}

GameObject* HTRLoader::getGameObjectByName(std::string jointName)
{
	int numJoints = jointGameObjects.size();

	for (int i = 0; i < numJoints; i++)
	{
		if (jointName == jointGameObjects[i].name)
			return &jointGameObjects[i];
	}

	return nullptr;
}
