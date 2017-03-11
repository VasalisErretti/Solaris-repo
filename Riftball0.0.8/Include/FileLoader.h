#pragma once
// Core Libraries
#include <iostream>
#include <fstream>
#include <vector>

// 3rd Party Libraries
#include <glm\glm\glm.hpp>


class TextLoader
{
protected:


public:

	bool TextLoader::objectLoader(const char* filename)
	{
		FILE *file;
		errno_t errorCode = fopen_s(&file, filename, "r"); //opens the file // also check just incase

		std::vector<int> temp_Number;
		std::vector<glm::vec3> temp_Position;
		std::vector<glm::vec3> temp_Rotation;
		std::vector<glm::vec3> temp_Scale;

		//std::vector<std::vector<int>> m_Number;
		std::vector<std::vector<glm::vec3>> m_Position;
		std::vector<std::vector<glm::vec3>> m_Rotation;
		std::vector<std::vector<glm::vec3>> m_Scale;
		int amountOfObjects = 0;

		if (!file) //if the file can't be read
		{
			std::cout << "[FO.1] File not opened. [" << filename << "]\n"; return false;
		}
		else //if the file is read
		{
			std::cout << "[FO.2] File opened. [" << filename << "]\n";
			while (1)// this can't be 0
			{
				char lineHeader[64];
				lineHeader[0] = '\0';
				// read the first word of the line
				int res = fscanf_s(file, "%s", lineHeader, _countof(lineHeader));
				if (res == EOF) { break; } //when it reaches the end of the file, exit the while loop

				if (strcmp(lineHeader, "obj") == 0) {
					amountOfObjects += 1;
					//m_Number.push_back(temp_Number);
					m_Position.push_back(temp_Position);
					m_Rotation.push_back(temp_Rotation);
					m_Scale.push_back(temp_Scale);
					//std::cout << "[" << temp_Number.size() << "] ";
					//std::cout << "[" << amountOfObjects << "]\n" << std::endl;
				}
				else if (strcmp(lineHeader, "number") == 0) {
					int temp_num;
					fscanf_s(file, "%i\n", &temp_num);
					temp_Number.push_back(temp_num);
				}
				else if (strcmp(lineHeader, "pos") == 0) {
					glm::vec3 temp_pos;
					fscanf_s(file, "%f %f %f\n", &temp_pos.x, &temp_pos.y, &temp_pos.z);
					temp_Position.push_back(temp_pos);
				}
				else if (strcmp(lineHeader, "rot") == 0) {
					glm::vec3 temp_rot;
					fscanf_s(file, "%f %f %f\n", &temp_rot.x, &temp_rot.y, &temp_rot.z);
					temp_Rotation.push_back(temp_rot);
				}
				else if (strcmp(lineHeader, "siz") == 0) {
					glm::vec3 temp_siz;
					fscanf_s(file, "%f %f %f\n", &temp_siz.x, &temp_siz.y, &temp_siz.z);
					temp_Scale.push_back(temp_siz);
				}
				

			}//end of [while] loop
		}//end of [else] the file is open

		fclose(file);

		return true;
	}
	bool TextLoader::objectLoader(std::string filename) {}
};