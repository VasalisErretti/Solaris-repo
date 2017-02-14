#include "GameObject.h"
#include "MorphMath.h"




GameObject::GameObject() {
	m_Mass = 1.0f;
	m_InvertedMass = (1.0f / m_Mass);;
	m_Restitution = 1.0f; //not currently using 
	m_Drag = 0.010f;
	m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Size = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_ForceOnObject = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	Viewable = true;
}
GameObject::~GameObject() {
	glDeleteBuffers(1, &vertbo);
	glDeleteBuffers(1, &normbo);
	glDeleteBuffers(1, &texbo);
	glDeleteBuffers(1, &colorbo);
	glDeleteBuffers(1, &vao);
}


/* function update()
* Description:
*   - this is called when updating the positions of objects in the program
*/
void GameObject::update(float deltaT)
{
	if (Viewable == true) {
		if (inShock) {
			TimeInShock += deltaT;
			if (TimeInShock > 3.0f) {
				TimeInShock = 0.0f;
				inShock = false;
			}
		}

		float TopSpeed = 4.0f;
		if (m_Mass > 0.0f) {
			if (((m_Velocity.x < TopSpeed) && (m_Velocity.y < TopSpeed) && (m_Velocity.z < TopSpeed))
				&& ((m_Velocity.x > -TopSpeed) && (m_Velocity.y > -TopSpeed) && (m_Velocity.z > -TopSpeed)))
			{
				m_Acceleration = (m_ForceOnObject / m_Mass);//update acceleration based on external force
				//if a force is applyed to the object //update velocity based on acceleration
				if (m_Acceleration != glm::vec3(0.0f)) { m_Velocity += ((m_Acceleration * deltaT)*0.5f); }

				//if no force is applyed to the object
				else { m_Velocity -= (m_Velocity * m_Drag); }

				//facing a direction
				if (m_Velocity.x != (0.0f) || m_Velocity.z != (0.0f)) {
				//if (m_Velocity.x <= -0.01f || m_Velocity.x >= 0.01f || m_Velocity.z <= -0.01f || m_Velocity.z >= 0.01f) {
					ForwardDirection = (m_Position - (m_Position + m_Velocity));
					FaceYRotation = atan2(ForwardDirection.z, -ForwardDirection.x);
					this->setRotation(glm::vec3(0.0f, FaceYRotation*(180.0f / 3.14159f), 0.0f));
				}
				//update position based on velocity
				m_Position += m_Velocity;
			}
			else if (m_Velocity.x >  TopSpeed) { m_Velocity.x =  (TopSpeed*0.95); }
			else if (m_Velocity.x < -TopSpeed) { m_Velocity.x = -(TopSpeed*0.95); }
			else if (m_Velocity.y >  TopSpeed) { m_Velocity.y =  (TopSpeed*0.95); }
			else if (m_Velocity.y < -TopSpeed) { m_Velocity.y = -(TopSpeed*0.95); }
			else if (m_Velocity.z >  TopSpeed) { m_Velocity.z =  (TopSpeed*0.95); }
			else if (m_Velocity.z < -TopSpeed) { m_Velocity.z = -(TopSpeed*0.95); }

			else if (m_Velocity.x == NULL) { m_Velocity.x = 0.0f; }
			else if (m_Velocity.y == NULL) { m_Velocity.y = 0.0f; }
			else if (m_Velocity.z == NULL) { m_Velocity.z = 0.0f; }
		}
		else {
			m_Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);//update acceleration based on external force
			m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);//update velocity based on acceleration
			m_Position += (m_Velocity * deltaT);//update position based on velocity
		}
		m_ForceOnObject = glm::vec3(0.0f, 0.0f, 0.0f);//zero out the old force

		//if (!inAir) { m_Position.y = m_Radius.y; }
	}
	else if (Viewable == false) {
		m_ForceOnObject = glm::vec3(0.0f, 0.0f, 0.0f);//zero out the old force
		m_Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);//update acceleration based on external force
		m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);//update velocity based on acceleration
	}
	else {}
}
void GameObject::updateP(float deltaT)
{
	if (Viewable == true) {
		if (inShock) {
			TimeInShock += deltaT;
			if (TimeInShock > 3.0f) {
				TimeInShock = 0.0f;
				inShock = false;
			}
		}

		float TopSpeed = 4.0f;
		if (m_Mass > 0.0f) {
			if (((m_Velocity.x < TopSpeed) && (m_Velocity.y < TopSpeed) && (m_Velocity.z < TopSpeed))
				&& ((m_Velocity.x > -TopSpeed) && (m_Velocity.y > -TopSpeed) && (m_Velocity.z > -TopSpeed)))
			{
				m_Acceleration = (m_ForceOnObject / m_Mass);//update acceleration based on external force
				//if a force is applyed to the object //update velocity based on acceleration
				if (m_Acceleration != glm::vec3(0.0f)) { m_Velocity += ((m_Acceleration * deltaT)*0.5f); }
				//if no force is applyed to the object
				else { m_Velocity -= (m_Velocity * m_Drag); }
				//facing a direction
				if (m_Velocity.x <= -0.01f || m_Velocity.x >= 0.01f || m_Velocity.z <= -0.01f || m_Velocity.z >= 0.01f) {
					//ForwardDirection = (m_Position - (m_Position + m_Velocity));
					FaceYRotation = atan2(-ForwardDirection.z, ForwardDirection.x);
					if (!IsJumping) { this->setRotation(glm::vec3(0.0f, FaceYRotation*(180.0f / 3.14159f), 0.0f)); }
					if (IsJumping && FaceYRotation != 0.0f) { this->setRotation(glm::vec3(0.0f, FaceYRotation*(180.0f / 3.14159f), 0.0f)); }
				}
				//update position based on velocity
				m_Position += (m_Velocity + (((m_ForceOnObject / m_Mass))*1.5f));
			}
			else if (m_Velocity.x >  TopSpeed) { m_Velocity.x = (TopSpeed*0.95); }
			else if (m_Velocity.x < -TopSpeed) { m_Velocity.x = -(TopSpeed*0.95); }
			else if (m_Velocity.y >  TopSpeed) { m_Velocity.y = (TopSpeed*0.95); }
			else if (m_Velocity.y < -TopSpeed) { m_Velocity.y = -(TopSpeed*0.95); }
			else if (m_Velocity.z >  TopSpeed) { m_Velocity.z = (TopSpeed*0.95); }
			else if (m_Velocity.z < -TopSpeed) { m_Velocity.z = -(TopSpeed*0.95); }

			else if (m_Velocity.x == NULL) { m_Velocity.x = 0.0f; }
			else if (m_Velocity.y == NULL) { m_Velocity.y = 0.0f; }
			else if (m_Velocity.z == NULL) { m_Velocity.z = 0.0f; }
		}
		m_ForceOnObject = glm::vec3(0.0f, 0.0f, 0.0f);//zero out the old force

		if (!inAir && !onObject) { m_Position.y = m_Radius.y; }
	}
	else if (Viewable == false) {
		m_Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);//update acceleration based on external force
		m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);//update velocity based on acceleration
												 //m_Position = glm::vec3(-1000.0f, 0.0f, -1000.0f);
	}
	else {}
}
/* function drawObject()
* Description:
*   - this is called when drawing an .obj file into the program
*/
void GameObject::drawObject(Shader *s)
{
	if (Viewable) {
		// bind tex here if you had one
		glBindTexture(GL_TEXTURE_2D, textureHandle);
		
		// compute local transformation
		glm::mat4x4 scaleMatrix = glm::scale(m_Scale);
		glm::mat4x4 rotationMatrix = m_Rotation;
		glm::mat4x4 translationMatrix = glm::translate(m_Position);
		//glm::mat4x4 rotationX = glm::rotate(m_Angle.x, glm::vec3(1.0, 0.0, 0.0));
		//glm::mat4x4 rotationY = glm::rotate(m_Angle.y, glm::vec3(0.0, 1.0, 0.0));
		//glm::mat4x4 rotationZ = glm::rotate(m_Angle.z, glm::vec3(0.0, 0.0, 1.0));
		//glm::mat4x4 rotationMatrix = (rotationZ * rotationY * rotationX);


		//first [scale] then [rotate] then [translate]
		glm::mat4x4 transform = (translationMatrix * rotationMatrix * scaleMatrix);
		s->uniformMat4x4("localTransform", &transform);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, numtris);
		glBindVertexArray(0);
	}
}
/* function objectLoader()
* Description:
*   - this is called when loading an .obj file into the program
*/
bool GameObject::objectLoader(const char* filePath)
{
	FILE *file;
	errno_t errorCode = fopen_s(&file, filePath, "r"); //opens the file // also check just incase

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords; //note: vec2
	std::vector<Face> faces;

	if (!file) //if the file can't be read
	{
		std::cout <<"[FO.1] File not opened. [" << filePath << "]\n"; return false;
	}
	else //if the file is read
	{
		std::cout << "[FO.2] File opened. [" << filePath << "]\n";
		while (1)// this can't be 0
		{
			char lineHeader[64];
			// read the first word of the line
			int res = fscanf_s(file, "%s", lineHeader, _countof(lineHeader));
			if (res == EOF) { break; } //when it reaches the end of the file, exit the while loop

			if (strcmp(lineHeader, "v") == 0) //when it finds a [v] or vertex, it will look for the next three float numbers
			{
				glm::vec3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				vertices.push_back(vertex); //saves the data for the vertex into a temp_variable to later be bound to it's proper index
			}
			else if (strcmp(lineHeader, "vt") == 0) //when it finds a [vt] or uv, it will look for the next two float numbers
			{
				glm::vec2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				texcoords.push_back(uv); //saves the data for the uvs into a temp_variable to later be bound to it's proper index
			}
			else if (strcmp(lineHeader, "vn") == 0) //when it finds a [vn] or normal, it will look for the next three float numbers
			{
				glm::vec3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				normals.push_back(normal); //saves the data for the normals into a temp_variable to later be bound to it's proper index
			}
			else if (strcmp(lineHeader, "f") == 0) //when it finds a [f] or face, it will look for the next nine integer numbers
			{
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				// saves the data for the faces to be used to pair the vertex,uvs,normals,etc, with the correct index
				unsigned int facesIndex = fscanf_s(file, "%u/%u/%u %u/%u/%u %u/%u/%u\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (facesIndex != 9) //if the file does not have vertexs, uvs, or normals. 
				{
					std::cout << "File can't be read correctly. [" << facesIndex << "] \n"; return false;
				}
				if (facesIndex == 8 || facesIndex == 6)
				{
					std::cout << "Obj file was not of triangles, or is missing something. [" << facesIndex << "] \n"; return false;
				}
				// rearranges the variables that were just inputed from the file to there correct index
				Face temp;

				temp.vertices[0] = vertexIndex[0] - 1;
				temp.vertices[1] = vertexIndex[1] - 1;
				temp.vertices[2] = vertexIndex[2] - 1;
				temp.textures[0] = uvIndex[0]	  - 1;
				temp.textures[1] = uvIndex[1]	  - 1;
				temp.textures[2] = uvIndex[2]	  - 1;
				temp.normals[0]  = normalIndex[0] - 1;
				temp.normals[1]  = normalIndex[1] - 1;
				temp.normals[2]  = normalIndex[2] - 1;

				faces.push_back(temp);
			}
		}//end of [while] loop
		//return true; //fuck you 
	}//end of [else] the file is open

	fclose(file);

	//convert obj to interleaved vertex array
	numtris = faces.size() * 3;

	this->verts = new float[numtris * 3];
	this->norms = new float[numtris * 3];
	this->texs = new float[numtris * 2];

	for (unsigned long i = 0; i < faces.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			this->verts[i * 9 + j * 3 + 0] = vertices[faces[i].vertices[j]].x;
			this->verts[i * 9 + j * 3 + 1] = vertices[faces[i].vertices[j]].y;
			this->verts[i * 9 + j * 3 + 2] = vertices[faces[i].vertices[j]].z;

			this->texs[i * 6 + j * 2 + 0] = texcoords[faces[i].textures[j]].x;
			this->texs[i * 6 + j * 2 + 1] = texcoords[faces[i].textures[j]].y;

			this->norms[i * 9 + j * 3 + 0] = normals[faces[i].normals[j]].x;
			this->norms[i * 9 + j * 3 + 1] = normals[faces[i].normals[j]].y;
			this->norms[i * 9 + j * 3 + 2] = normals[faces[i].normals[j]].z;
		}
	}

	this->colors = new float[numtris * 3];
	for (unsigned int i = 0; i < numtris * 3; i++)
	{
		colors[i] = norms[i] + 1.0 / 2.0;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// verts tho
	glGenBuffers(1, &vertbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // position/vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// textures tho(poopybutt)
	glGenBuffers(1, &texbo);
	glBindBuffer(GL_ARRAY_BUFFER, texbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 2, this->texs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); // texcoords/uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// normals tho
	glGenBuffers(1, &normbo);
	glBindBuffer(GL_ARRAY_BUFFER, normbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->norms, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2); //  normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//color tho
	glGenBuffers(1, &colorbo);
	glBindBuffer(GL_ARRAY_BUFFER, colorbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3); //  colour
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);




	//// verts tho
	//glGenBuffers(1, &vertbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vertbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->verts, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(4); // position/vertices
	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//// textures tho(poopybutt)
	//glGenBuffers(1, &texbo);
	//glBindBuffer(GL_ARRAY_BUFFER, texbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 2, this->texs, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(5); // texcoords/uv
	//glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//// normals tho
	//glGenBuffers(1, &normbo);
	//glBindBuffer(GL_ARRAY_BUFFER, normbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->norms, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(6); //  normals
	//glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	////color tho
	//glGenBuffers(1, &colorbo);
	//glBindBuffer(GL_ARRAY_BUFFER, colorbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->colors, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(7); //  colour
	//glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);




	glBindVertexArray(0);


	delete[] this->texs;
	delete[] this->norms;
	delete[] this->verts;
	delete[] this->colors;

	return true;
}
bool GameObject::objectLoader(std::string filePath1)
{
	const char* filePath = filePath1.c_str();
	FILE *file;
	errno_t errorCode = fopen_s(&file, filePath, "r"); //opens the file // also check just incase

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords; //note: vec2
	std::vector<Face> faces;

	if (!file) //if the file can't be read
	{
		std::cout << "[FO.1] File not opened. [" << filePath << "]\n"; return false;
	}
	else //if the file is read
	{
		std::cout << "[FO.2] File opened. [" << filePath << "]\n";
		while (1)// this can't be 0
		{
			char lineHeader[64];
			// read the first word of the line
			int res = fscanf_s(file, "%s", lineHeader, _countof(lineHeader));
			if (res == EOF) { break; } //when it reaches the end of the file, exit the while loop

			if (strcmp(lineHeader, "v") == 0) //when it finds a [v] or vertex, it will look for the next three float numbers
			{
				glm::vec3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				vertices.push_back(vertex); //saves the data for the vertex into a temp_variable to later be bound to it's proper index
			}
			else if (strcmp(lineHeader, "vt") == 0) //when it finds a [vt] or uv, it will look for the next two float numbers
			{
				glm::vec2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				texcoords.push_back(uv); //saves the data for the uvs into a temp_variable to later be bound to it's proper index
			}
			else if (strcmp(lineHeader, "vn") == 0) //when it finds a [vn] or normal, it will look for the next three float numbers
			{
				glm::vec3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				normals.push_back(normal); //saves the data for the normals into a temp_variable to later be bound to it's proper index
			}
			else if (strcmp(lineHeader, "f") == 0) //when it finds a [f] or face, it will look for the next nine integer numbers
			{
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				// saves the data for the faces to be used to pair the vertex,uvs,normals,etc, with the correct index
				unsigned int facesIndex = fscanf_s(file, "%u/%u/%u %u/%u/%u %u/%u/%u\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (facesIndex != 9) //if the file does not have vertexs, uvs, or normals. 
				{
					std::cout << "File can't be read correctly. [" << facesIndex << "] \n"; return false;
				}
				if (facesIndex == 8 || facesIndex == 6)
				{
					std::cout << "Obj file was not of triangles, or is missing something. [" << facesIndex << "] \n"; return false;
				}
				// rearranges the variables that were just inputed from the file to there correct index
				Face temp;

				temp.vertices[0] = vertexIndex[0] - 1;
				temp.vertices[1] = vertexIndex[1] - 1;
				temp.vertices[2] = vertexIndex[2] - 1;
				temp.textures[0] = uvIndex[0] - 1;
				temp.textures[1] = uvIndex[1] - 1;
				temp.textures[2] = uvIndex[2] - 1;
				temp.normals[0] = normalIndex[0] - 1;
				temp.normals[1] = normalIndex[1] - 1;
				temp.normals[2] = normalIndex[2] - 1;

				faces.push_back(temp);
			}
		}//end of [while] loop
		 //return true; //fuck you 
	}//end of [else] the file is open

	fclose(file);

	//convert obj to interleaved vertex array
	numtris = faces.size() * 3;

	this->verts = new float[numtris * 3];
	this->norms = new float[numtris * 3];
	this->texs = new float[numtris * 2];

	for (unsigned long i = 0; i < faces.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			this->verts[i * 9 + j * 3 + 0] = vertices[faces[i].vertices[j]].x;
			this->verts[i * 9 + j * 3 + 1] = vertices[faces[i].vertices[j]].y;
			this->verts[i * 9 + j * 3 + 2] = vertices[faces[i].vertices[j]].z;

			this->texs[i * 6 + j * 2 + 0] = texcoords[faces[i].textures[j]].x;
			this->texs[i * 6 + j * 2 + 1] = texcoords[faces[i].textures[j]].y;

			this->norms[i * 9 + j * 3 + 0] = normals[faces[i].normals[j]].x;
			this->norms[i * 9 + j * 3 + 1] = normals[faces[i].normals[j]].y;
			this->norms[i * 9 + j * 3 + 2] = normals[faces[i].normals[j]].z;
		}
	}

	this->colors = new float[numtris * 3];
	for (unsigned int i = 0; i < numtris * 3; i++)
	{
		colors[i] = norms[i] + 1.0 / 2.0;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// verts tho
	glGenBuffers(1, &vertbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // position/vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// textures tho(poopybutt)
	glGenBuffers(1, &texbo);
	glBindBuffer(GL_ARRAY_BUFFER, texbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 2, this->texs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); // texcoords/uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// normals tho
	glGenBuffers(1, &normbo);
	glBindBuffer(GL_ARRAY_BUFFER, normbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->norms, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2); //  normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//color tho
	glGenBuffers(1, &colorbo);
	glBindBuffer(GL_ARRAY_BUFFER, colorbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3); //  colour
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//// verts tho
	//glGenBuffers(1, &vertbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vertbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->verts, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(4); // position/vertices
	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//// textures tho(poopybutt)
	//glGenBuffers(1, &texbo);
	//glBindBuffer(GL_ARRAY_BUFFER, texbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 2, this->texs, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(5); // texcoords/uv
	//glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//// normals tho
	//glGenBuffers(1, &normbo);
	//glBindBuffer(GL_ARRAY_BUFFER, normbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->norms, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(6); //  normals
	//glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	////color tho
	//glGenBuffers(1, &colorbo);
	//glBindBuffer(GL_ARRAY_BUFFER, colorbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->colors, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(7); //  normals
	//glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);




	glBindVertexArray(0);


	delete[] this->texs;
	delete[] this->norms;
	delete[] this->verts;
	delete[] this->colors;

	return true;
}
void GameObject::objectLoader(GameObject * objPath)
{
	//Modern openGL Object Loader///////////
	vao = objPath->vao;
	vertbo = objPath->vertbo;
	normbo = objPath->normbo;
	texbo = objPath->texbo;
	colorbo = objPath->colorbo;
	numtris = objPath->numtris;

	textureHandle = objPath->textureHandle;
	//variables///////////
	Viewable = objPath->Viewable;
	textureHandle_hasTransparency = objPath->textureHandle_hasTransparency;
	m_Position = objPath->m_Position;
	m_Rotation = objPath->m_Rotation;
	m_Scale = objPath->m_Scale;
	m_Top = objPath->m_Top;
	m_Bottom = objPath->m_Bottom;
	m_Radius = objPath->m_Radius;
	m_Size = objPath->m_Size;
	m_Colour = objPath->m_Colour;
	m_Mass = objPath->m_Mass;
	m_InvertedMass = objPath->m_InvertedMass;
	m_Velocity = objPath->m_Velocity;
	m_Acceleration = objPath->m_Acceleration;
	m_ForceOnObject = objPath->m_ForceOnObject;
	m_Restitution = objPath->m_Restitution;
	m_Drag = objPath->m_Drag;
}
void GameObject::objectHitBox(GameObject * objPath) {

}

void GameObject::objectLoaderHTR(GameObject * objPath)
{
	vao = objPath->vao;
	vertbo = objPath->vertbo;
	normbo = objPath->normbo;
	texbo = objPath->texbo;
	colorbo = objPath->colorbo;
	numtris = objPath->numtris;

	Viewable = objPath->Viewable;
	textureHandle = objPath->textureHandle;
	textureHandle_hasTransparency = objPath->textureHandle_hasTransparency;
}

void GameObject::morphTarget(GameObject * objPath, float dt) {
	//MorphMath hello;
	////this once worked but now that it is in [Modern openGL] it broke.
	//for (int i = 0; i < sizeof(verts); i++) {
	//	this->verts = hello.Lerp(this->verts, objPath->verts, dt);
	//}
	//for (int i = 0; i < sizeof(norms); i++) {
	//	this->norms = hello.Lerp(this->norms, objPath->norms, dt);
	//}
	//for (int i = 0; i < sizeof(texs); i++) {
	//	this->texs = hello.Lerp(this->texs, objPath->texs, dt);
	//}
}



void GameObject::updateHTR(float dt)
{
	// Create 4x4 transformation matrix

	// If there is no animation for this joint, create the transform matrix as usual
	if (jointAnimation == nullptr){}
	else // Transform comes from HTR
	{
		m_Rotation = glm::mat4_cast(jointAnimation->jointBaseRotation * jointAnimation->jointRotations[m_CurrentFrame]);

		m_Position = jointAnimation->jointBasePosition + jointAnimation->jointPositions[m_CurrentFrame];

		jointAnimation->jointScales[m_CurrentFrame];

		// Increment frame (note: you could do this based on dt)
		m_CurrentFrame++;

		if (m_CurrentFrame >= jointAnimation->numFrames) { m_CurrentFrame = 0; }

	}

	// Create translation matrix
	glm::mat4 tran = glm::translate(m_Position);

	// Create scale matrix
	glm::mat4 scal = glm::scale(glm::vec3(m_Scale));

	// Combine all above transforms into a single matrix
	// This is the local transformation matrix, ie. where is this game object relative to it's parent
	// If a game object has no parent (it is a root node) then its local transform is also it's global transform
	// If a game object has a transform, then we must apply the parent's transform
	m_LocalTransformMatrix = tran * m_Rotation * scal;

	if (m_Parent) { m_LocalToWorldMatrix = m_Parent->getLocalToWorldMatrix() * m_LocalTransformMatrix; }
	else { m_LocalToWorldMatrix = m_LocalTransformMatrix; }

	// Update children
	for (unsigned int i = 0; i < m_Children.size(); i++) { m_Children[i]->updateHTR(dt); }
}

void GameObject::drawHTR(Shader *s)
{
	//// Draw node
	//TTK::Graphics::DrawSphere(m_LocalToWorldMatrix, 0.5f, colour);
	//// Draw line to parent 
	//if (m_Parent && !m_Parent->isRoot()){
	//	glm::vec3 parentWorldPosition = m_Parent->getWorldPosition();
	//	glm::vec3 myWorldPositon = getWorldPosition();
	//	TTK::Graphics::DrawLine(myWorldPositon, parentWorldPosition, 5.0f);
	//}


	if (Viewable) {
		// bind tex here if you had one
		glBindTexture(GL_TEXTURE_2D, textureHandle);

		// compute local transformation
		glm::mat4x4 scaleMatrix = glm::scale(m_Scale);
		glm::mat4x4 rotationMatrix = m_Rotation;
		glm::mat4x4 translationMatrix = glm::translate(m_Position);


		glm::mat4x4 transform = translationMatrix * rotationMatrix * scaleMatrix;
		s->uniformMat4x4("localTransform", &transform);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, numtris);
		glBindVertexArray(0);
	}

	// Draw children
	for (unsigned int i = 0; i < m_Children.size(); ++i) {
		m_Children[i]->objectLoaderHTR(this);
		m_Children[i]->drawHTR(s); 
	}

}