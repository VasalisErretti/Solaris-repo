#include <GL\glew.h>
#include "GameObject.h"

#include <string>
#include <iomanip> //setfill //setw
#include <map> //text
#include <ft2build.h> //text
#include FT_FREETYPE_H //text

/* class Manifold
* Description:
*   - this is use to compaire two game objects
*/
class Manifold
{
public:
	Manifold() {}
	~Manifold() {}

	//GameObject A, B; //holds two objects
	std::shared_ptr<GameObject> A, B; //holds two objects
	glm::vec3 Normal; //normal between two objects
	bool AreColliding; // didn't really use
};

/* class PhysicsMath
* Description:
*   - this is use for math
*/
class PhysicsMath
{
public:
	PhysicsMath() {}
	~PhysicsMath() {}
	//calculate dot product 
	float DotProduct(glm::vec3 a, glm::vec3 b)
	{
		return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z));
	}
	//calculate distance
	float Distance(glm::vec3 a, glm::vec3 b)
	{
		return hypotf(hypotf(a.x - b.x, a.y - b.y), a.z - b.z);
	}
	//calculate normal between two objects
	glm::vec3 GetNormal(glm::vec3 a, glm::vec3 b)
	{
		glm::vec3 ret = (b - a);
		glNormal3f(ret.x, ret.y, ret.z);
		return ret;
	}
};

/* class Sliders
* Description:
*   - this is use to create sliders
*/
class Sliders
{
public:
	Sliders() {
		SBar_Top = glm::vec3(0.0f);
		SBar_Bot = glm::vec3(0.0f);
		SBar_Rad = glm::vec3(0.0f);
		SBar_Pos = glm::vec3(0.0f);

		SNob_Top = glm::vec3(0.0f);
		SNob_Bot = glm::vec3(0.0f);
		SNob_Rad = glm::vec3(0.0f);
		SNob_Pos = glm::vec3(0.0f);

		SBar_Length = glm::vec3(0.0f);
		SNob_Length = glm::vec3(0.0f);
		SNob_Precent = glm::vec3(0.0f);
		YaxisSlider = false;
	}
	~Sliders() {}

	glm::vec3 SBar_Top;	 //Top XYZ of the bar
	glm::vec3 SBar_Bot;	 //Bottom XYZ of the bar
	glm::vec3 SBar_Rad;	 //Radius of the bar
	glm::vec3 SBar_Pos;	 //Position of the bar

	glm::vec3 SNob_Top;	 //Top XYZ of the nob
	glm::vec3 SNob_Bot;	 //Bottom XYZ of the nob
	glm::vec3 SNob_Rad;	 //Radius of the nob
	glm::vec3 SNob_Pos;	 //Position of the nob

	glm::vec3 SBar_Length;	//Length of the bar (the middle of the bar that the nob slides along)
	glm::vec3 SNob_Length;	//Length from the bottom of the bar to the nob
	glm::vec3 SNob_Precent;	//001.0f to 100.0f precent of the nob along the bar

	bool YaxisSlider;

	bool moveNob(float MPosToOPosX, float MPosToOPosY) {
		//if the slider is going side to side
		if ((!YaxisSlider) && (MPosToOPosX >= SBar_Bot.x + (SNob_Rad.x*1.0f)) && (MPosToOPosX <= SBar_Top.x - (SNob_Rad.x*1.0f))) {
			if ((MPosToOPosY >= SBar_Bot.z) && (MPosToOPosY <= SBar_Top.z)) {
				//setting the nob position to the mouse position along the X-axis
				SNob_Pos.x = MPosToOPosX;
				//length from the bottom of the bar to the nob
				SNob_Length = (SNob_Pos - (SBar_Bot + SNob_Rad));
				//declaring precent of the nob along the bar
				SNob_Precent = (SNob_Length / SBar_Length)*102.0f;

				//stating the maximum and minimum precent
				if (SNob_Precent.x < 1.0f) { SNob_Precent.x = 1.0f; }
				else if (SNob_Precent.x > 100.0f) { SNob_Precent.x = 100.0f; }
				if (SNob_Precent.y < 1.0f) { SNob_Precent.y = 1.0f; }
				else if (SNob_Precent.y > 100.0f) { SNob_Precent.y = 100.0f; }
				if (SNob_Precent.z < 1.0f) { SNob_Precent.z = 1.0f; }
				else if (SNob_Precent.z > 100.0f) { SNob_Precent.z = 100.0f; }

				std::cout << "[" << SNob_Precent.x << "]" << std::endl;
				return true;
			}
		}
		//if the slider is going up and down
		else if ((YaxisSlider) && (MPosToOPosY >= SBar_Bot.z + (SNob_Rad.z*1.0f)) && (MPosToOPosY <= SBar_Top.z - (SNob_Rad.z*1.0f))) {
			if ((MPosToOPosX >= SBar_Bot.x) && (MPosToOPosX <= SBar_Top.x)) {
				//setting the nob position to the mouse position along the Y-axis
				SNob_Pos.z = MPosToOPosY;
				//length from the bottom of the bar to the nob
				SNob_Length = (SNob_Pos - (SBar_Bot + SNob_Rad));
				//declaring precent of the nob along the bar
				SNob_Precent = (SNob_Length / SBar_Length)*102.0f;

				//stating the maximum and minimum precent
				if (SNob_Precent.x < 1.0f) { SNob_Precent.x = 1.0f; }
				else if (SNob_Precent.x > 100.0f) { SNob_Precent.x = 100.0f; }
				if (SNob_Precent.y < 1.0f) { SNob_Precent.y = 1.0f; }
				else if (SNob_Precent.y > 100.0f) { SNob_Precent.y = 100.0f; }
				if (SNob_Precent.z < 1.0f) { SNob_Precent.z = 1.0f; }
				else if (SNob_Precent.z > 100.0f) { SNob_Precent.z = 100.0f; }

				std::cout << "[" << SNob_Precent.z << "]" << std::endl;
				return true;
			}
		}

		return false;
	}
	void copySlider(Sliders *SliderPath) {
		SBar_Top = SliderPath-> SBar_Top;
		SBar_Bot = SliderPath-> SBar_Bot;
		SBar_Rad = SliderPath-> SBar_Rad;
		SBar_Pos = SliderPath-> SBar_Pos;
							    
		SNob_Top = SliderPath-> SNob_Top;
		SNob_Bot = SliderPath-> SNob_Bot;
		SNob_Rad = SliderPath-> SNob_Rad;
		SNob_Pos = SliderPath-> SNob_Pos;

		SBar_Length = SliderPath-> SBar_Length;
		SNob_Length = SliderPath-> SNob_Length;
		SNob_Precent = SliderPath-> SNob_Precent;

		YaxisSlider = SliderPath-> YaxisSlider;
	}

};
/* class Buttons
* Description:
*   - this is use to create buttons
*/
class Buttons
{
public:
	Buttons() {
		SBut_Top = glm::vec3(0.0f);
		SBut_Bot = glm::vec3(0.0f);
		SBut_Rad = glm::vec3(0.0f);
		SBut_Pos = glm::vec3(0.0f);
	}
	~Buttons() {}

	glm::vec3 SBut_Top;	 //Top XYZ of the button
	glm::vec3 SBut_Bot;	 //Bottom XYZ of the button
	glm::vec3 SBut_Rad;	 //Radius of the button
	glm::vec3 SBut_Pos;	 //Position of the button

	void copyButton(Buttons *SliderPath) {
		SBut_Top = SliderPath->SBut_Top;
		SBut_Bot = SliderPath->SBut_Bot;
		SBut_Rad = SliderPath->SBut_Rad;
		SBut_Pos = SliderPath->SBut_Pos;
	}

	bool button(float MPosToOPosX, float MPosToOPosY) {
		//if the button is going side to side
		if ((MPosToOPosX >= SBut_Bot.x) && (MPosToOPosX <= SBut_Top.x)) {
			if ((MPosToOPosY >= SBut_Bot.z) && (MPosToOPosY <= SBut_Top.z)) {
				std::cout << "[Hit]" << std::endl;
				return true;
			}
		}
		return false;
	}
};

/* struct Character
* Description:
*   - this is use to store character fonts
*/
struct Character {
	GLuint TextureID;	//ID handle of the font
	glm::ivec2 Size;	//Size of font
	glm::ivec2 Bearing;	//Offset from baseline to left/top of font
	GLuint Advance;		//Offset to advance to next character
};
static std::map<GLchar, Character> Characters;

/* class RenderText
* Description:
*   - this is use to load fonts
*   - this is use to display text
*/
class RenderText
{
public:
	RenderText() { }
	~RenderText() { }
	GLuint VAO, VBO;

	//load all the necessary gl calls for the VBOs and VAOs
	void LoadVBOForText() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//draw text to the screen
	void TextDraw(Material &s, glm::mat4x4 *pvm, std::string text, float _x, float _y, float _scale, glm::vec3 color, int orientation)
	{
		GLfloat x = static_cast<GLfloat>(_x);
		GLfloat y = static_cast<GLfloat>(_y);
		GLfloat scale = static_cast<GLfloat>(_scale);


		//binds shader
		s.shader->bind();
		//set the colour of the text
		s.shader->sendUniformVec4("textColor", glm::vec4(color,1.0f));
		//set the progection matrix the will be used for the text
		s.shader->sendUniformMat4("projection", *pvm);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);


		//value the holds the text character
		std::string::const_iterator c;
		//lenght of total characters
		GLfloat startOffSet = 0.0f;

		//iterate through all characters to fine the lenght of the string to get a proper offset
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];
			startOffSet += ((ch.Advance >> 6) * scale);
		}

		//depending on choice, the text will be left, middle, or right orientated.
		if (orientation == 0) { startOffSet = 0.0f; }
		else if (orientation == 1) { startOffSet = startOffSet*0.5f; }
		else if (orientation == 2) { startOffSet = startOffSet; }

		//iterate through all characters to display the string to the screen
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];
			//set the position of the character
			GLfloat xpos = x + (ch.Bearing.x * scale) - startOffSet;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
			//update character size by the scale
			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;
			//set the size of the plane that the characters will be drawn on
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			//bind the charater texture
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			//draw the charater texture
			glDrawArrays(GL_TRIANGLES, 0, 6);
			//set the starting position of the next charater
			static_cast<GLfloat>(x) += (ch.Advance >> 6) * scale;
		}
		//unbind vertex array
		glBindVertexArray(0);
		//unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
		//unbind shader
		//s.shader->unbind();
	}

	//load in a font
	void LoadTextFont(std::string filePath1, RenderText &filePath2) {
		const char* filePath = filePath1.c_str();
		///Initialize the FreeType library
		FT_Library ftLib;
		if (FT_Init_FreeType(&ftLib)) { std::cout << "[ERROR] Could not init freetype library\n" << std::endl; }
		//Initialize the Font
		FT_Face face_font;
		//set the file path of the font
		FT_New_Face(ftLib, filePath, 0, &face_font);
		//set size of font
		FT_Set_Pixel_Sizes(face_font, 0, 48);
		/// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

		for (GLubyte c = 0; c < 128; c++)
		{
			//load character 
			if (FT_Load_Char(face_font, c, FT_LOAD_RENDER))
			{
				std::cout << "[ERROR] Failed to load [" << c << "]" << std::endl;
			}
			//generate font texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RED,
				face_font->glyph->bitmap.width,
				face_font->glyph->bitmap.rows,
				0, GL_RED, GL_UNSIGNED_BYTE,
				face_font->glyph->bitmap.buffer
			);
			//set font parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//set characters to [struct Character]
			Character character = {
				texture,
				glm::vec2(face_font->glyph->bitmap.width, face_font->glyph->bitmap.rows),
				glm::vec2(face_font->glyph->bitmap_left, face_font->glyph->bitmap_top),
				face_font->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, Character>(c, character));
		}

		FT_Done_Face(face_font);
		FT_Done_FreeType(ftLib);
		filePath2.LoadVBOForText();
	}
};

// float myRad = myDegree * degToRad;
static const float degToRad = (3.14159f / 180.0f);
// float myDegree = myRad * radToDegree;
static const float radToDeg = (180.0f / 3.14159f);


/* function Random()
* Description:
*   - this is generates a random number between two variables
*/
template <typename T>
static T Random(T a, T b)
{
	return ((T(rand()) / T(RAND_MAX)) * abs(b - a) + std::fminf(a, b));
}


struct PlayerHealth {
	int MaxHealth;
	int MinHealth;
	int CurrentHealth;
	int PreviousHealth;
};
static bool changeInHealth(PlayerHealth Pnum) {
	if (Pnum.CurrentHealth > Pnum.MaxHealth) { Pnum.CurrentHealth = Pnum.MaxHealth; }
	if (Pnum.CurrentHealth < Pnum.MinHealth) { Pnum.CurrentHealth = Pnum.MinHealth; }
	if (Pnum.CurrentHealth != Pnum.PreviousHealth) { return true; }
	else { return false; }
}

struct PlayerInfo {
	//PlayerValues[i].
	//camera
	int cameraMode;
	glm::mat4x4 cameraTransform;
	glm::vec3 cameraPosition;
	glm::vec3 forwardVector;
	glm::vec3 rightVector;
	//
	int PlayerTeam;
	float MenuSwitchCounter;
	//Shock wave attributes
	bool Right_TRIGGERED;
	bool Left_TRIGGERED;
	bool ShockWave;
	float ShockWaveCounter;
	float ShockWaveChargeUp;
	//Sprint attributes
	float SprintSpeed;
	float SprintCounter;
	float SprintCoolDown;
	//Abilitys
	bool FlipedControllers;
	bool* AbilityAffected;
	float* AbilityCounter;
	float* AbilityLength;
};

//////////////////////////////////////////////////////////////////////


/* function enableCulling()
* Description:
*   - this is called to turn culling on
*	- this is used to disable transparency
*/
static void enableCulling() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}
/* function disableCulling()
* Description:
*   - this is called to turn culling off
*	- this is used to inable transparency
*/
static void disableCulling() {
	glDisable(GL_CULL_FACE);
}

/* function DoesFileExists()
* Description:
*   - this is called to check if a file path exists
*/
inline bool DoesFileExists(const std::string& name) {
	struct stat buffer;
	//if (!stat(name.c_str(), &buffer)) { std::cout << "."; } //Can't open
	//else if (stat(name.c_str(), &buffer)) { std::cout << "."; } //Opened
	return (stat(name.c_str(), &buffer) == 0);
}

/* function writeSomething()
* Description:
*  - this is to display text to the screen
*/
static void writeSomething(float xPos, float yPos, float zPos, std::string str) {
	glRasterPos3f(xPos, yPos, zPos);
	for (unsigned int i = 0; i < str.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
}


//////////////////////////////////////////////////////////////////////


/* function CheckCollision()
* Description:
*   - this is called to check if two objects have hit each other in a square
*/
static bool CheckCollision(Manifold &m)
{
	if (m.A.get()->Viewable && m.B.get()->Viewable) {
		if (m.A.get()->Top().x < m.B.get()->Bottom().x || m.A.get()->Bottom().x > m.B.get()->Top().x) { return false; }
		if (m.A.get()->Top().y < m.B.get()->Bottom().y || m.A.get()->Bottom().y > m.B.get()->Top().y) { return false; }
		if (m.A.get()->Top().z < m.B.get()->Bottom().z || m.A.get()->Bottom().z > m.B.get()->Top().z) { return false; }

		PhysicsMath DoPhysics;
		m.Normal = DoPhysics.GetNormal(m.A.get()->Position(), m.B.get()->Position());
		m.AreColliding = true;
		DoPhysics.~PhysicsMath();
		return true;
	}
	else { m.AreColliding = false; return false; }
}
/* function checkRadialCollision()
* Description:
*   - this is called to check if two objects have hit each other in a circle
*/
static bool checkRadialCollision(Manifold &m)
{
	if (m.A.get()->Viewable && m.B.get()->Viewable) {
		float minimumSeparationX = m.A.get()->Radius().x + m.B.get()->Radius().x;
		float minimumSeparationY = m.A.get()->Radius().y + m.B.get()->Radius().y;
		float minimumSeparationZ = m.A.get()->Radius().z + m.B.get()->Radius().z;
		float dist = glm::distance(m.A.get()->Position(), m.B.get()->Position());

		if (dist > minimumSeparationX) { return false; }
		else if (dist > minimumSeparationY) { return false; }
		else if (dist > minimumSeparationZ) { return false; }
		else { return true; }
	}
	else { m.AreColliding = false; return false; }
}
/* function ResolveCollision()
* Description:
*   - this is called to calculate the output when two objects have hit each other
*/
static void ResolveCollision(Manifold &m)
{
	PhysicsMath DoPhysics;

	m.Normal = glm::normalize(m.B.get()->Position() - m.A.get()->Position());
	glm::vec3 rv = (m.B.get()->Velocity() - m.A.get()->Velocity());// Calculate relative velocity
	float velAlongNormal = DoPhysics.DotProduct(rv, m.Normal);// Calculate relative velocity in terms of the normal direction
	if (velAlongNormal > 0) { return; }// Do not resolve if velocities are separating

	float e = std::fminf(m.A.get()->Restitution(), m.B.get()->Restitution());// Calculate restitution
	float j = (-(1 + e) * velAlongNormal);// Calculate impulse scalar
	j = (j / (m.A.get()->InvertedMass() + m.B.get()->InvertedMass()));// Calculate impulse scalar
	glm::vec3 impulse = (glm::vec3(j * m.Normal)*1000.0f);// Apply impulse
	impulse = glm::vec3(impulse.x, impulse.y*0.1f, impulse.z);

	//m.A.get()->setVelocity(m.A.get()->Velocity()-(m.A.get()->InvertedMass()*impulse));
	//m.B.get()->setVelocity(m.A.get()->Velocity()+(m.B.get()->InvertedMass()*impulse));
	//m.A.get()->setForceOnObject(m.A.get()->Velocity() - ((m.A.get()->InvertedMass()*impulse)));//*m.A.get()->Restitution()
	m.B.get()->setForceOnObject(m.B.get()->Velocity() + ((m.B.get()->InvertedMass()*impulse)));//*m.B.get()->Restitution()

	DoPhysics.~PhysicsMath();
	return;
}
/* function ResolveCollision()
* Description:
*   - this is called to calculate the output when two objects have hit each other
*/
static void ResolveCollision(Manifold &m, float correction)
{
	PhysicsMath DoPhysics;

	m.Normal = glm::normalize(m.B.get()->Position() - m.A.get()->Position());
	glm::vec3 rv = correction*(m.B.get()->Velocity() - m.A.get()->Velocity());// Calculate relative velocity
	float velAlongNormal = DoPhysics.DotProduct(rv, m.Normal);// Calculate relative velocity in terms of the normal direction
	if (velAlongNormal > 0) { return; }// Do not resolve if velocities are separating

	float e = std::fminf(m.A.get()->Restitution(), m.B.get()->Restitution());// Calculate restitution
	float j = (-(1 + e) * velAlongNormal);// Calculate impulse scalar
	j = (j / (m.A.get()->InvertedMass() + m.B.get()->InvertedMass()));// Calculate impulse scalar
	glm::vec3 impulse = (glm::vec3(j * m.Normal)*1000.0f);// Apply impulse
	impulse = glm::vec3(impulse.x, impulse.y*0.1f, impulse.z);

	//m.A.get()->setVelocity(m.A.get()->Velocity()-(m.A.get()->InvertedMass()*impulse));
	//m.B.get()->setVelocity(m.A.get()->Velocity()+(m.B.get()->InvertedMass()*impulse));
	m.A.get()->setForceOnObject(m.A.get()->Velocity() - ((m.A.get()->InvertedMass()*impulse)));//*m.A.get()->Restitution()
	m.B.get()->setForceOnObject(m.B.get()->Velocity() + ((m.B.get()->InvertedMass()*impulse)));//*m.B.get()->Restitution()

	DoPhysics.~PhysicsMath();
	return;
}
/* function ResolveCollision()
* Description:
*   - this is called to calculate the output when two objects have hit each other
*/
static void ResolveCollision(Manifold &m, float correction, float correction2)
{
	PhysicsMath DoPhysics;

	m.Normal = glm::normalize(m.B.get()->Position() - m.A.get()->Position());
	glm::vec3 rv = correction2*(m.B.get()->Velocity() - m.A.get()->Velocity());// Calculate relative velocity
	float velAlongNormal = DoPhysics.DotProduct(rv, m.Normal);// Calculate relative velocity in terms of the normal direction
	if (velAlongNormal > 0) { return; }// Do not resolve if velocities are separating

	float e = std::fminf(m.A.get()->Restitution(), m.B.get()->Restitution());// Calculate restitution
	float j = (-(1 + e) * velAlongNormal);// Calculate impulse scalar
	j = (j / (m.A.get()->InvertedMass() + m.B.get()->InvertedMass()));// Calculate impulse scalar
	glm::vec3 impulse = (glm::vec3(j * m.Normal)*1000.0f);// Apply impulse
	impulse = glm::vec3(impulse.x, impulse.y*0.1f, impulse.z);

	//m.A.get()->setVelocity(m.A.get()->Velocity()-(m.A.get()->InvertedMass()*impulse));
	//m.B.get()->setVelocity(m.A.get()->Velocity()+(m.B.get()->InvertedMass()*impulse));
	m.A.get()->setForceOnObject(m.A.get()->Velocity() - ((m.A.get()->InvertedMass()*impulse*correction)));//*m.A.get()->Restitution()
	m.B.get()->setForceOnObject(m.B.get()->Velocity() + ((m.B.get()->InvertedMass()*impulse*correction2)));//*m.B.get()->Restitution()

	DoPhysics.~PhysicsMath();
	return;
}


//////////////////////////////////////////////////////////////////////

/* function setEnemySpawn()
* Description:
*   - this is called to set enemies position
*	- used m.B for enemies
*/
static void setEnemySpawn(Manifold &m, int Inum) {
	m.B.get()->setMass(5.0f);
	m.B.get()->setDrag(0.010f);
	m.B.get()->setRestitution(2.0f);
	m.B.get()->setScale(glm::vec3(3.0f, 3.0f, 3.0f)); //displayed size
	m.B.get()->setSizeOfHitBox(glm::vec3(3.0f, 2.0f, 3.0f)); //HitBox
	m.B.get()->setSizeOfHitBox(glm::vec3(1.3f)); //HitBox
	float ranPosZ = (Random(-40.0f, 40.0f) + (Random(0.0f, 100.0f)*0.01f));
	float ranPosY = (Random(10.0f, 40.0f) + (Random(0.0f, 100.0f)*0.01f)); 
	float ranPosX = (Random(-1.0f, 1.0f) + (Random(0.0f, 100.0f)*0.01f));
	float ranFall = (Random(1.0f, 8.0f) + (Random(0.0f, 100.0f)*0.01f));
	if (Inum % 2 == 0) { m.B.get()->setPosition(glm::vec3(-ranPosX, ranPosY, ranPosZ)); }
	else { m.B.get()->setPosition(glm::vec3(ranPosX, ranPosY, ranPosZ)); }
	m.B.get()->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	m.B.get()->setForceOnObject(glm::vec3(0.0f, (-0.15f - ranFall), 0.0f));
}

/* function applyWallAvoidingSystem()
* Description:
*   - this applys force to objects when they get near something
*	- used m.B for fleeing objects
*/
static void applyWallAvoidingSystem(Manifold &m, float infunce) {
	glm::vec3 seek = m.A.get()->Position() - m.B.get()->Position();
	if (glm::length(seek) > 0.0f) { seek = glm::normalize(seek); }
	else { seek = glm::vec3(0.0f); }
	seek *= infunce; //how much infunce 
	m.B.get()->setForceOnObject(m.B.get()->ForceOnObject() + seek);
}
/* function ForceWallAvoidingSystem()
* Description:
*   - this over-rides the force applyed to objects when they get near something
*	- used m.B for fleeing objects
*/
static void ForceWallAvoidingSystem(Manifold &m, float infunce) {
	glm::vec3 seek = m.A.get()->Position() - m.B.get()->Position();
	if (glm::length(seek) > 0.0f) { seek = glm::normalize(seek); }
	else { seek = glm::vec3(0.0f); }
	seek *= infunce; //how much infunce 
	m.B.get()->setForceOnObject(seek);
}
/* function applySeekSystem()
* Description:
*   - this applys force to objects to make them go in certain directions
*	- used m.B for seeking objects
*/
static void applySeekSystem(Manifold &m, float infunce) {
	glm::vec3 seek = m.A.get()->Position() - m.B.get()->Position();
	if (glm::length(seek) > 0.0f) { seek = glm::normalize(seek); }
	else { seek = glm::vec3(0.0f); }
	seek *= infunce; //how much infunce 
	m.B.get()->setForceOnObject(m.B.get()->ForceOnObject() + seek);
}
/* function FleeFromDirection()
* Description:
*   - this applys force to objects towards certain directions
*	- used m.A for fleeing objects
*	- used to make enemies flee walls
*/
static void FleeFromDirection(Manifold &m, float infunce, std::string fleeDirection) {
	glm::vec3 flee;
	if		(fleeDirection == "x")  { flee = m.A.get()->Position() - glm::vec3(m.A.get()->Position().x + 1.0f, m.A.get()->Position().y, m.A.get()->Position().z); }
	else if (fleeDirection == "-x") { flee = m.A.get()->Position() - glm::vec3(m.A.get()->Position().x - 1.0f, m.A.get()->Position().y, m.A.get()->Position().z); }
	else if (fleeDirection == "y")  { flee = m.A.get()->Position() - glm::vec3(m.A.get()->Position().x, m.A.get()->Position().y + 1.0f, m.A.get()->Position().z); }
	else if (fleeDirection == "-y") { flee = m.A.get()->Position() - glm::vec3(m.A.get()->Position().x, m.A.get()->Position().y - 1.0f, m.A.get()->Position().z); }
	else if (fleeDirection == "z")  { flee = m.A.get()->Position() - glm::vec3(m.A.get()->Position().x, m.A.get()->Position().y, m.A.get()->Position().z + 1.0f); }
	else if (fleeDirection == "-z") { flee = m.A.get()->Position() - glm::vec3(m.A.get()->Position().x, m.A.get()->Position().y, m.A.get()->Position().z - 1.0f); }
	else { flee = m.A.get()->Position() - m.A.get()->Position(); }

	if (glm::length(flee) > 0.0f) { flee = glm::normalize(flee); }
	else { flee = glm::vec3(0.0f); }
	flee *= infunce; //how much infunce 
	m.B.get()->setForceOnObject(m.B.get()->ForceOnObject() + flee);
}
/* function applyAvoidingSystem()
* Description:
*   - this applys force to both objects to make them seperate
*/
static void applyAvoidingSystem(Manifold &m, float infunce) {
	glm::vec3 avoid = m.A.get()->Position() - m.B.get()->Position();
	if (glm::length(avoid) > 0.0f) { avoid = glm::normalize(avoid); }
	else { avoid = glm::vec3(0.0f); }
	avoid *= infunce; //how much infunce 
	m.A.get()->setForceOnObject(m.B.get()->ForceOnObject() + avoid);
	m.B.get()->setForceOnObject(m.B.get()->ForceOnObject() - avoid);
}
/* function applyRadialAvoidingSystem()
* Description:
*   - this applys force to both objects to make them seperate if they are within a certain distance of each other
*/
static bool applyRadialAvoidingSystem(Manifold &m, float areaAvoidence, float infunce)
{
	float minimumSeparationX = ((m.A.get()->Radius().x + m.B.get()->Radius().x));
	float minimumSeparationY = ((m.A.get()->Radius().y + m.B.get()->Radius().y));
	float minimumSeparationZ = ((m.A.get()->Radius().z + m.B.get()->Radius().z));
	float dist = glm::distance(m.A.get()->Position(), m.B.get()->Position());

	if (dist > (minimumSeparationX+areaAvoidence)) { return false; }
	else if (dist > (minimumSeparationY+areaAvoidence)) { return false; }
	else if (dist > (minimumSeparationZ+areaAvoidence)) { return false; }
	else {
		glm::vec3 avoid = m.A.get()->Position() - m.B.get()->Position(); 
		if (glm::length(avoid) > 0.0f) { avoid = glm::normalize(avoid); }
		else { avoid = glm::vec3(0.0f); }
		if (avoid.y > 0.5) { avoid.x = avoid.x*3.0f; avoid.z = avoid.z*3.0f; }
		else if (avoid.y < -0.5) { avoid.x = avoid.x*3.0f; avoid.z = avoid.z*3.0f; }
		avoid *= infunce; //how much infunce 
		m.A.get()->setForceOnObject(m.B.get()->ForceOnObject() + avoid);
		m.B.get()->setForceOnObject(m.B.get()->ForceOnObject() - avoid);
	}

	return true;
}
static bool applyRadialFleeingSystem(Manifold &m, float areaAvoidence, float infunce)
{
	float minimumSeparationX = ((m.A.get()->Radius().x + m.B.get()->Radius().x));
	float minimumSeparationY = ((m.A.get()->Radius().y + m.B.get()->Radius().y));
	float minimumSeparationZ = ((m.A.get()->Radius().z + m.B.get()->Radius().z));
	float dist = glm::distance(m.A.get()->Position(), m.B.get()->Position());

	if (dist > (minimumSeparationX + areaAvoidence)) { return false; }
	else if (dist > (minimumSeparationY + areaAvoidence)) { return false; }
	else if (dist > (minimumSeparationZ + areaAvoidence)) { return false; }
	else {
		glm::vec3 avoid = m.A.get()->Position() - m.B.get()->Position();
		if (glm::length(avoid) > 0.0f) { avoid = glm::normalize(avoid); }
		else { avoid = glm::vec3(0.0f); }
		if (avoid.y > 0.5) { avoid.x = avoid.x*3.0f; avoid.z = avoid.z*3.0f; }
		else if (avoid.y < -0.5) { avoid.x = avoid.x*3.0f; avoid.z = avoid.z*3.0f; }
		avoid *= infunce; //how much infunce 
		m.B.get()->setForceOnObject(m.B.get()->ForceOnObject() - avoid);
	}

	return true;
}

/* function CheckIfOnObject()
* Description:
*   - this checks to see if two objects are inside eachother
*/
static bool CheckIfOnObject(Manifold &m, float OnObjRadius, bool DoSquareAndCircle) {
	if (m.B.get()->Viewable) {
		//check in a square
		if (m.A.get()->Top().x < (m.B.get()->Bottom().x + m.B.get()->Radius().x*OnObjRadius) || m.A.get()->Bottom().x > (m.B.get()->Top().x - m.B.get()->Radius().x*OnObjRadius)
		 || m.A.get()->Top().z < (m.B.get()->Bottom().z + m.B.get()->Radius().z*OnObjRadius) || m.A.get()->Bottom().z > (m.B.get()->Top().z - m.B.get()->Radius().z*OnObjRadius)) 
		{ return false; }
		else if (DoSquareAndCircle) {
			//if player is on the object
			if (m.B.get()->Position().y < ((m.A.get()->Position().y + m.A.get()->Radius().y) + m.B.get()->Radius().y + (m.B.get()->Radius().y*0.2f))
				&& m.B.get()->Position().y >(m.A.get()->Position().y + (m.A.get()->Radius().y + (m.B.get()->Radius().y*0.2f)))
				&& m.B.get()->Velocity().y < 0.08f && m.B.get()->Velocity().y > -0.08f)
			{
				m.B.get()->setPosition(glm::vec3(m.B.get()->Position().x, ((m.A.get()->Position().y + m.A.get()->Radius().y) + m.B.get()->Radius().y), m.B.get()->Position().z));
				return true;
			}
		}
		//check in a circle
		float minimumSeparationX = (m.A.get()->Radius().x + m.B.get()->Radius().x)*0.50f;
		//float minimumSeparationY = m.A.get()->Radius().y + (m.B.get()->Radius().y + m.B.get()->Radius().y*0.2f);//&& dist < minimumSeparationY
		float minimumSeparationZ = (m.A.get()->Radius().z + m.B.get()->Radius().z)*0.50f;
		//float dist = glm::distance(glm::vec3(m.A.get()->Position().x, 01.0f, m.A.get()->Position().z), glm::vec3(m.B.get()->Position().x, 01.0f, m.B.get()->Position().z));
		float dist = glm::distance(m.A.get()->Position(), m.B.get()->Position());
		if (dist < minimumSeparationX && dist < minimumSeparationZ) { 
			//if player is on the object
			if (m.B.get()->Position().y < ((m.A.get()->Position().y + m.A.get()->Radius().y) + m.B.get()->Radius().y + (m.B.get()->Radius().y*0.2f))
				&& m.B.get()->Position().y > (m.A.get()->Position().y + (m.A.get()->Radius().y + (m.B.get()->Radius().y*0.2f)))
				&& m.B.get()->Velocity().y < 0.08f && m.B.get()->Velocity().y > -0.08f)
			{
				m.B.get()->setPosition(glm::vec3(m.B.get()->Position().x, ((m.A.get()->Position().y + m.A.get()->Radius().y) + m.B.get()->Radius().y), m.B.get()->Position().z));
				return true;
			}
		}
		else { return false; }
	}
	return false;
}
/* function ObjectInBox()
* Description:
*   - this checks to see it an objects is inside the square hit box of another (boarder)
*/
static bool ObjectInBox(Manifold &m) {
	//If Object go outside the box
	if		((m.A.get()->Position().x + (m.A.get()->Radius().x*0.5f)) > (m.B.get()->Top().x))	{ return false; }
	else if ((m.A.get()->Position().x - (m.A.get()->Radius().x*0.5f)) < (m.B.get()->Bottom().x))	{ return false; }
	else if ((m.A.get()->Position().y + (m.A.get()->Radius().y*0.5f)) > (m.B.get()->Top().y))	{ return false; }
	else if ((m.A.get()->Position().y - (m.A.get()->Radius().y*0.5f)) < (m.B.get()->Bottom().y))	{ return false; }
	else if ((m.A.get()->Position().z + (m.A.get()->Radius().z*0.5f)) > (m.B.get()->Top().z))	{ return false; }
	else if ((m.A.get()->Position().z - (m.A.get()->Radius().z*0.5f)) < (m.B.get()->Bottom().z))	{ return false; }
	else { return true; }
}
/* function CheckIfObjectInBorderOfBox()
* Description:
*   - this checks to see it an objects is inside the boarder of another object (boarder edge)
*   - this is used to set objects at the right position
*/
static bool CheckIfObjectInBorderOfBox(Manifold &m) {
	//m.A is player //m.B is border
	if ((m.A.get()->Position().x < m.B.get()->Bottom().x + m.A.get()->Radius().x && m.A.get()->Position().x > m.B.get()->Bottom().x - m.A.get()->Radius().x)
	 || (m.A.get()->Position().x > m.B.get()->Top().x	-	 m.A.get()->Radius().x && m.A.get()->Position().x < m.B.get()->Top().x +	m.A.get()->Radius().x)) { return true; }
	if ((m.A.get()->Position().y < m.B.get()->Bottom().y + m.A.get()->Radius().y && m.A.get()->Position().y > m.B.get()->Bottom().y - m.A.get()->Radius().y)
	 || (m.A.get()->Position().y > m.B.get()->Top().y -	 m.A.get()->Radius().y && m.A.get()->Position().y < m.B.get()->Top().y + m.A.get()->Radius().y)) { return true; }
	if ((m.A.get()->Position().z < m.B.get()->Bottom().z + m.A.get()->Radius().z && m.A.get()->Position().z > m.B.get()->Bottom().z - m.A.get()->Radius().z)
	 || (m.A.get()->Position().z > m.B.get()->Top().z -	 m.A.get()->Radius().z && m.A.get()->Position().z < m.B.get()->Top().z + m.A.get()->Radius().z)) { return true; }
	return false;
}



/* function ObjectsWithinRange()
* Description:
*   - this checks to see it an objects are inside a specified range of eachother
*/
static bool ObjectsWithinRange(Manifold &m, float range) {
	glm::vec3 seek = (m.A.get()->Position() - m.B.get()->Position());
	if (glm::length(seek) > 0.0f && glm::length(seek) < range) { seek = glm::normalize(seek);  return true; }
	else { seek = glm::vec3(0.0f); return false; }
}
/* function applyWanderingSystem()
* Description:
*   - this applys a force on a object that makes is move slitly
*/
static void applyWanderingSystem(GameObject* obj, float infunce, float randPoseY) {
	glm::vec3 WanderingToPostition = glm::vec3(0.0f, randPoseY, 0.0f);

	glm::vec3 Wandering = (WanderingToPostition - obj->Position());
	if (glm::length(Wandering) > 0.0f) { Wandering = glm::normalize(Wandering); }
	else { Wandering = glm::vec3(0.0f); return; }
	Wandering *= infunce; //how much infunce 
	obj->setAcceleration(obj->Acceleration() + Wandering);
}



/* function applyGravitationalForces()
* Description:
*   - this applys a force on a object that makes is move down
*	- m.A is the ground while m.B is the Objects
*/
static void applyGravitationalForces(Manifold &m, float gv)
{//m.A is the ground //m.B is the Objects

	if (m.B.get()->Viewable) {
		if (m.B.get()->InAirCounter > 0.0f) { m.B.get()->InAirCounter -= 0.01f; }
		else { m.B.get()->IsJumping = false; }

		//if the object is in the air; apply gravity
		if (m.B.get()->inAir) {
			//Jumping
			if (m.B.get()->IsJumping && m.B.get()->InAirCounter > 0.0f) {
				glm::vec3 gravity = glm::vec3(0.0f, (((-0.980f - gv) * (m.B.get()->Mass())*0.15 + m.B.get()->InAirCounter)), 0.0f);
				glm::vec3 JumpDirectionForce = glm::vec3((radToDeg*m.B.get()->ForwardDirection.x)*-0.001f, 0.0f, (radToDeg*m.B.get()->ForwardDirection.z)*-0.001f);

				m.B.get()->setForceOnObject(m.B.get()->ForceOnObject() + gravity + JumpDirectionForce);
			}
			//NOT Jumping //Falling
			else {
				glm::vec3 gravity = glm::vec3(0.0f, ((-0.980f * (m.B.get()->Mass())*0.15) + gv), 0.0f);
				glm::vec3 JumpDirectionForce = glm::vec3((radToDeg*m.B.get()->ForwardDirection.x)*-0.001f, 0.0f, (radToDeg*m.B.get()->ForwardDirection.z)*-0.001f);
				m.B.get()->setForceOnObject(m.B.get()->ForceOnObject() + gravity);
			}
		}

		if (m.B.get()->onObject == false) {
			//if object is lower then the ground
			if ((m.A.get()->Position().y + (m.B.get()->Radius().y - (m.B.get()->Radius().y*0.1f))) > m.B.get()->Position().y) {
				m.B.get()->setPosition(glm::vec3(m.B.get()->Position().x, (m.A.get()->Position().y + m.B.get()->Radius().y), m.B.get()->Position().z));
				if (m.B.get()->Velocity().y < 0.0f) { m.B.get()->setVelocity(glm::vec3(m.B.get()->Velocity().x, m.B.get()->Velocity().y*-0.450f, m.B.get()->Velocity().z)); }
				m.B.get()->inAir = false;
			}
			//if the object is above the ground
			else if (m.B.get()->Position().y > (m.A.get()->Position().y + (m.B.get()->Radius().y + (m.B.get()->Radius().y*0.2f)))) { m.B.get()->inAir = true; }
			//if the object is on the ground
			else if (m.B.get()->Position().y < (m.A.get()->Position().y + (m.B.get()->Radius().y + (m.B.get()->Radius().y*0.2f))) && m.B.get()->Velocity().y < 0.1f) {
				m.B.get()->setPosition(glm::vec3(m.B.get()->Position().x, (m.A.get()->Position().y + m.B.get()->Radius().y), m.B.get()->Position().z));
				m.B.get()->inAir = false;
			}
			//if the object is someplace
			else { m.B.get()->inAir = false; }
		}
	}
}


//////////////////////////////////////////////////////////////////////


static void someTempFiles() {

	int dd = 1, mm = 9, yy = 1;
	char s[25];
	sprintf(s, "%02d - %02d - %04d", mm, dd, yy);
	std::cout << s;



	std::cout << std::setfill('0') << std::setw(5) << 25;

}