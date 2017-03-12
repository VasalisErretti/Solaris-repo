#pragma once

#include <iostream>
#include <math.h>
#include <string>

class MorphMath {
public:
	struct pointIS
	{
		float x;
		float y;
		float z;
	};

	// Linear interpolation
	template <typename T>
	T Lerp(T p1, T p2, float t)
	{
		//modifier for smoothstep
		//t = (t*t) * (3.0f - 2.0f*t);
		//modifier for Extra smoothstep
		//t = (t*t*t) * ((t * (5.0f*t - 10.0f)) + 10.0f);
		T temp = static_cast<T>(((1.0f - t) * p1) + (p2 * t));
		return temp;
	}

	template <typename T>
	T Lerp(T p1, T p2, float s, float t)
	{
		float distance = glm::length(p2 -p1);
		if (distance != 0.0f) {
			T direction = glm::normalize(p2 - p1);
			return (direction * s * t);
		}
		return (p1 * p2);
	}

	// inverse lerp
	template <typename T>
	float invLerp(T p0, T p1, T p2)
	{
		return ((p0 - p1) / (p2 - p1));
	}

	//bezier
	template <typename T>
	T Bezier4(T p1, T p2, T p3, T p4, float t) {
		return Lerp(Lerp(p1, p2, t), Lerp(p3, p4, t), t);
	}

	//catmull-rom
	template <typename T>
	T CatmullRoll(T p1, T p2, T p3, T p4, float t )
	{
		float t2 = t*t; float t3 = t*t*t;
		return (((-t3 + 2 * t2 - t)*(p1)+(3 * t3 - 5 * t2 + 2)*(p2)+(-3 * t3 + 4 * t2 + t)*(p3)+(t3 - t2)*(p4)) * 0.5);
	}


	class PointHandle
	{
	public:
		PointHandle(float _pointSize, glm::vec3 _position, std::string _label = "") {
			pointSize = _pointSize;
			position = _position;
			label = _label;
		}
		glm::vec3 position;
		std::string label;
		float pointSize;
	};
	struct pointHandlers {
		std::vector<PointHandle> pointHandles; // manipulators
	};
	std::vector<pointHandlers> PointHandlerRef;

	struct lineSegments {
		std::vector< glm::vec3 > lineSegmentVertex;
	};
	std::vector<lineSegments> LSV;

	bool loadMorphSegments(const char * filePath)
	{
		std::vector<PointHandle> pointHandlesLineSegment;
		std::vector< glm::vec3 > lineSegmentVertex;
		std::vector< glm::vec3 > temp_vertices;

		FILE *file;
		errno_t errorCode = fopen_s(&file, filePath, "r"); //opens the file // also check just incase

		if (!file) //if the file can't be read
		{
			std::cout << "[FO.1] File not opened. [" << filePath << "] \n";
			return false;
		}
		else //if the file is read
		{
			std::cout << "[FO.2] File opened. [" << filePath << "]\n";
			unsigned int NoOF = 0; //number of objects found
			while (1)// this can't be 0
			{
				char lineHeader[64];
				lineHeader[0] = '\0';
				// read the first word of the line
				int res = fscanf_s(file, "%s", lineHeader, _countof(lineHeader));
				if (res == EOF) { break; } //when it reaches the end of the file, exit the while loop

				if (strcmp(lineHeader, "obj") == 0)
				{
					NoOF += 1;
					if (NoOF != 1)
					{
						for (unsigned int i = 0; i < temp_vertices.size(); i++)
						{
							glm::vec3 vertex = temp_vertices[i];
							lineSegmentVertex.push_back(vertex);
						}
						lineSegments a = { lineSegmentVertex };
						LSV.push_back(a);
						temp_vertices.clear();
						lineSegmentVertex.clear();
					}
					else {}
				}
				else if (strcmp(lineHeader, "vc") == 0) //when it finds a [vt] or uv, it will look for the next two float numbers
				{
					glm::vec3 vertex;
					fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					temp_vertices.push_back(vertex); //saves the data for the vertex into a temp_variable to later be bound to it's proper index
				}
			}//end of [while] loop
			for (unsigned int i = 0; i < temp_vertices.size(); i++)
			{
				glm::vec3 vertex = temp_vertices[i];
				lineSegmentVertex.push_back(vertex);
				PointHandle newHandle(25.0f, vertex, std::to_string(i));
				pointHandlesLineSegment.push_back(newHandle);
			}
			lineSegments a = { lineSegmentVertex };
			LSV.push_back(a);
			temp_vertices.clear();
			lineSegmentVertex.clear();

			pointHandlers b = { pointHandlesLineSegment };
			PointHandlerRef.push_back(b);
			pointHandlesLineSegment.clear();


		}//end of [else] the file is open
		fclose(file);
		return true;
	}
};