// Core Libraries
#define WIN32_LEAN_AND_MEAN //cmd
#include <Windows.h> //cmd
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <memory> // for std::shared_ptr
#include <map> // for std::map

// 3rd Party Libraries
#include <GL\glew.h>
#include <GL\glut.h>
#include <GL\freeglut.h>
#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>
#include <glm\glm\glm.hpp>
#include <glm\glm\vec3.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtc\type_ptr.hpp>
#include <glm\glm\gtc\random.hpp> //time
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm\gtx\rotate_vector.hpp>

// User Libraries
#include "GameObject.h"
#include "MorphMath.h"
#include "GamePadControls.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ExtraFunctions.cpp"
#include "HTRLoader.h"
#include "FMOD/SoundEngine.h"
#include "FileLoader.h"
#include "Material.h"
#include "FrameBufferObject.h"

//temp
static float TestFloat = 0.0f; static float TestFloatIncrementAmount = 01.0f;
//////////////////////////////////////////////////////////////////////

// Create Shader
Shader *shader;
Shader *TextShader;
glm::vec3 lightPosition_01(0.0f, 300.0f, 0.0f);//( 48.0f, 30.0f, 0.0f);
glm::vec3 lightPosition_02(0.0f, -30.0f, 0.0f);//(-48.0f, 30.0f, 0.0f);
// Monitor our Projections
glm::mat4x4 projectionMatrix[4];
glm::mat4x4 orthoMatrix[4];
glm::mat4x4 modelViewMatrix[4];
glm::mat4x4 cameraViewMatrix[4];
// These are our texture handles
GLuint textureHandle;

// Defines Core variables//
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Miliseconds per frame
float deltaTasSecs;
int windowWidth = (16*80); int windowHeight = (9*80);
int mousepositionX; int mousepositionY;
int screenpositionX; int screenpositionY;
//mouse position to object position
float MPosToOPosX; float MPosToOPosY;
//key input //check if a key is down or up
bool keyDown[256];
bool mouseDown[3];
//Game Objects//
std::map<std::string, std::shared_ptr<GameObject>> GameObjects; //working on this

const int NumberOfPlayers = 2; GameObject Players[NumberOfPlayers];
const int NumberOfAffects = 3; GameObject AffectsOnPlayer[NumberOfPlayers][NumberOfAffects]; //GameObject ShockWaves[NumberOfPlayers];
const int NumberOfRifts = 2; GameObject Rifts[NumberOfRifts];
const int NumberOfObjects = 7; GameObject Objects[NumberOfObjects];
const int NumberOfSpecials = 10; GameObject Specials[NumberOfSpecials];
const int NumberOfEnemies = 12; GameObject Enemies[12];
GameObject ShadowObject[2];
const int NumberOfPlaneForText = 5; GameObject planeForText[NumberOfPlaneForText];//Menu Screens
const int NumberOfSliders = 10; GameObject planeForSliders[NumberOfSliders]; GameObject ButtonForSliders[NumberOfSliders];
Sliders Slider[NumberOfSliders];//Sliders
const int NumberOfButtons = 7; GameObject ButtonObjects[NumberOfButtons];
Buttons Button[NumberOfButtons];//Buttons
const int NumberOfBorders = 1; GameObject Borders[NumberOfBorders];//Boarders
std::map<std::string, std::shared_ptr<Material>> materials;//Materials
//Framebuffer objects
FrameBufferObject fbo;


//Random Variables//

float randomSpecialTime;
//camera viewport
int cameralook = 0; int cameraMode = 0;
glm::vec3 cameraPosition[NumberOfRifts];
glm::vec3 forwardVector[NumberOfRifts]{ glm::vec3(-1.0f, 0.0f, 0.0f) , glm::vec3(1.0f, 0.0f, 0.0f) };
glm::vec3 rightVector;
//player health
int Health[NumberOfRifts]{40,40};
//Bool's for the game///////////////////////////
bool inMenu = true; bool inGame = false;
bool inOptions = false; int inOptionsTab = 0;
std::string lastMenu = "inMenu"; float MenuSwitchCounter[NumberOfPlayers] = { 0.0f, 0.0f };
const bool ApplyingGravity = true;
const bool CollisionBetweenObjects = true;
const bool IdleEnemiesRespawn = true;
const bool EnableShadows = true;
//Shock wave attributes
bool Right_TRIGGERED[NumberOfPlayers]; bool Left_TRIGGERED[NumberOfPlayers]; bool speedControlSW = false;
bool AButtonDown = false;
bool PShockWave[NumberOfPlayers] = { false }; float PShockWaveCounter[NumberOfPlayers] = { 0.0f }; float PShockWaveChargeUp[NumberOfPlayers] = { 0.0f };
//Sprint attributes
float PSprintCounter[NumberOfPlayers] = { 0.0f }; float PSprintCoolDown = 2.0f;
glm::vec3 speedToWallDegradation(0.80f, 0.50f, 0.80f);

//Abilitys
int PlayerTeam[NumberOfPlayers];
bool AbilityAffected[NumberOfPlayers][NumberOfSpecials];
float AbilityCounter[NumberOfPlayers][NumberOfSpecials];
float AbilityLength[NumberOfPlayers][NumberOfSpecials];
bool FlipedControllers[NumberOfPlayers];
float SprintSpeed = 1.5f; float abilityRotation[NumberOfSpecials]{ 0.0f };

//Gamepad controls
Gamepad gamepad;
MorphMath morphmath;
//Sounds
Sound systemSound;
Sound powerup[9];//Powerup sounds
Sound Fx[3];//Fx sounds
FMOD::Channel *powChannel;
FMOD::Channel *FxChannel[3];
//Text
RenderText SystemText;
Manifold m;


//////////////////////////////////////////////////////////////////////

/* function setBoardStart()
* Description:
*  - this sets enemies and players positions
*  - set player health
*/
void setBoardStart() {

	if (&Slider[2] != NULL) {
		Health[0] = static_cast<int>(Slider[2].SNob_Precent.x);
		Health[1] = static_cast<int>(Slider[2].SNob_Precent.x);
	}

	for (int i = 0; i < NumberOfPlayers; i++)
	{
		if (&Players[i] != NULL) {
			float multipliyer002 = 0.0f;
			if (i > 1) { multipliyer002 += 20.0f; }
			if (PlayerTeam[i] == 0) { Players[i].setPosition(glm::vec3(20.0f, 0.0f, -10.0f + multipliyer002)); }
			else if (PlayerTeam[i] == 1) { Players[i].setPosition(glm::vec3(-20.0f, 0.0f, -10.0f + multipliyer002)); }

			Players[i].setVelocity(glm::vec3(0.0f));
			Players[i].setForceOnObject(glm::vec3(0.0f));
		}
	}

	if (&Enemies[0] != NULL) { m.A = Enemies[0]; }
	for (int i = 0; i < NumberOfEnemies; i++)
	{
		if (&Enemies[i] != NULL) {
			m.B = Enemies[i];
			setEnemySpawn(m, i);
			Enemies[i] = m.B;
		}
	}

	for (int i = 0; i < NumberOfSpecials; i++) 
	{
		if (&Specials[i] != NULL) {
			Specials[i].Viewable = false;
			Specials[i].setVelocity(glm::vec3(0.0f));
			Specials[i].setForceOnObject(glm::vec3(0.0f));
			Specials[i].setRotation(glm::vec3(0.0f));
		}
	}
}

void exitProgram() {

	if (&ShadowObject[0] != NULL) { ShadowObject[0].~GameObject(); }
	for (unsigned int i = 0; i < NumberOfPlayers; i++) {
		if (&Players[i] != NULL) { Players[i].~GameObject(); }
		for (unsigned int j = 0; j < NumberOfAffects; j++) {
			if (&AffectsOnPlayer[i][j] != NULL) { AffectsOnPlayer[i][j].~GameObject(); }
		}
	}
	for (unsigned int i = 0; i < NumberOfRifts; i++) { if (&Rifts[i] != NULL) { Rifts[i].~GameObject(); } }
	for (unsigned int i = 0; i < NumberOfObjects; i++) { if (&Objects[i] != NULL) { Objects[i].~GameObject(); } }
	for (unsigned int i = 0; i < NumberOfEnemies; i++) { if (&Enemies[i] != NULL) { Enemies[i].~GameObject(); } }
	for (unsigned int i = 0; i < NumberOfSpecials; i++) { if (&Specials[i] != NULL) { Specials[i].~GameObject(); } }
	for (unsigned int i = 0; i < NumberOfPlaneForText; i++) { if (&planeForText[i] != NULL) { planeForText[i].~GameObject(); } }
	for (unsigned int i = 0; i < NumberOfSliders; i++) { 
		if (&planeForSliders[i] != NULL) { planeForSliders[i].~GameObject(); }
		if (&ButtonForSliders[i] != NULL) { ButtonForSliders[i].~GameObject(); }
	}
	for (unsigned int i = 0; i < NumberOfButtons; i++) { if (&ButtonObjects[i] != NULL) { ButtonObjects[i].~GameObject(); } }
	for (unsigned int i = 0; i < NumberOfBorders; i++) { if (&Borders[i] != NULL) { Borders[i].~GameObject(); } }

	for (std::map<std::string, std::shared_ptr<Material>>::iterator itr = materials.begin(); itr != materials.end(); itr++) { materials.erase(itr); }

	glutLeaveMainLoop();
}


/* function WhatCameraIsLookingAt(void)
* Description:
*  - this sets what the player will be seeing
*/
void WhatCameraIsLookingAt()
{
	if (cameralook == 0)
	{
		glViewport(0, 0, (windowWidth / 2), windowHeight);
		projectionMatrix[0] = glm::perspective(45.0f, (windowWidth / windowHeight)*1.0f, 0.1f, 10000.f);
		modelViewMatrix[0] = glm::mat4x4(1.0f);

		if (cameraMode == 0) {
			glm::mat4x4 transform = glm::lookAt(
				glm::vec3((Players[0].Position().x + 50.0f), 50.0f, (Players[0].Position().z)),
				glm::vec3((Players[0].Position().x), 1.0f, (Players[0].Position().z)),
				glm::vec3(0.0f, 1.0f, 0.0f));
			modelViewMatrix[0] = transform * modelViewMatrix[0];
			cameraPosition[0] = glm::vec3((Players[0].Position().x - 50.0f), 50.0f, (Players[0].Position().z));
		}
		else if (cameraMode == 1) {
			glm::mat4x4 transform = glm::lookAt(
				cameraPosition[0],
				cameraPosition[0] + forwardVector[0], 
				glm::vec3(0.0f, 1.0f, 0.0f));
			modelViewMatrix[0] = transform * modelViewMatrix[0];
		}
		cameraViewMatrix[0] = glm::mat4(glm::translate(cameraPosition[0])) * modelViewMatrix[0];
	}
	else if (cameralook == 1)
	{
		glViewport((windowWidth/2), 0, (windowWidth / 2), windowHeight);
		projectionMatrix[1] = glm::perspective(45.0f, (windowWidth/windowHeight)*1.0f, 0.1f, 10000.f);
		modelViewMatrix[1] = glm::mat4x4(1.0f);

		if (cameraMode == 0) {
			glm::mat4x4 transform = glm::lookAt(
				glm::vec3((Players[1].Position().x - 50.0f), 50.0f, (Players[1].Position().z)),
				glm::vec3((Players[1].Position().x), 1.0f, (Players[1].Position().z)),
				glm::vec3(0.0f, 1.0f, 0.0f));
			modelViewMatrix[1] = transform * modelViewMatrix[1];
			cameraPosition[1] = glm::vec3((Players[1].Position().x - 50.0f), 50.0f, (Players[1].Position().z));
		}
		else if (cameraMode == 1) {
			glm::mat4x4 transform = glm::lookAt(
				cameraPosition[1],
				cameraPosition[1] + forwardVector[1],
				glm::vec3(0.0f, 1.0f, 0.0f));
			modelViewMatrix[1] = transform * modelViewMatrix[1];
		}
		cameraViewMatrix[1] = glm::mat4(glm::translate(cameraPosition[1])) * modelViewMatrix[1];
	}
	else if (cameralook == 2)
	{
		glViewport(0, 0, windowWidth, windowHeight);
		//projectionMatrix[2] = glm::ortho<float>(-windowWidth*0.10f, windowWidth*0.10f, -windowHeight*0.10f, windowHeight*0.10f, 0.01f, 1000.0f);
		projectionMatrix[2] = glm::perspective(45.0f, (windowWidth / windowHeight)*1.0f, 0.1f, 10000.f);
		modelViewMatrix[2] = glm::mat4x4(1.0f);

		glm::mat4x4 transform = glm::lookAt(
			glm::vec3(0.0f, 50.0f, 0.0f),//eye
			glm::vec3(0.01f, 0.01f, 0.01f),//center
			glm::vec3(0.0f, 0.0f, -1.0f));//up
		modelViewMatrix[2] = transform * modelViewMatrix[2];
		cameraViewMatrix[2] = glm::mat4(glm::translate(cameraPosition[0])) * modelViewMatrix[2];
	}
	else if (cameralook == 3)
	{
		glViewport(0, 0, windowWidth, windowHeight);
		projectionMatrix[3] = glm::ortho<float>(-windowWidth*0.50f, windowWidth*0.50f, -windowHeight*0.50f, windowHeight*0.50f);
		modelViewMatrix[3] = glm::mat4x4(1.0f);

		glm::mat4x4 transform = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 10.0f),//eye
			glm::vec3(0.01f, 0.01f, 0.01f),//center
			glm::vec3(0.0f, 1.0f, 0.0f));//up
		modelViewMatrix[3] = transform * modelViewMatrix[3];
		cameraViewMatrix[3] = glm::mat4(glm::translate(cameraPosition[0])) * modelViewMatrix[3];
	}
	else { }
}


/* function ControllerDelayButton()
* Description:
*   - this is called to get inputs from controllers
*/
void ControllerDelayButton(int portNumber, float deltaTasSeconds)
{
	float MovementModifier = 12.0f;


	if (inGame) {
		if (cameraMode == 0) {
			//JoySticks
			if (PlayerTeam[portNumber] == 0) {
				float Tx = 0.0f; float Ty = 0.0f; float Tz = 0.0f; float rotY = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
				if (!FlipedControllers[portNumber]) {
					if (gamepad.leftStickY < -0.1) { Tx -= gamepad.leftStickY * 0.0666666f; } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
					if (gamepad.leftStickY > 00.1) { Tx -= gamepad.leftStickY * 0.0666666f; } //Y-Up
					if (gamepad.leftStickX < -0.1) { Tz -= gamepad.leftStickX * 0.0666666f; } //X-Left
					if (gamepad.leftStickX > 00.1) { Tz -= gamepad.leftStickX * 0.0666666f; } //X-Right
				}
				else {
					if (gamepad.leftStickY < -0.1) { Tx -= gamepad.leftStickY * -0.0666666f; } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
					if (gamepad.leftStickY > 00.1) { Tx -= gamepad.leftStickY * -0.0666666f; } //Y-Up
					if (gamepad.leftStickX < -0.1) { Tz -= gamepad.leftStickX * -0.0666666f; } //X-Left
					if (gamepad.leftStickX > 00.1) { Tz -= gamepad.leftStickX * -0.0666666f; } //X-Right
				}

				if (Tx > 00.055f) { Tx = 00.055f; }
				else if (Tx < -0.055f) { Tx = -0.055f; }
				if (Tz > 00.055f) { Tz = 00.055f; }
				else if (Tz < -0.055f) { Tz = -0.055f; }
				Tx = Tx * 1.25f; Ty = Ty * 1.25f; Tz = Tz * 1.25f;

				if (gamepad.rightStickX > 00.1) { rotY -= gamepad.rightStickX * 0.0666666f; }
				if (gamepad.rightStickX < -0.1) { rotY -= gamepad.rightStickX * 0.0666666f; }

				Tx = ((Tx*MovementModifier*0.750f));//(Tx*MovementModifier)*(Tx*MovementModifier));
				Ty = ((Ty*MovementModifier*0.750f));//(Ty*MovementModifier)*(Ty*MovementModifier));
				Tz = ((Tz*MovementModifier*0.750f));//(Tz*MovementModifier)*(Tz*MovementModifier));

				Players[portNumber].setForceOnObject(glm::vec3(Tx, Ty, Tz));
				Players[portNumber].setVelocity(glm::vec3(Tx, Ty, Tz)*Players[portNumber].SprintSpeed);
				Players[portNumber].ForwardDirection = (Players[portNumber].Position() - (Players[portNumber].Position() + Players[portNumber].Velocity()));
			}
			if (PlayerTeam[portNumber] == 1) {
				float Tx = 0.0f; float Ty = 0.0f; float Tz = 0.0f; float rotY = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
				if (!FlipedControllers[portNumber]) {
					if (gamepad.leftStickY < -0.1) { Tx += gamepad.leftStickY * 0.0666666f; } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
					if (gamepad.leftStickY > 00.1) { Tx += gamepad.leftStickY * 0.0666666f; } //Y-Up
					if (gamepad.leftStickX < -0.1) { Tz += gamepad.leftStickX * 0.0666666f; } //X-Left
					if (gamepad.leftStickX > 00.1) { Tz += gamepad.leftStickX * 0.0666666f; } //X-Right
				}
				else {
					if (gamepad.leftStickY < -0.1) { Tx -= gamepad.leftStickY * 0.0666666f; } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
					if (gamepad.leftStickY > 00.1) { Tx -= gamepad.leftStickY * 0.0666666f; } //Y-Up
					if (gamepad.leftStickX < -0.1) { Tz -= gamepad.leftStickX * 0.0666666f; } //X-Left
					if (gamepad.leftStickX > 00.1) { Tz -= gamepad.leftStickX * 0.0666666f; } //X-Right
				}

				if (Tx > 00.055f) { Tx = 00.055f; }
				else if (Tx < -0.055f) { Tx = -0.055f; }
				if (Tz > 00.055f) { Tz = 00.055f; }
				else if (Tz < -0.055f) { Tz = -0.055f; }
				Tx = Tx * 1.25f; Ty = Ty * 1.25f; Tz = Tz * 1.25f;

				if (gamepad.rightStickX > 0.1) { rotY -= gamepad.rightStickX * 0.0666666f; }
				if (gamepad.rightStickX < -0.1) { rotY -= gamepad.rightStickX * 0.0666666f; }

				Tx = ((Tx*MovementModifier*0.750f));//(Tx*MovementModifier)*(Tx*MovementModifier));
				Ty = ((Ty*MovementModifier*0.750f));//(Ty*MovementModifier)*(Ty*MovementModifier));
				Tz = ((Tz*MovementModifier*0.750f));//(Tz*MovementModifier)*(Tz*MovementModifier));

				Players[portNumber].setForceOnObject(glm::vec3(Tx, Ty, Tz));
				Players[portNumber].setVelocity(glm::vec3(Tx, Ty, Tz)*Players[portNumber].SprintSpeed);
				Players[portNumber].ForwardDirection = (Players[portNumber].Position() - (Players[portNumber].Position() + Players[portNumber].Velocity()));
			}
		}
		else if (cameraMode == 1) {
			if (portNumber >= 0) {
				float Tmovement = 0.0f; float yaw = 0.0f; float pitch = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.

				if (!FlipedControllers[portNumber]) {
					if (gamepad.leftStickY < -0.1) { Tmovement = (MovementModifier * (gamepad.leftStickY * 0.0666666f)); } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
					if (gamepad.leftStickY > 00.1) { Tmovement = (MovementModifier * (gamepad.leftStickY * 0.0666666f)); } //Y-Up
					if (gamepad.leftStickX < -0.1) { yaw = (gamepad.leftStickX * -0.0666666f); } //X-Left
					if (gamepad.leftStickX > 00.1) { yaw = (gamepad.leftStickX * -0.0666666f); } //X-Right

					if (gamepad.rightStickY > 00.1) { pitch = gamepad.rightStickY * 0.0666666f; }
					if (gamepad.rightStickY < -0.1) { pitch = gamepad.rightStickY * 0.0666666f; }
					if (gamepad.rightStickX < -0.1) { yaw = (gamepad.rightStickX * -0.0666666f); } //X-Left
					if (gamepad.rightStickX > 00.1) { yaw = (gamepad.rightStickX * -0.0666666f); } //X-Right
				}
				else {
					if (gamepad.leftStickY < -0.1) { Tmovement = -(MovementModifier * (gamepad.leftStickY * 0.0666666f)); } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
					if (gamepad.leftStickY > 00.1) { Tmovement = -(MovementModifier * (gamepad.leftStickY * 0.0666666f)); } //Y-Up
					if (gamepad.leftStickX < -0.1) { yaw = -(gamepad.leftStickX * -0.0666666f); } //X-Left
					if (gamepad.leftStickX > 00.1) { yaw = -(gamepad.leftStickX * -0.0666666f); } //X-Right

					if (gamepad.rightStickY > 00.1) { pitch = -gamepad.rightStickY * 0.0666666f; }
					if (gamepad.rightStickY < -0.1) { pitch = -gamepad.rightStickY * 0.0666666f; }
					if (gamepad.rightStickX < -0.1) { yaw = -(gamepad.rightStickX * -0.0666666f); } //X-Left
					if (gamepad.rightStickX > 00.1) { yaw = -(gamepad.rightStickX * -0.0666666f); } //X-Right
				}

				//yaw
				rightVector = glm::cross(forwardVector[portNumber], glm::vec3(0.0f, 1.0f, 0.0f));
				rightVector = glm::normalize(rightVector);
				forwardVector[portNumber] = glm::rotate(forwardVector[portNumber], yaw, glm::vec3(0.0f, 1.0f, 0.0f));
				//pitch
				rightVector = glm::cross(forwardVector[portNumber], glm::vec3(0.0f, 1.0f, 0.0f));
				rightVector = glm::normalize(rightVector);
				forwardVector[portNumber] = glm::rotate(forwardVector[portNumber], pitch, rightVector);
				//player position
				glm::vec3 forwardVectorTemp = glm::vec3(forwardVector[portNumber].x, 0.0f, forwardVector[portNumber].z);
				Players[portNumber].ForwardDirection = (forwardVectorTemp * -0.001f);
				Players[portNumber].setForceOnObject((Tmovement * forwardVectorTemp)*0.5f);
				Players[portNumber].setVelocity((Tmovement * forwardVectorTemp)*0.5f*Players[portNumber].SprintSpeed);
				//camera position
				cameraPosition[portNumber] = glm::vec3(Players[portNumber].Position().x - (forwardVector[portNumber].x*6.0f),
					Players[portNumber].Position().y + (Players[portNumber].Radius().y*2.0f),
					Players[portNumber].Position().z - (forwardVector[portNumber].z*6.0f));
				//shockwace position
			}
		}
		//Buttons
		if (portNumber >= 0) {
			if (MenuSwitchCounter[portNumber] > 0.0f) { MenuSwitchCounter[portNumber] -= deltaTasSeconds; }
			else {
				//first press of [LEFT_TRIGGERED]
				if (gamepad.leftTrigger > 0.2 && Left_TRIGGERED[portNumber] == false) { Left_TRIGGERED[portNumber] = true; std::cout << "[LEFT_TRIGGERED][-]"; }
				//[LEFT_TRIGGERED] was pressed last tic
				else if (Left_TRIGGERED[portNumber] == true) {
					//holding [LEFT_TRIGGERED]
					if (gamepad.leftTrigger > 0.2) {}
					//[LEFT_TRIGGERED] released
					else {
						Left_TRIGGERED[portNumber] = false;
						MenuSwitchCounter[portNumber] = 0.70f;
						std::cout << "[LEFT_TRIGGERED][+]";
					}
				}

				//first press of [RIGHT_TRIGGERED]
				if (gamepad.rightTrigger > 0.2 && Right_TRIGGERED[portNumber] == false) { Right_TRIGGERED[portNumber] = true; std::cout << "[RIGHT_TRIGGERED][-]"; }
				//[RIGHT_TRIGGERED] was pressed last tic
				else if (Right_TRIGGERED[portNumber] == true) {
					//holding [RIGHT_TRIGGERED]
					if (gamepad.rightTrigger > 0.2) {
						PShockWaveChargeUp[portNumber] += deltaTasSeconds;
						//std::cout << "	[C](" << portNumber << ")[" << PShockWaveChargeUp[portNumber] << "]" << std::endl;
					}
					//[RIGHT_TRIGGERED] released
					else {
						if (PShockWaveChargeUp[portNumber] < 0.50f) { PShockWaveChargeUp[portNumber] = 0.0f; }
						Right_TRIGGERED[portNumber] = false;
						PShockWave[portNumber] = true;
						PShockWaveCounter[portNumber] = 0.250f;
						MenuSwitchCounter[portNumber] = 0.60f;
						std::cout << "[RIGHT_TRIGGERED][+]";
					}
					Players[portNumber].setVelocity(Players[portNumber].Velocity()*0.5f);
				}

				////first press of [A]
				//if (gamepad.IsPressed(XINPUT_GAMEPAD_A) && AButtonDown == false) { AButtonDown = true; std::cout << "[A][-]"; }
				////[A] was pressed last tic
				//else if (gamepad.WasPressed(XINPUT_GAMEPAD_A) && AButtonDown == true) {
				//	//holding [A]
				//	if (gamepad.IsPressed(XINPUT_GAMEPAD_A)) {
				//		PShockWaveChargeUp[portNumber] += deltaTasSeconds;
				//		std::cout << "	(" << portNumber << ")[" << PShockWaveChargeUp[portNumber] << "]" << std::endl;
				//	}
				//	//[A] released
				//	else {
				//		if (PShockWaveChargeUp[portNumber] < 0.50f) { PShockWaveChargeUp[portNumber] = 0.0f; }
				//		AButtonDown = false;
				//		PShockWave[portNumber] = true;
				//		PShockWaveCounter[portNumber] = 0.250f;
				//		MenuSwitchCounter[portNumber] = 0.70f;
				//		std::cout << "[A][+]";
				//	}
				//	Players[portNumber].setVelocity(Players[portNumber].Velocity()*0.5f);
				//}

				if (gamepad.IsPressed(XINPUT_GAMEPAD_A)) {
					if (Players[portNumber].inAir == false && Players[portNumber].IsJumping == false) {
						Players[portNumber].inAir = true; Players[portNumber].IsJumping = true;
						Players[portNumber].onObject = false;
						Players[portNumber].InAirCounter = 0.25f;
						Players[portNumber].setPosition(glm::vec3(Players[portNumber].Position().x, Players[portNumber].Position().y + (Players[portNumber].Radius().y * 0.30f), Players[portNumber].Position().z));
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_B)) {
					if (PSprintCounter[portNumber] < PSprintCoolDown && Players[portNumber].inAir == false) {
						PSprintCounter[portNumber] += deltaTasSeconds;
						Players[portNumber].setVelocity(Players[portNumber].Velocity()*SprintSpeed);
					}
				}
				else {
					if (PSprintCounter[portNumber] > 0.0f && Players[portNumber].inAir == false) {
						PSprintCounter[portNumber] -= deltaTasSeconds;
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_X)) { std::cout << "[X][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_Y)) {
					Players[portNumber].setVelocity(Players[portNumber].Velocity()*0.50f);
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_THUMB)) { std::cout << "[LEFT_THUMB][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_THUMB)) { std::cout << "[RIGHT_THUMB][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_UP)) { std::cout << "[DPAD_UP][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN)) { std::cout << "[DPAD_DOWN][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT)) { std::cout << "[DPAD_LEFT][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT)) { std::cout << "[DPAD_RIGHT][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_START)) {
					std::cout << "[START][-]";
					while (1) {
						gamepad.Refresh();
						if (gamepad.IsPressed(XINPUT_GAMEPAD_START) && gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) {
							break;
						}
					}
					MenuSwitchCounter[portNumber] = 1.0f;
				}
				else if (gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) {
					std::cout << "[BACK][-]";
					inGame = false; inMenu = true;
					MenuSwitchCounter[portNumber] = 1.0f;
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)) { std::cout << "[LEFT_SHOULDER][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)) { std::cout << "[RIGHT_SHOULDER][-]"; }
			}
		}
	}
	else if (inMenu || inOptions) {
		MovementModifier = MovementModifier*10.0f;

		if (mousepositionX + screenpositionX > GetSystemMetrics(SM_CXSCREEN)) { mousepositionX = GetSystemMetrics(SM_CXSCREEN) - screenpositionX; }
		else if (mousepositionX + screenpositionX < 0) { mousepositionX = 0 - screenpositionX; }
		if (mousepositionY + screenpositionY > GetSystemMetrics(SM_CYSCREEN)) { mousepositionY = GetSystemMetrics(SM_CYSCREEN) - screenpositionY; }
		else if (mousepositionY + screenpositionY < 0) { mousepositionY = 0 - screenpositionY; }

		if (portNumber >= 0) {
			float Tx = 0.0f; float Ty = 0.0f; float Tz = 0.0f; float rotY = 0.0f;
			//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
			if (gamepad.leftStickY < -0.08f) { Ty -= gamepad.leftStickY * 0.0666666f; } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
			if (gamepad.leftStickY > 00.08f) { Ty -= gamepad.leftStickY * 0.0666666f; } //Y-Up
			if (gamepad.leftStickX < -0.08f) { Tx -= gamepad.leftStickX * 0.0666666f; } //X-Left
			if (gamepad.leftStickX > 00.08f) { Tx -= gamepad.leftStickX * 0.0666666f; } //X-Right

			if (gamepad.rightStickY < -0.10f) { Ty -= gamepad.rightStickY * 0.0366666f; } //Y-Down
			if (gamepad.rightStickY > 00.10f) { Ty -= gamepad.rightStickY * 0.0366666f; } //Y-Up
			if (gamepad.rightStickX < -0.10f) { Tx -= gamepad.rightStickX * 0.0366666f; } //X-Left
			if (gamepad.rightStickX > 00.10f) { Tx -= gamepad.rightStickX * 0.0366666f; } //X-Right

			Tx = (Tx*MovementModifier);
			Ty = (Ty*MovementModifier);
			Tz = (Tz*MovementModifier);

			mousepositionX -= static_cast<int>(Tx); mousepositionY += static_cast<int>(Ty);
			//
			//std::cout << "[" << screen_pos_x << "] [" << screen_pos_y << "]" << std::endl;
			if (Tx != 0.0f || Ty != 0.0f || Tz != 0.0f) { SetCursorPos(glutGet((GLenum)GLUT_WINDOW_X) + mousepositionX, glutGet((GLenum)GLUT_WINDOW_Y) + mousepositionY); }

			if (MenuSwitchCounter[portNumber] > 0.0f) { MenuSwitchCounter[portNumber] -= deltaTasSeconds; }
			else {
				if (gamepad.IsPressed(XINPUT_GAMEPAD_A)) {
					std::cout << "[A]";
					INPUT input;
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP);
					input.mi.mouseData = 0;
					input.mi.dwExtraInfo = NULL;
					input.mi.time = 0;
					SendInput(1, &input, sizeof(INPUT));

					bool pressedASlider = false;
					for (unsigned int i = 0; i < NumberOfSliders; i++) {
						//move nob along the slider
						if (Slider[i].moveNob(MPosToOPosX, MPosToOPosY)) { ButtonForSliders[i].setPosition(glm::vec3(MPosToOPosX, 0.02f, Slider[i].SBar_Pos.z)); pressedASlider = true; }
					}
					if (!pressedASlider) { MenuSwitchCounter[portNumber] = 0.40f; }
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_B)) {
					std::cout << "[B]";
					INPUT input;
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP);
					input.mi.mouseData = 0;
					input.mi.dwExtraInfo = NULL;
					input.mi.time = 0;
					SendInput(1, &input, sizeof(INPUT));

					MenuSwitchCounter[portNumber] = 1.0f;
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_X)) { std::cout << "[X]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_Y)) { std::cout << "[Y]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_UP)) { std::cout << "[DPAD_UP]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN)) { std::cout << "[DPAD_DOWN]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT)) { std::cout << "[DPAD_LEFT]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT)) { std::cout << "[DPAD_RIGHT]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_START)) {
					std::cout << "[START]";
					setBoardStart();
					inMenu = false; inGame = true;
					MenuSwitchCounter[portNumber] = 1.0f;
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) {
					std::cout << "[BACK]";
					if (inOptions) {
						if (inOptionsTab != 0) { inOptionsTab = 0; }
						else { inOptions = false; inMenu = true; }
						MenuSwitchCounter[portNumber] = 1.0f;
					}
					else { exitProgram(); }
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)) { std::cout << "[LEFT_SHOULDER]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)) { std::cout << "[RIGHT_SHOULDER]"; }
			}
		}
	}
}

/* function KeyBoardDelayButton()
* Description:
*   - this is called to get inputs from the keyboard
*	- only called during the game
*/
void KeyBoardDelayButton(float deltaTasSeconds) {

	for (int playerNumberControl = 0; playerNumberControl < NumberOfPlayers; playerNumberControl++) {
		if (cameraMode == 0) {
			float Tx[2]{ 0.0f,0.0f }; float Ty[2]{ 0.0f,0.0f }; float Tz[2]{ 0.0f,0.0f };

			if (MenuSwitchCounter[playerNumberControl] > 0.0f) { MenuSwitchCounter[playerNumberControl] -= deltaTasSeconds; }
			else {
				if (playerNumberControl == 0) {
					//player One
					if (!FlipedControllers[playerNumberControl]) {
						if (keyDown['w'] || keyDown['W']) { Tx[playerNumberControl] = -0.20f; }
						if (keyDown['s'] || keyDown['S']) { Tx[playerNumberControl] = 00.20f; }
						if (keyDown['a'] || keyDown['A']) { Tz[playerNumberControl] = 00.20f; }
						if (keyDown['d'] || keyDown['D']) { Tz[playerNumberControl] = -0.20f; }
					}
					else {
						if (keyDown['w'] || keyDown['W']) { Tx[playerNumberControl] = 00.20f; }
						if (keyDown['s'] || keyDown['S']) { Tx[playerNumberControl] = -0.20f; }
						if (keyDown['a'] || keyDown['A']) { Tz[playerNumberControl] = -0.20f; }
						if (keyDown['d'] || keyDown['D']) { Tz[playerNumberControl] = 00.20f; }
					}


					if (keyDown['q'] || keyDown['Q']) {
						if (Players[playerNumberControl].inAir == false && Players[playerNumberControl].IsJumping == false) {
							Players[playerNumberControl].inAir = true; Players[playerNumberControl].IsJumping = true;
							Players[playerNumberControl].onObject = false;
							Players[playerNumberControl].InAirCounter = 0.25f;
							Players[playerNumberControl].setPosition(glm::vec3(Players[playerNumberControl].Position().x, Players[playerNumberControl].Position().y + (Players[playerNumberControl].Radius().y * 0.30f), Players[playerNumberControl].Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['e'] || keyDown['E']) && Right_TRIGGERED[playerNumberControl] == false) { Right_TRIGGERED[playerNumberControl] = true; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (Right_TRIGGERED[playerNumberControl] == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['e'] || keyDown['E']) {
							PShockWaveChargeUp[playerNumberControl] += deltaTasSeconds;
							//std::cout << "	[C](" << 0 << ")[" << PShockWaveChargeUp[0] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PShockWaveChargeUp[playerNumberControl] < 0.50f) { PShockWaveChargeUp[playerNumberControl] = 0.0f; }
							Right_TRIGGERED[playerNumberControl] = false;
							PShockWave[playerNumberControl] = true;
							PShockWaveCounter[playerNumberControl] = 0.250f;
							MenuSwitchCounter[playerNumberControl] = 0.60f;
							//std::cout << "[RIGHT_TRIGGERED][+]";
						}
						Players[playerNumberControl].setVelocity(Players[playerNumberControl].Velocity()*0.5f);
					}
				}
				else if (playerNumberControl == 1) {
					//player Two
					if (!FlipedControllers[playerNumberControl]) {
						if (keyDown['i'] || keyDown['I']) { Tx[playerNumberControl] = 00.20f; }
						if (keyDown['k'] || keyDown['K']) { Tx[playerNumberControl] = -0.20f; }
						if (keyDown['j'] || keyDown['J']) { Tz[playerNumberControl] = -0.20f; }
						if (keyDown['l'] || keyDown['L']) { Tz[playerNumberControl] = 00.20f; }
					}
					else {
						if (keyDown['i'] || keyDown['I']) { Tx[playerNumberControl] = -0.20f; }
						if (keyDown['k'] || keyDown['K']) { Tx[playerNumberControl] = 00.20f; }
						if (keyDown['j'] || keyDown['J']) { Tz[playerNumberControl] = 00.20f; }
						if (keyDown['l'] || keyDown['L']) { Tz[playerNumberControl] = -0.20f; }
					}
					if (keyDown['o'] || keyDown['O']) {
						if (Players[playerNumberControl].inAir == false && Players[playerNumberControl].IsJumping == false) {
							Players[playerNumberControl].inAir = true; Players[playerNumberControl].IsJumping = true;
							Players[playerNumberControl].onObject = false;
							Players[playerNumberControl].InAirCounter = 0.25f;
							Players[playerNumberControl].setPosition(glm::vec3(Players[playerNumberControl].Position().x, Players[playerNumberControl].Position().y + (Players[playerNumberControl].Radius().y * 0.30f), Players[playerNumberControl].Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['U'] || keyDown['u']) && Right_TRIGGERED[playerNumberControl] == false) { Right_TRIGGERED[playerNumberControl] = true; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (Right_TRIGGERED[playerNumberControl] == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['U'] || keyDown['u']) {
							PShockWaveChargeUp[playerNumberControl] += deltaTasSeconds;
							//std::cout << "	[C](" << 1 << ")[" << PShockWaveChargeUp[1] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PShockWaveChargeUp[playerNumberControl] < 0.50f) { PShockWaveChargeUp[playerNumberControl] = 0.0f; }
							Right_TRIGGERED[playerNumberControl] = false;
							PShockWave[playerNumberControl] = true;
							PShockWaveCounter[playerNumberControl] = 0.250f;
							MenuSwitchCounter[playerNumberControl] = 0.60f;
							//std::cout << "[RIGHT_TRIGGERED][+]";
						}
						Players[playerNumberControl].setVelocity(Players[playerNumberControl].Velocity()*0.5f);
					}
				}


				Players[playerNumberControl].setForceOnObject(glm::vec3(Tx[playerNumberControl], Ty[playerNumberControl], Tz[playerNumberControl]));
				Players[playerNumberControl].setVelocity(glm::vec3(Tx[playerNumberControl], Ty[playerNumberControl], Tz[playerNumberControl])*SprintSpeed);
				Players[playerNumberControl].ForwardDirection = (Players[playerNumberControl].Position() - (Players[playerNumberControl].Position() + Players[playerNumberControl].Velocity()));
			}
		}
		else if (cameraMode == 1) {
			float Tmovement[2]{ 0.0f,0.0f }; float yaw[2]{ 0.0f,0.0f }; float pitch[2]{ 0.0f,0.0f };


			if (MenuSwitchCounter[playerNumberControl] > 0.0f) { MenuSwitchCounter[playerNumberControl] -= deltaTasSeconds; }
			else {
				if (playerNumberControl == 0) {
					//player One //checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
					if (!FlipedControllers[playerNumberControl]) {
						if (keyDown['w'] || keyDown['W']) { Tmovement[playerNumberControl] = 00.50f; }
						if (keyDown['s'] || keyDown['S']) { Tmovement[playerNumberControl] = -0.50f; }
						if (keyDown['a'] || keyDown['A']) { yaw[playerNumberControl] = 00.05f; }
						if (keyDown['d'] || keyDown['D']) { yaw[playerNumberControl] = -0.05f; }
					}
					else {
						if (keyDown['w'] || keyDown['W']) { Tmovement[playerNumberControl] = -0.50f; }
						if (keyDown['s'] || keyDown['S']) { Tmovement[playerNumberControl] = 00.50f; }
						if (keyDown['a'] || keyDown['A']) { yaw[playerNumberControl] = -0.05f; }
						if (keyDown['d'] || keyDown['D']) { yaw[playerNumberControl] = 00.05f; }
					}

					if (keyDown['q'] || keyDown['Q']) {
						if (Players[playerNumberControl].inAir == false && Players[playerNumberControl].IsJumping == false) {
							Players[playerNumberControl].inAir = true; Players[playerNumberControl].IsJumping = true;
							Players[playerNumberControl].onObject = false;
							Players[playerNumberControl].InAirCounter = 0.25f;
							Players[playerNumberControl].setPosition(glm::vec3(Players[playerNumberControl].Position().x, Players[playerNumberControl].Position().y + (Players[playerNumberControl].Radius().y * 0.30f), Players[playerNumberControl].Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['e'] || keyDown['E']) && Right_TRIGGERED[playerNumberControl] == false) { Right_TRIGGERED[playerNumberControl] = true; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (Right_TRIGGERED[playerNumberControl] == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['e'] || keyDown['E']) {
							PShockWaveChargeUp[playerNumberControl] += deltaTasSeconds;
							//std::cout << "	[C](" << 0 << ")[" << PShockWaveChargeUp[0] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PShockWaveChargeUp[playerNumberControl] < 0.50f) { PShockWaveChargeUp[playerNumberControl] = 0.0f; }
							Right_TRIGGERED[playerNumberControl] = false;
							PShockWave[playerNumberControl] = true;
							PShockWaveCounter[playerNumberControl] = 0.250f;
							MenuSwitchCounter[playerNumberControl] = 0.60f;
							//std::cout << "[RIGHT_TRIGGERED][+]";
						}
						Players[playerNumberControl].setVelocity(Players[playerNumberControl].Velocity()*0.5f);
					}
				}
				if (playerNumberControl == 1) {
					//player Two
					if (!FlipedControllers[playerNumberControl]) {
						if (keyDown['i'] || keyDown['I']) { Tmovement[playerNumberControl] = 00.50f; }
						if (keyDown['k'] || keyDown['K']) { Tmovement[playerNumberControl] = -0.50f; }
						if (keyDown['j'] || keyDown['J']) { yaw[playerNumberControl] = 00.05f; }
						if (keyDown['l'] || keyDown['L']) { yaw[playerNumberControl] = -0.05f; }
					}
					else {
						if (keyDown['i'] || keyDown['I']) { Tmovement[playerNumberControl] = -0.50f; }
						if (keyDown['k'] || keyDown['K']) { Tmovement[playerNumberControl] = 00.50f; }
						if (keyDown['j'] || keyDown['J']) { yaw[playerNumberControl] = -0.05f; }
						if (keyDown['l'] || keyDown['L']) { yaw[playerNumberControl] = 00.05f; }
					}
					if (keyDown['o'] || keyDown['O']) {
						if (Players[playerNumberControl].inAir == false && Players[playerNumberControl].IsJumping == false) {
							Players[playerNumberControl].inAir = true; Players[playerNumberControl].IsJumping = true;
							Players[playerNumberControl].onObject = false;
							Players[playerNumberControl].InAirCounter = 0.25f;
							Players[playerNumberControl].setPosition(glm::vec3(Players[playerNumberControl].Position().x, Players[playerNumberControl].Position().y + (Players[playerNumberControl].Radius().y * 0.30f), Players[playerNumberControl].Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['U'] || keyDown['u']) && Right_TRIGGERED[playerNumberControl] == false) { Right_TRIGGERED[playerNumberControl] = true; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (Right_TRIGGERED[playerNumberControl] == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['U'] || keyDown['u']) {
							PShockWaveChargeUp[playerNumberControl] += deltaTasSeconds;
							//std::cout << "	[C](" << 1 << ")[" << PShockWaveChargeUp[1] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PShockWaveChargeUp[playerNumberControl] < 0.50f) { PShockWaveChargeUp[playerNumberControl] = 0.0f; }
							Right_TRIGGERED[playerNumberControl] = false;
							PShockWave[playerNumberControl] = true;
							PShockWaveCounter[playerNumberControl] = 0.250f;
							MenuSwitchCounter[playerNumberControl] = 0.60f;
							//std::cout << "[RIGHT_TRIGGERED][+]";
						}
						Players[playerNumberControl].setVelocity(Players[playerNumberControl].Velocity()*0.5f);
					}
				}
			}

			//yaw
			rightVector = glm::cross(forwardVector[playerNumberControl], glm::vec3(0.0f, 1.0f, 0.0f));
			rightVector = glm::normalize(rightVector);
			forwardVector[playerNumberControl] = glm::rotate(forwardVector[playerNumberControl], yaw[playerNumberControl], glm::vec3(0.0f, 1.0f, 0.0f));
			//pitch
			rightVector = glm::cross(forwardVector[playerNumberControl], glm::vec3(0.0f, 1.0f, 0.0f));
			rightVector = glm::normalize(rightVector);
			forwardVector[playerNumberControl] = glm::rotate(forwardVector[playerNumberControl], pitch[playerNumberControl], rightVector);
			//player position
			glm::vec3 forwardVectorTemp = glm::vec3(forwardVector[playerNumberControl].x, 0.0f, forwardVector[playerNumberControl].z);
			Players[playerNumberControl].ForwardDirection = (forwardVectorTemp * -0.001f);
			Players[playerNumberControl].setForceOnObject((Tmovement[playerNumberControl] * forwardVectorTemp)*0.5f);
			Players[playerNumberControl].setVelocity((Tmovement[playerNumberControl] * forwardVectorTemp)*0.5f*SprintSpeed);
			//camera position
			cameraPosition[playerNumberControl] = glm::vec3(Players[playerNumberControl].Position().x - (forwardVector[playerNumberControl].x*6.0f),
				Players[playerNumberControl].Position().y + (Players[playerNumberControl].Radius().y*2.0f),
				Players[playerNumberControl].Position().z - (forwardVector[playerNumberControl].z*6.0f));
			//shockwace position
		}
	}
}



//////////////////////////////////////////////////////////////////////



/* function InMenuDraw()
* Description:
*  - Draws the menu screen
*/
void InMenuDraw(int Inum)
{
	auto defaultMaterial = materials["default"];
	auto passThroughMaterial = materials["passThrough"];
	auto textMaterial = materials["text"];



	passThroughMaterial->shader->bind();
	Inum = 2;
	cameralook = Inum; //window
	WhatCameraIsLookingAt(); //Resising Window
	passThroughMaterial->shader->sendUniformMat4("mvm", modelViewMatrix[Inum]);
	passThroughMaterial->shader->sendUniformMat4("prm", projectionMatrix[Inum]);
	passThroughMaterial->shader->sendUniformMat4("u_mvp", (modelViewMatrix[Inum] * projectionMatrix[Inum]));
	passThroughMaterial->shader->sendUniformMat4("u_mv", modelViewMatrix[Inum]);
	passThroughMaterial->shader->sendUniformMat4("u_lightPos_01", (modelViewMatrix[Inum] * glm::translate(glm::mat4(1.0f), lightPosition_01)));
	passThroughMaterial->shader->sendUniformMat4("u_lightPos_02", (modelViewMatrix[Inum] * glm::translate(glm::mat4(1.0f), lightPosition_02)));

	if (planeForText[0].Viewable) { planeForText[0].drawObject(); }
	for (unsigned int i = 0; i <= 2; i++) {
		if (ButtonObjects[i].Viewable) { ButtonObjects[i].drawObject(); }
	}

	//passThroughMaterial->shader->unbind();
}

/* function MenuScreen()
* Description:
*  - does all the functions/calculations for the menu screen
*/
void MenuScreen(float deltaTasSeconds)
{
	//Sound::SystemUpdate();

	systemSound.SystemUpdate();

	FMOD_VECTOR drumPos; drumPos.x = 0; drumPos.y = 0; drumPos.z = 0;
	FMOD_VECTOR drumVel; drumVel.x = 12.0f; drumVel.y = 0.0f; drumVel.z = 0.0f;

	powerup[5].SetPosition(powChannel, drumPos, drumVel);
	Fx[0].SetPosition(FxChannel[0], drumPos, drumVel);
	Fx[1].SetPosition(FxChannel[1], drumPos, drumVel);

	if (mouseDown[0]) {
		mouseDown[0] = false;
		if (Button[0].button(MPosToOPosX, MPosToOPosY)) { setBoardStart(); inGame = true; inMenu = false; }
		if (Button[1].button(MPosToOPosX, MPosToOPosY)) { inOptions = true; inMenu = false; }
		if (Button[2].button(MPosToOPosX, MPosToOPosY)) { exitProgram(); }
	}
	if (mouseDown[1]) {
		mouseDown[1] = false;
	}
	if (mouseDown[2]) {
		mouseDown[2] = false; 
	}


	
}

/* function InOptionDraw()
* Description:
*  - Draws the options screen
*/
void InOptionDraw(int Inum)
{
	auto defaultMaterial = materials["default"];
	auto passThroughMaterial = materials["passThrough"];
	auto textMaterial = materials["text"];


	passThroughMaterial->shader->bind();
	Inum = 2;
	cameralook = Inum; //window
	WhatCameraIsLookingAt(); //Resising Window

	// Draw our scene
	passThroughMaterial->shader->sendUniformMat4("mvm", modelViewMatrix[Inum]);
	passThroughMaterial->shader->sendUniformMat4("prm", projectionMatrix[Inum]);
	passThroughMaterial->shader->sendUniformMat4("u_mvp", (modelViewMatrix[Inum] * projectionMatrix[Inum]));
	passThroughMaterial->shader->sendUniformMat4("u_mv", modelViewMatrix[Inum]);
	passThroughMaterial->shader->sendUniformMat4("u_lightPos_01", (modelViewMatrix[Inum] * glm::translate(glm::mat4(1.0f), lightPosition_01)));
	passThroughMaterial->shader->sendUniformMat4("u_lightPos_02", (modelViewMatrix[Inum] * glm::translate(glm::mat4(1.0f), lightPosition_02)));

	


	if (inOptionsTab == 0) {
		if (planeForText[inOptionsTab + 1].Viewable) { planeForText[inOptionsTab + 1].drawObject(); }
		if (ButtonObjects[3].Viewable) { ButtonObjects[3].drawObject(); }
		for (unsigned int i = 4; i <= 6; i++) {
			if (ButtonObjects[i].Viewable) { ButtonObjects[i].drawObject(); }
		}
	}
	else if (inOptionsTab == 1) {
		if (planeForText[inOptionsTab + 1].Viewable) { planeForText[inOptionsTab + 1].drawObject(); }
		if (ButtonObjects[3].Viewable) { ButtonObjects[3].drawObject(); }
	}
	else if (inOptionsTab == 2) {
		if (planeForText[inOptionsTab + 1].Viewable) { planeForText[inOptionsTab + 1].drawObject(); }
		if (ButtonObjects[3].Viewable) { ButtonObjects[3].drawObject(); }
	}
	else if (inOptionsTab == 3) {
		if (planeForText[inOptionsTab + 1].Viewable) { planeForText[inOptionsTab + 1].drawObject(); }
		if (ButtonObjects[3].Viewable) { ButtonObjects[3].drawObject(); }
		for (unsigned int i = 0; i < NumberOfSliders; i++) {
			if (ButtonForSliders[i].Viewable) { ButtonForSliders[i].drawObject(); }
			if (planeForSliders[i].Viewable) { planeForSliders[i].drawObject(); }
		}
	}

	//passThroughMaterial->shader->unbind();

}

/* function OptionScreen()
* Description:
*  - does all the functions/calculations for the options screen
*/
void OptionScreen(float deltaTasSeconds)
{
	Sound::SystemUpdate();

	if (mouseDown[0]) {
		mouseDown[0] = false; 
		if (inOptionsTab == 0) {
			if (Button[4].button(MPosToOPosX, MPosToOPosY)) { inOptionsTab = 1; }
			if (Button[5].button(MPosToOPosX, MPosToOPosY)) { inOptionsTab = 2; }
			if (Button[6].button(MPosToOPosX, MPosToOPosY)) { inOptionsTab = 3; }
			if (Button[3].button(MPosToOPosX, MPosToOPosY)) { inOptions = false; inMenu = true; }
		}
		else if (inOptionsTab == 1) {
			if (Button[3].button(MPosToOPosX, MPosToOPosY)) { inOptionsTab = 0; }
		}
		else if (inOptionsTab == 2) {
			if (Button[3].button(MPosToOPosX, MPosToOPosY)) { inOptionsTab = 0; }
		}
		else if (inOptionsTab == 3) {
			if (Button[3].button(MPosToOPosX, MPosToOPosY)) { inOptionsTab = 0; }
			for (unsigned int i = 0; i < NumberOfSliders; i++) {
				//move nob along the slider
				if (Slider[i].moveNob(MPosToOPosX, MPosToOPosY)) { ButtonForSliders[i].setPosition(glm::vec3(MPosToOPosX, 0.02f, Slider[i].SBar_Pos.z)); }
			}
		}
	}
	if (mouseDown[1]) {
		mouseDown[1] = false;
	}
	if (mouseDown[2]) {
		mouseDown[2] = false;
	}


	if (inOptionsTab == 0) {
	
	}
	else if (inOptionsTab == 1) {

	}
	else if (inOptionsTab == 2) {
	
	}
	else if (inOptionsTab == 3) {
		TestFloat = Slider[0].SNob_Precent.x;
		for (int i = NumberOfEnemies-1; i > (NumberOfEnemies - static_cast<int>(Slider[1].SNob_Precent.x / 100)); i--) {
			Enemies[i].Viewable = false;
		}
		//NumberOfEnemies
		Health[0] = Health[1] = static_cast<int>(Slider[2].SNob_Precent.x);
	}
}

/* function InGameDraw()
* Description:
*  - Draws the game screen
*/
void InGameDraw(int Inum)
{
	auto defaultMaterial = materials["default"];
	auto passThroughMaterial = materials["passThrough"];
	auto textMaterial = materials["text"];


	passThroughMaterial->shader->bind();
	cameralook = Inum; //window
	WhatCameraIsLookingAt(); //Resising Window
	
	//Draw scene //cameraViewMatrix //modelViewMatrix	
	passThroughMaterial->shader->sendUniformMat4("mvm", modelViewMatrix[PlayerTeam[Inum]]);
	passThroughMaterial->shader->sendUniformMat4("prm", projectionMatrix[PlayerTeam[Inum]]);
	passThroughMaterial->shader->sendUniformMat4("u_mvp", (modelViewMatrix[PlayerTeam[Inum]] * projectionMatrix[PlayerTeam[Inum]]));
	passThroughMaterial->shader->sendUniformMat4("u_mv", modelViewMatrix[PlayerTeam[Inum]]);
	passThroughMaterial->shader->sendUniformMat4("u_lightPos_01", (modelViewMatrix[PlayerTeam[Inum]] * glm::translate(glm::mat4(1.0f), lightPosition_01)));
	passThroughMaterial->shader->sendUniformMat4("u_lightPos_02", (modelViewMatrix[PlayerTeam[Inum]] * glm::translate(glm::mat4(1.0f), lightPosition_02)));

	//glFramebufferTexture2D();

	for (unsigned int i = 0; i < NumberOfPlayers; i++) {
		if (Players[i].Viewable) {
			//Players
			if (Players[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Players[i].drawObject();
			//Shadows
			if (EnableShadows) {
				if (ShadowObject[0].textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				ShadowObject[0].setPosition(glm::vec3(Players[i].Position().x, 0.01f, Players[i].Position().z));
				ShadowObject[0].setScale(Players[i].Scale());
				ShadowObject[0].setRotation(Players[i].Angle());
				ShadowObject[0].drawObject();
			}

			for (unsigned int j = 0; j < NumberOfAffects; j++) {
				if (AffectsOnPlayer[i][j].textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				AffectsOnPlayer[i][j].setPosition(Players[i].Position());
				AffectsOnPlayer[i][j].drawObject();
			}

			//Players[i].drawHTR(shader);
		}
	}
	for (unsigned int i = 0; i < NumberOfRifts; i++) {
		if (Rifts[i].Viewable) {
			//Rifts
			if (Rifts[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Rifts[i].drawObject();
		}
	}
	for (unsigned int i = 0; i < NumberOfObjects; i++) {
		if (Objects[i].Viewable) {
			if (Objects[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Objects[i].drawObject();
		}
	}
	for (unsigned int i = 0; i < NumberOfEnemies; i++) {
		if (Enemies[i].Viewable) {
			//Enemies
			if (Enemies[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Enemies[i].drawObject();
			//Shadows
			if (EnableShadows) {
				if (ShadowObject[0].textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				ShadowObject[0].setPosition(glm::vec3(Enemies[i].Position().x, 0.01f, Enemies[i].Position().z));

				if ((Enemies[i].Position().y*0.05f) > 1.50f) { ShadowObject[0].setScale(Enemies[i].Scale()*1.50f); }
				else if ((Enemies[i].Position().y*0.05f) < 1.0f) { ShadowObject[0].setScale(Enemies[i].Scale()*1.0f); }
				else { ShadowObject[0].setScale(Enemies[i].Scale()*(Enemies[i].Position().y*0.05f)); }

				ShadowObject[0].setRotation(Enemies[i].Angle());
				ShadowObject[0].drawObject();
			}
		}
	}
	for (unsigned int i = 0; i < NumberOfSpecials; i++) {
		if (Specials[i].Viewable) {
			//Specials
			if (Specials[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Specials[i].drawObject();
			//Shadows
			if (EnableShadows) {
				if (ShadowObject[1].textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				ShadowObject[1].setPosition(glm::vec3(Specials[i].Position().x, 0.01f, Specials[i].Position().z));
				ShadowObject[1].setScale(Specials[i].Scale());
				ShadowObject[1].setRotation(Specials[i].Angle());
				ShadowObject[1].drawObject();
			}
		}
	}




	//passThroughMaterial->shader->unbind();

	cameralook = 3; //window
	WhatCameraIsLookingAt(); //Resising Window

	
	if (Inum == 0) {
		void TextDraw(Material &s, glm::mat4x4 *pvm, std::string text, float _x, float _y, float _scale, glm::vec3 color, int orientation);
		SystemText.TextDraw(*textMaterial, &projectionMatrix[3], "[" + std::to_string(Health[0]) + "]", -(windowWidth / 4.0f), -3.0f * (windowHeight / 8.0f), 1.0f, glm::vec3(0.3, 0.3f, 0.9f), 1);
		SystemText.TextDraw(*textMaterial, &projectionMatrix[3], "[" + std::to_string(Health[1]) + "]", -(windowWidth / 4.0f), (windowHeight / 2.0f) - (windowHeight / 8.0f), 1.0f, glm::vec3(0.9, 0.2f, 0.2f), 1);
	}
	else if (Inum == 1) {
		SystemText.TextDraw(*textMaterial, &projectionMatrix[3], "[" + std::to_string(Health[0]) + "]",  (windowWidth / 4.0f), (windowHeight / 2.0f) - (windowHeight / 8.0f), 1.0f, glm::vec3(0.3, 0.3f, 0.9f), 1);
		SystemText.TextDraw(*textMaterial, &projectionMatrix[3], "[" + std::to_string(Health[1]) + "]",  (windowWidth / 4.0f), -3.0f * (windowHeight / 8.0f), 1.0f, glm::vec3(0.9, 0.2f, 0.2f), 1);
	}
}

/* function GameField()
* Description:
*  - does all the functions/calculations for the game screen
*/
void GameScreen(float deltaTasSeconds) 
{
	//Intialize the crowd sound to play when the game starts
	FxChannel[1] = Fx[1].Play();
	FxChannel[1]->setVolume(0.1);

	for (int i = 0; i < 2; i++) {
		Sound::Sys.listenerPos[i].x = Players[i].Position().x*10.0f;
		Sound::Sys.listenerPos[i].y = Players[i].Position().y*10.0f;
		Sound::Sys.listenerPos[i].z = Players[i].Position().z*10.0f;
	}
	//systemSound.Sys.Init();
	systemSound.Sys.Update();
	


	//Collision between things
	if (CollisionBetweenObjects) {
		//std::cout << "[Hit][" << j << "]" << std::endl;

		//Specials to Borders
		for (int i = 0; i < NumberOfSpecials; i++) {
			m.A = Specials[i];
			bool InBorder = false;

			//check to see if player is in a bounding box
			for (int j = 0; j < NumberOfBorders; j++) {
				m.B = Borders[j];
				if (ObjectInBox(m)) { InBorder = true; }
			}

			if (InBorder) {}
			//if the player is not in any box, seek to nearest one
			else {
				float distanceToNearestBox; int boxNumber_1 = 0; glm::vec3 boxWithLargerRadius(0.0f); int boxNumber_2 = 0;
				distanceToNearestBox = glm::distance(m.A.Position(), Borders[0].Position());
				//find nearest bounding box
				for (int j = 0; j < NumberOfBorders; j++) {
					m.B = Borders[j];
					glm::vec3 distNearPos = glm::vec3(glm::distance(m.A.Position().x, m.B.Position().x), glm::distance(m.A.Position().y, m.B.Position().y), glm::distance(m.A.Position().z, m.B.Position().z));
					if ((distNearPos.x < m.B.Radius().x) && (distNearPos.y < m.B.Radius().y) && (distNearPos.z < m.B.Radius().z)) {
						//box with larger radius
						if (boxWithLargerRadius.x < m.B.Radius().x && boxWithLargerRadius.y < m.B.Radius().y && boxWithLargerRadius.z < m.B.Radius().z) {
							boxWithLargerRadius = m.B.Radius();
							boxNumber_2 = j;
						}
						//closest box
						if (distanceToNearestBox > glm::distance(m.A.Position(), m.B.Position())) {
							distanceToNearestBox = glm::distance(m.A.Position(), m.B.Position());
							boxNumber_1 = j;
						}
					}
				}
				//switch the m.A & m.B cause only m.B is affected
				m.B = Borders[boxNumber_2];
				if (CheckCollision(m)) { m.A = Borders[boxNumber_2]; m.B = Specials[i]; }
				else { m.A = Borders[boxNumber_1]; m.B = Specials[i]; }


				//determine the wall collision
				if (m.B.Position().x + (m.B.Radius().x*0.5f) > m.A.Top().x) {
					m.B.setPosition(glm::vec3(m.A.Top().x - (m.B.Radius().x*0.5f), m.B.Position().y, m.B.Position().z));
					m.B.setVelocity(glm::vec3(-(m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().x - (m.B.Radius().x*0.5f) < m.A.Bottom().x) {
					m.B.setPosition(glm::vec3(m.A.Bottom().x + (m.B.Radius().x*0.5f), m.B.Position().y, m.B.Position().z));
					m.B.setVelocity(glm::vec3(-(m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B.Position().y + (m.B.Radius().y*0.5f) > m.A.Top().y) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.A.Top().y - (m.B.Radius().y*0.5f), m.B.Position().z));
					m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), -(m.B.Velocity().y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().y - (m.B.Radius().y*0.5f) < m.A.Bottom().y) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.A.Bottom().y + (m.B.Radius().y*0.5f), m.B.Position().z));
					m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), -(m.B.Velocity().y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B.Position().z + (m.B.Radius().z*0.5f) > m.A.Top().z) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.B.Position().y, m.A.Top().z - (m.B.Radius().z*0.5f)));
					m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), -(m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().z - (m.B.Radius().z*0.5f) < m.A.Bottom().z) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.B.Position().y, m.A.Bottom().z + (m.B.Radius().z*0.5f)));
					m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), -(m.B.Velocity().z*speedToWallDegradation.z)));
				}
				////If Specials are nearing the walls
				//if (t_PPos.x > (t_BTop.x*0.95)) { FleeFromDirection(m, 1.0f, "x"); }
				//else if (t_PPos.x < (t_BBot.x*0.95)) { FleeFromDirection(m, 1.0f, "-x"); }
				//if (t_PPos.z > (t_BTop.z*0.95)) { FleeFromDirection(m, 1.0f, "z"); }
				//else if (t_PPos.z < (t_BBot.z*0.95)) { FleeFromDirection(m, 1.0f, "-z"); }
				Specials[i] = m.B;
			}
		}
		//Specials to Objects
		for (int i = 0; i < NumberOfSpecials; i++) {
			m.B = Specials[i];
			for (int j = 1; j < NumberOfObjects; j++) {
				m.A = Objects[j];
				if (checkRadialCollision(m)) { ResolveCollision(m, 0.1f); }
				Objects[j] = m.A;
			}//end for
			Specials[i] = m.B;
		}//end for

		 //Players to Borders
		for (int i = 0; i < NumberOfPlayers; i++) {
			m.A = Players[i];
			bool InBorder = false;

			//check to see if player is in a bounding box
			for (int j = 0; j < NumberOfBorders; j++) {
				m.B = Borders[j];
				if (ObjectInBox(m)) { InBorder = true; }
			}

			if (InBorder) { }
			//if the player is not in any box, seek to nearest one
			else {
				float distanceToNearestBox; int boxNumber_1 = 0; glm::vec3 boxWithLargerRadius(0.0f); int boxNumber_2 = 0;
				distanceToNearestBox = glm::distance(m.A.Position(), Borders[0].Position());
				//find nearest bounding box
				for (int j = 0; j < NumberOfBorders; j++) {
					m.B = Borders[j];
					glm::vec3 distNearPos = glm::vec3(glm::distance(m.A.Position().x, m.B.Position().x), glm::distance(m.A.Position().y, m.B.Position().y), glm::distance(m.A.Position().z, m.B.Position().z));
					if ((distNearPos.x < m.B.Radius().x) && (distNearPos.y < m.B.Radius().y) && (distNearPos.z < m.B.Radius().z)) {
						//box with larger radius
						if (boxWithLargerRadius.x < m.B.Radius().x && boxWithLargerRadius.y < m.B.Radius().y && boxWithLargerRadius.z < m.B.Radius().z) {
							boxWithLargerRadius = m.B.Radius();
							boxNumber_2 = j;
						}
						//closest box
						if (distanceToNearestBox > glm::distance(m.A.Position(), m.B.Position())) {
							distanceToNearestBox = glm::distance(m.A.Position(), m.B.Position());
							boxNumber_1 = j;
						}
					}
				}
				
				//switch the m.A & m.B cause only m.B is affected
				if (boxNumber_1 != boxNumber_2) {
					m.B = Borders[boxNumber_2];
					if (CheckIfObjectInBorderOfBox(m)) { 
						m.B = Borders[boxNumber_1];
						if (CheckIfObjectInBorderOfBox(m)) {

							glm::vec3 pointTopL(Borders[boxNumber_2].Position().x - m.A.Radius().x, m.A.Position().y, Borders[boxNumber_1].Position().z);
							glm::vec3 pointTopR(Borders[boxNumber_2].Position().x, m.A.Position().y, Borders[boxNumber_1].Position().z);
							glm::vec3 pointBotL(Borders[boxNumber_2].Position().x - m.A.Radius().x, m.A.Position().y, Borders[boxNumber_1].Position().z - m.A.Radius().z);
							glm::vec3 pointBotR(Borders[boxNumber_2].Position().x, m.A.Position().y, Borders[boxNumber_1].Position().z - m.A.Radius().z);



							m.A = Borders[boxNumber_2]; m.B = Players[i]; /*std::cout << "[Double border collid to bigger box] \n";*/ 
						}
						else {
							if (CheckCollision(m)) { m.A = Borders[boxNumber_1]; m.B = Players[i]; /*std::cout << "[In border collid to nearest box] \n";*/ }
							else { m.A = Borders[boxNumber_2]; m.B = Players[i]; /*std::cout << "[In border collid to bigger box] \n";*/ }
						}
					}
					else if (CheckCollision(m)) { m.A = Borders[boxNumber_2]; m.B = Players[i]; /*std::cout << "[Collid to bigger box] \n";*/ }
					else { m.A = Borders[boxNumber_1]; m.B = Players[i]; /*std::cout << "[Collid to nearest box] \n";*/ }
				}
				else { m.A = Borders[boxNumber_1]; m.B = Players[i]; /*std::cout << "[Collid to only box] \n";*/ }

				


				//determine the wall collision
				if (m.B.Position().x + (m.B.Radius().x*0.5f) > m.A.Top().x) {
					m.B.setPosition(glm::vec3(m.A.Top().x - (m.B.Radius().x*0.5f), m.B.Position().y, m.B.Position().z));
					//m.B.setVelocity(glm::vec3(-(m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().x - (m.B.Radius().x*0.5f) < m.A.Bottom().x) {
					m.B.setPosition(glm::vec3(m.A.Bottom().x + (m.B.Radius().x*0.5f), m.B.Position().y, m.B.Position().z));
					//m.B.setVelocity(glm::vec3(-(m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B.Position().y + (m.B.Radius().y*0.5f) > m.A.Top().y) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.A.Top().y - (m.B.Radius().y*0.5f), m.B.Position().z));
					//m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), -(m.B.Velocity().y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().y - (m.B.Radius().y*0.5f) < m.A.Bottom().y) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.A.Bottom().y + (m.B.Radius().y*0.5f), m.B.Position().z));
					//m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), -(m.B.Velocity().y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B.Position().z + (m.B.Radius().z*0.5f) > m.A.Top().z) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.B.Position().y, m.A.Top().z - (m.B.Radius().z*0.5f)));
					//m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), -(m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().z - (m.B.Radius().z*0.5f) < m.A.Bottom().z) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.B.Position().y, m.A.Bottom().z + (m.B.Radius().z*0.5f)));
					//m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), -(m.B.Velocity().z*speedToWallDegradation.z)));
				}
				////If players are nearing the walls
				//if (m.B.Position().x > (m.A.Top().x*0.95)) { FleeFromDirection(m, 1.0f, "x"); }
				//else if (m.B.Position().x < (m.A.Bottom().x*0.95)) { FleeFromDirection(m, 1.0f, "-x"); }
				//if (m.B.Position().z > (m.A.Top().z*0.95)) { FleeFromDirection(m, 1.0f, "z"); }
				//else if (m.B.Position().z < (m.A.Bottom().z*0.95)) { FleeFromDirection(m, 1.0f, "-z"); }

				Players[i] = m.B;
			}
		}
		 //Players to Specials
		for (int i = 0; i < NumberOfPlayers; i++) {
			m.A = Players[i];
			for (int j = 0; j < NumberOfSpecials; j++) {
				m.B = Specials[j];
				if (checkRadialCollision(m)) {
					if (m.B.SpecialAttribute() == 0) {}
					//Seeker Swarm
					else if (m.B.SpecialAttribute() == 1) {
						powerup[0].Play();
						m.B.Viewable = false;
						AbilityAffected[i][1] = true;
						AbilityCounter[i][1] = 0.0f;
					}
					//Toss-Up
					else if (m.B.SpecialAttribute() == 2) {
						powerup[1].Play();
						m.B.Viewable = false;
						for (int ij = 0; ij < NumberOfEnemies; ij++) {
							float ranPosY = static_cast<float>(rand() % 1000 + 100); //100 to 1100
							Enemies[ij].setForceOnObject(Enemies[ij].ForceOnObject() + glm::vec3(0.0f, ranPosY, 0.0f));
						}
					}
					//Health Up
					else if (m.B.SpecialAttribute() == 3) {
						powerup[2].Play();
						m.B.Viewable = false;
						Health[i] += 10; 
					}
					//Boost
					else if (m.B.SpecialAttribute() == 4) {
						powerup[3].Play();
						m.B.Viewable = false;
						AbilityAffected[i][4] = true;
						AbilityCounter[i][4] = 0.0f;
					}
					//Flee
					else if (m.B.SpecialAttribute() == 5) {
						powerup[4].Play();
						m.B.Viewable = false;
						AbilityAffected[i][5] = true;
						AbilityCounter[i][5] = 0.0f;
					}
					//Short Circuit
					else if (m.B.SpecialAttribute() == 6) {
						powerup[5].Play();
						m.B.Viewable = false;
						AbilityAffected[i][6] = true;
						AbilityCounter[i][6] = 0.0f;
					}
					//Super Shockwave
					else if (m.B.SpecialAttribute() == 7) {
						powerup[6].Play();
						m.B.Viewable = false;
						AbilityAffected[i][7] = true;
						AbilityCounter[i][7] = 0.0f;
					}
					//Invincibility
					else if (m.B.SpecialAttribute() == 8) {
						powerup[7].Play();
						m.B.Viewable = false;
						AbilityAffected[i][8] = true;
						AbilityCounter[i][8] = 0.0f;
					}
					//Flipped
					else if (m.B.SpecialAttribute() == 9) {
						powerup[8].Play();
						m.B.Viewable = false;
						AbilityAffected[i][9] = true;
						AbilityCounter[i][9] = 0.0f;
					}
				}
				if (checkRadialCollision(m)) { ResolveCollision(m, 0.01f); }
				Specials[j] = m.B;
			}//end for
			Players[i] = m.A;
		}//end for
		 //Players to Objects
		for (int i = 0; i < NumberOfPlayers; i++) {
			m.B = Players[i];
			bool onAObjectTemp = false;
			for (int j = 0; j < NumberOfObjects; j++) {
				if (Objects[j].Viewable) {
					m.A = Objects[j];
					//avoid objects
					applyRadialAvoidingSystem(m, 0.00f, 01.0f);
					//collid with objects
					if (checkRadialCollision(m)) { ResolveCollision(m, 0.01f); }
					//stand ontop of objects
					if (CheckIfOnObject(m, 0.0f, false)) {
						if (m.B.onObjectNum != j) { std::cout << "[P:" << i << "]on[O:" << j << "]" << std::endl; }
						onAObjectTemp = true; m.B.onObjectNum = j;
					}

					/*
					if (CheckCollision(m)) {
						bool positiveXYZ[3] { false, false, false };
						bool negitiveXYZ[3] { false, false, false };

						if ((m.B.Position().x - (m.B.Radius().x*0.50f) < m.A.Top().x) && (m.B.Position().x - (m.B.Radius().x*0.50f) > m.A.Bottom().x)) {
							positiveXYZ[0] = true; negitiveXYZ[0] = false;
						}
						else if ((m.B.Position().x + (m.B.Radius().x*0.50f) > m.A.Bottom().x) && (m.B.Position().x + (m.B.Radius().x*0.50f) < m.A.Top().x)) {
							negitiveXYZ[0] = true; positiveXYZ[0] = false;
						}
						else { positiveXYZ[0] = false; negitiveXYZ[0] = false; }

						//if (m.B.Position().y + (m.B.Radius().y*0.5f) > m.A.Top().y) {
							//	m.B.setPosition(glm::vec3(m.B.Position().x, m.A.Top().y - (m.B.Radius().y*0.5f), m.B.Position().z));
							//	m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), -(m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
							//}
							//else if (m.B.Position().y - (m.B.Radius().y*0.5f) < m.A.Bottom().y) {
							//	m.B.setPosition(glm::vec3(m.B.Position().x, m.A.Bottom().y + (m.B.Radius().y*0.5f), m.B.Position().z));
							//	m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), -(m.B.Velocity().y), (m.B.Velocity().z*speedToWallDegradation.z)));
							//}
						if ((m.B.Position().z - (m.B.Radius().z*0.50f) < m.A.Top().z) && (m.B.Position().z - (m.B.Radius().z*0.50f) > m.A.Bottom().z)) {
							positiveXYZ[2] = true; negitiveXYZ[2] = false;
						}
						else if ((m.B.Position().z + (m.B.Radius().z*0.50f) > m.A.Bottom().z) && (m.B.Position().z + (m.B.Radius().z*0.50f) < m.A.Top().z)) {
							negitiveXYZ[2] = true; positiveXYZ[2] = false;
						}
						else { positiveXYZ[2] = false; negitiveXYZ[2] = false; }

						// check two sides at once too see if in a corner
						//if in a corner; find distance to nearest output, than do collion to that one
						for (int ij = 0; ij < 3; ij++) {
							for (int ji = 0; ji < 3; ji++) {
								if (positiveXYZ[ij] && negitiveXYZ[ji]) {
									//find distance to nearest output, than do collion to that one
								}
								if (ij != ji && positiveXYZ[ij] && positiveXYZ[ji]) {
								}
								if (ij != ji && negitiveXYZ[ij] && negitiveXYZ[ji]) {
								}
							}
						}

						// check one side
						if (positiveXYZ[0]) {
							m.B.setPosition(glm::vec3(m.A.Top().x + (m.B.Radius().x*0.50f), m.B.Position().y, m.B.Position().z));
							m.B.setVelocity(glm::vec3(-(m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
						}
						else if (negitiveXYZ[0]) {
							m.B.setPosition(glm::vec3(m.A.Bottom().x - (m.B.Radius().x*0.50f), m.B.Position().y, m.B.Position().z));
							m.B.setVelocity(glm::vec3(-(m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
						}

						if (positiveXYZ[2]) {
							m.B.setPosition(glm::vec3(m.B.Position().x, m.B.Position().y, m.A.Top().z + (m.B.Radius().z*0.50f)));
							m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), -(m.B.Velocity().z*speedToWallDegradation.z)));
						}
						else if (negitiveXYZ[2]) {
							m.B.setPosition(glm::vec3(m.B.Position().x, m.B.Position().y, m.A.Bottom().z - (m.B.Radius().z*0.50f)));
							m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), -(m.B.Velocity().z*speedToWallDegradation.z)));
						}
					}
					*/

				Objects[j] = m.A;
				}
			}//end for

			if (onAObjectTemp && !m.B.IsJumping) { m.B.onObject = true; m.B.inAir = false; }
			else { m.B.onObject = false; m.B.inAir = true; }
			Players[i] = m.B;
		}//end for
		 //Players to Enemies
		for (int i = 0; i < NumberOfPlayers; i++) {
			m.A = Players[i];
			for (int j = 0; j < NumberOfEnemies; j++) {
				m.B = Enemies[j];
				if (CheckCollision(m))
				{	FxChannel[0] = Fx[0].Play(); 
					ResolveCollision(m, 1.0f); 
					applySeekSystem(m, -0.50f); }
				Enemies[j] = m.B;
			}//end for
			//Players[i] = m.A;
		}//end for

		 //Enemies to Rifts
		for (int i = 0; i < NumberOfEnemies; i++) {
			m.B = Enemies[i];
			for (int j = 0; j < NumberOfPlayers; j++) {
				m.A = Rifts[PlayerTeam[j]];
				if (ObjectsWithinRange(m, 15.0f)) { applySeekSystem(m, 2.0f); }
				if (CheckCollision(m)) 
				{	FxChannel[2] = Fx[2].Play();
					setEnemySpawn(m, i);
					if (!AbilityAffected[j][8]) { Health[PlayerTeam[j]] -= 1; }
				}//end if
			}//end for
			Enemies[i] = m.B;
		}//end for
		 //Enemies to Objects
		for (int i = 0; i < NumberOfEnemies; i++) {
			m.B = Enemies[i];
			bool onAObjectTemp = false;
			for (int j = 0; j < NumberOfObjects; j++) {
				m.A = Objects[j];
				//if (CheckCollision(m)) { ResolveCollision(m, 1.0f); }
				if (checkRadialCollision(m)) 
				{	FxChannel[0] = Fx[0].Play();
					ResolveCollision(m, 01.0f); }
				if (CheckIfOnObject(m, 0.0f, true)) { onAObjectTemp = true; }
				Objects[j] = m.A;
			}//end for
			if (onAObjectTemp) { m.B.onObject = true; m.B.inAir = false; }
			else { m.B.onObject = false; m.B.inAir = true; }
			Enemies[i] = m.B;
		}//end for
		 //Enemies to Specials
		for (int i = 0; i < NumberOfEnemies; i++) {
			m.A = Enemies[i];
			for (int j = 0; j < NumberOfSpecials; j++) {
				m.B = Specials[j];
				if (checkRadialCollision(m)) 
				{	FxChannel[0] = Fx[0].Play();
					ResolveCollision(m, 2.0f, 0.01f); }
				Specials[j] = m.B;
			}//end for
			Enemies[i] = m.A;
		}//end for
		 //Enemies to Enemies
		for (int i = 0; i < NumberOfEnemies; i++) {
			m.A = Enemies[i];
			for (int j = 0; j < NumberOfEnemies; j++) {
				if (i != j) {
					m.B = Enemies[j];
					applyRadialAvoidingSystem(m, 01.0f, 0.20f);
					if (checkRadialCollision(m)) 
					{	FxChannel[0] = Fx[0].Play();
						ResolveCollision(m, 0.50f); }
					Enemies[j] = m.B;
				}//end if
				else {}//end else
			}//end for
			Enemies[i] = m.A;
		}//end for
		 //Enemies to Borders
		for (int i = 0; i < NumberOfEnemies; i++) {
			m.A = Enemies[i];
			bool InBorder = false;

			//check to see if player is in a bounding box
			for (int j = 0; j < NumberOfBorders; j++) {
				m.B = Borders[j];
				if (ObjectInBox(m)) { InBorder = true; }
			}

			if (InBorder) {
				m.A = Borders[0]; m.B = Enemies[i];
				//If players are nearing the walls
				if (m.B.Position().x >(m.A.Top().x*0.90)) { FleeFromDirection(m, 2.0f, "x"); }
				else if (m.B.Position().x < (m.A.Bottom().x*0.90)) { FleeFromDirection(m, 2.0f, "-x"); }
				if (m.B.Position().z >(m.A.Top().z*0.90)) { FleeFromDirection(m, 2.0f, "z"); }
				else if (m.B.Position().z < (m.A.Bottom().z*0.90)) { FleeFromDirection(m, 2.0f, "-z"); }
				Enemies[i] = m.B;
			}
			//if the player is not in any box, seek to nearest one
			else {
				float distanceToNearestBox; int boxNumber_1 = 0; glm::vec3 boxWithLargerRadius(0.0f); int boxNumber_2 = 0;
				distanceToNearestBox = glm::distance(m.A.Position(), Borders[0].Position());
				//find nearest bounding box
				for (int j = 0; j < NumberOfBorders; j++) {
					m.B = Borders[j];
					glm::vec3 distNearPos = glm::vec3(glm::distance(m.A.Position().x, m.B.Position().x), glm::distance(m.A.Position().y, m.B.Position().y), glm::distance(m.A.Position().z, m.B.Position().z));
					if ((distNearPos.x < m.B.Radius().x) && (distNearPos.y < m.B.Radius().y) && (distNearPos.z < m.B.Radius().z)) {
						//box with larger radius
						if (boxWithLargerRadius.x < m.B.Radius().x && boxWithLargerRadius.y < m.B.Radius().y && boxWithLargerRadius.z < m.B.Radius().z) {
							boxWithLargerRadius = m.B.Radius();
							boxNumber_2 = j;
						}
						//closest box
						if (distanceToNearestBox > glm::distance(m.A.Position(), m.B.Position())) {
							distanceToNearestBox = glm::distance(m.A.Position(), m.B.Position());
							boxNumber_1 = j;
						}
					}
				}
				//switch the m.A & m.B cause only m.B is affected
				m.B = Borders[boxNumber_2];
				if (CheckCollision(m)) { m.A = Borders[boxNumber_2]; m.B = Enemies[i]; }
				else { m.A = Borders[boxNumber_1]; m.B = Enemies[i]; }


				//determine the wall collision
				if (m.B.Position().x + (m.B.Radius().x*0.5f) > m.A.Top().x) {
					m.B.setPosition(glm::vec3(m.A.Top().x - (m.B.Radius().x*0.5f), m.B.Position().y, m.B.Position().z));
					m.B.setVelocity(glm::vec3(-(m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().x - (m.B.Radius().x*0.5f) < m.A.Bottom().x) {
					m.B.setPosition(glm::vec3(m.A.Bottom().x + (m.B.Radius().x*0.5f), m.B.Position().y, m.B.Position().z));
					m.B.setVelocity(glm::vec3(-(m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B.Position().y + (m.B.Radius().y*0.5f) > m.A.Top().y) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.A.Top().y - (m.B.Radius().y*0.5f), m.B.Position().z));
					m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), -(m.B.Velocity().y*speedToWallDegradation.y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().y - (m.B.Radius().y*0.5f) < m.A.Bottom().y) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.A.Bottom().y + (m.B.Radius().y*0.5f), m.B.Position().z));
					m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), -(m.B.Velocity().y), (m.B.Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B.Position().z + (m.B.Radius().z*0.5f) > m.A.Top().z) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.B.Position().y, m.A.Top().z - (m.B.Radius().z*0.5f)));
					m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), -(m.B.Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B.Position().z - (m.B.Radius().z*0.5f) < m.A.Bottom().z) {
					m.B.setPosition(glm::vec3(m.B.Position().x, m.B.Position().y, m.A.Bottom().z + (m.B.Radius().z*0.5f)));
					m.B.setVelocity(glm::vec3((m.B.Velocity().x*speedToWallDegradation.x), (m.B.Velocity().y*speedToWallDegradation.y), -(m.B.Velocity().z*speedToWallDegradation.z)));
				}


				//If players are nearing the walls
				if (m.B.Position().x >(m.A.Top().x*0.90)) { FleeFromDirection(m, 2.0f, "x"); }
				else if (m.B.Position().x < (m.A.Bottom().x*0.90)) { FleeFromDirection(m, 2.0f, "-x"); }
				if (m.B.Position().z >(m.A.Top().z*0.90)) { FleeFromDirection(m, 2.0f, "z"); }
				else if (m.B.Position().z < (m.A.Bottom().z*0.90)) { FleeFromDirection(m, 2.0f, "-z"); }


				Enemies[i] = m.B;
			}
		}
	}

	//Applying Gravity to everything
	if (ApplyingGravity) {
		m.A = Objects[0];
		//apply gravity relative to object[0]
		for (int i = 0; i < NumberOfPlayers; i++) { m.B = Players[i]; applyGravitationalForces(m, -01.0f); Players[i] = m.B; }
		for (int i = 0; i < NumberOfEnemies; i++) { m.B = Enemies[i]; applyGravitationalForces(m, -5.0f); Enemies[i] = m.B; }
		for (int i = 0; i < NumberOfSpecials; i++) { m.B = Specials[i]; applyGravitationalForces(m, -3.0f); Specials[i] = m.B; }
	}

	
	bool enableAbilitys = true;
	//Applys all over-time abilitys
	if (enableAbilitys) {


		for (int ijp = 0; ijp < NumberOfPlayers; ijp++) {

			//Enemies seek towers [1]
			if (AbilityAffected[ijp][1] && AbilityCounter[ijp][1] < AbilityLength[ijp][1]) {
				AbilityCounter[ijp][1] += deltaTasSeconds;
				if (PlayerTeam[ijp] == 0) { m.A = Rifts[1]; }
				else if (PlayerTeam[ijp] == 1) { m.A = Rifts[0]; }
				for (int j = 0; j < NumberOfEnemies; j++) {
					m.B = Enemies[j];
					applySeekSystem(m, 3.0f);
					Enemies[j] = m.B;
				}
			}
			else { AbilityAffected[ijp][1] = false; }

			//Boost [4]
			if (AbilityAffected[ijp][4] && AbilityCounter[ijp][4] < AbilityLength[ijp][4]) {
				AbilityCounter[ijp][4] += deltaTasSeconds;
				Players[ijp].SprintSpeed = SprintSpeed;
			}
			else {
				Players[ijp].SprintSpeed = 1.0f;
				AbilityAffected[ijp][4] = false;
			}

			//Flee [5]
			if (AbilityAffected[ijp][5] && AbilityCounter[ijp][5] < AbilityLength[ijp][5]) {
				AbilityCounter[ijp][5] += deltaTasSeconds;
				m.A = Players[ijp];
				for (int i = 0; i < NumberOfEnemies; i++) {
					m.B = Enemies[i];
					applyRadialFleeingSystem(m, 20.0f, 10.0f);
					Enemies[i] = m.B;
				}
			}
			else { AbilityAffected[ijp][5] = false; }

			//Short Circuit [6]
			if (AbilityAffected[ijp][6] && AbilityCounter[ijp][6] < AbilityLength[ijp][6]) {
				AbilityCounter[ijp][6] += deltaTasSeconds;
				if (PlayerTeam[ijp] == 0) {
					for (int ijp2 = 0; ijp2 < NumberOfPlayers; ijp2++) {
						if (PlayerTeam[ijp2] == 1) {
							Players[ijp2].inShock = true;
						}
					}
				}
				else if (PlayerTeam[ijp] == 1) {
					for (int ijp2 = 0; ijp2 < NumberOfPlayers; ijp2++) {
						if (PlayerTeam[ijp2] == 0) {
							Players[ijp2].inShock = true;
						}
					}
				}
			}
			else { AbilityAffected[ijp][6] = false; }

			//Super Shockwave [7] //not done yet
			if (AbilityAffected[ijp][7] && AbilityCounter[ijp][7] < AbilityLength[ijp][7]) {
				AbilityCounter[ijp][7] += deltaTasSeconds;
			}
			else { AbilityAffected[ijp][7] = false; }

			//Invincibility [8] 
			if (AbilityAffected[ijp][8] && AbilityCounter[ijp][8] < AbilityLength[ijp][8]) {
				AbilityCounter[ijp][8] += deltaTasSeconds;
			}
			else { AbilityAffected[ijp][8] = false; }

			//Flipped [9]
			if (AbilityAffected[ijp][9] && AbilityCounter[ijp][9] < AbilityLength[ijp][9]) {
				AbilityCounter[ijp][9] += deltaTasSeconds;
				if (PlayerTeam[ijp] == 0) {
					for (int ijp2 = 0; ijp2 < NumberOfPlayers; ijp2++) {
						if (PlayerTeam[ijp2] == 1) {
							FlipedControllers[ijp2] = true;
						}
					}
				}
				else if (PlayerTeam[ijp] == 1) {
					for (int ijp2 = 0; ijp2 < NumberOfPlayers; ijp2++) {
						if (PlayerTeam[ijp2] == 0) {
							FlipedControllers[ijp2] = true;
						}
					}
				}
			}
			else { AbilityAffected[ijp][9] = false; }

		}//end for

		/////////////////////////////////

		for (int ijp = 0; ijp < NumberOfPlayers; ijp++) {

			//Short Circuit [6]
			if (Players[ijp].inShock) {
				AffectsOnPlayer[ijp][1].setPosition(Players[ijp].Position());
				AffectsOnPlayer[ijp][2].setPosition(Players[ijp].Position());

				AffectsOnPlayer[ijp][1].Viewable = true;
				AffectsOnPlayer[ijp][2].Viewable = true;

				AffectsOnPlayer[ijp][1].setRotation(glm::vec3(0.0f, AffectsOnPlayer[ijp][1].Angle().y - (deltaTasSeconds*316.0f), 0.0f));
				AffectsOnPlayer[ijp][2].setRotation(glm::vec3(0.0f, AffectsOnPlayer[ijp][2].Angle().y + (deltaTasSeconds*147.0f), 0.0f));
			}
			else if (!Players[ijp].inShock) {
				AffectsOnPlayer[ijp][1].setPosition(Players[ijp].Position());
				AffectsOnPlayer[ijp][1].Viewable = false;
				AffectsOnPlayer[ijp][2].setPosition(Players[ijp].Position());
				AffectsOnPlayer[ijp][2].Viewable = false;
			}

			//Flipped [9]
			if (FlipedControllers[ijp]) {
				AbilityCounter[ijp][9] += deltaTasSeconds;
				if (AbilityCounter[ijp][9] > AbilityLength[ijp][9]) {
					AbilityCounter[ijp][9] = 0.0f;
					FlipedControllers[ijp] = false;
				}
			}

		}//end for
	}//end enableAbilitys


	//Can the users use the shock wave ability
	for (int i = 0; i < NumberOfPlayers; i++) {
			if (PShockWave[i]) {
				AffectsOnPlayer[i][0].Viewable = true;
				if (PShockWaveCounter[i] > 0.0f) { PShockWaveCounter[i] -= deltaTasSeconds; }
				else { PShockWave[i] = false; PShockWaveChargeUp[i] = 0.0f; }

				float ForceWeight;
				float ForceWeight_Addition = 70.0f + ((2.4f * (0.80f + PShockWaveChargeUp[i]))*(2.4f * (0.80f + PShockWaveChargeUp[i]))*(2.4f * (0.80f + PShockWaveChargeUp[i])));
				float ForceWeight_Minius = ((1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i])));

				if (1000.0f <= ForceWeight_Addition - (ForceWeight_Addition / 2)) { ForceWeight = 1000.0f; }
				else if (ForceWeight_Minius < (ForceWeight_Addition / 2)) { ForceWeight = ForceWeight_Addition - ForceWeight_Minius; }

				float ForceModifier = -(40.0f + 1.5f*ForceWeight);

				glm::vec3 sizeofShockWave;
				if (speedControlSW) { sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f)), 100.0f, PShockWaveCounter[i]); }
				else { sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f)), PShockWaveCounter[i]); }

				AffectsOnPlayer[i][0].setScale(glm::vec3(sizeofShockWave.x, 01.0f + (0.50f*PShockWaveChargeUp[i]), sizeofShockWave.z));
				AffectsOnPlayer[i][0].setRotation(glm::vec3(0.0f, PShockWaveChargeUp[i]*(180.0f / 3.14159f), 0.0f));
				AffectsOnPlayer[i][0].setSizeOfHitBox(sizeofShockWave);

				m.A = AffectsOnPlayer[i][0];
				//collision to Enemies
				for (int j = 0; j < NumberOfEnemies; j++) {
					m.B = Enemies[j];
					if (checkRadialCollision(m))
					{	FxChannel[0] = Fx[0].Play();
						applySeekSystem(m, ForceModifier);
						ResolveCollision(m, 1.0f); 
						m.B.inShock = true; }
					Enemies[j] = m.B;
				}//end for
				//collision to Specials
				for (int j = 0; j < NumberOfSpecials; j++) {
					m.B = Specials[j];
					if (checkRadialCollision(m)) { applySeekSystem(m, ForceModifier); ResolveCollision(m, 1.0f); m.B.inShock = true; }
					Specials[j] = m.B;
				}//end for
			}//end if
			else if (PShockWave[i] == false && PShockWaveChargeUp[i] > 0.250f) {
				AffectsOnPlayer[i][0].Viewable = true;
				float ForceWeight;
				float ForceWeight_Addition = 70.0f + ((2.4f * (0.80f + PShockWaveChargeUp[i]))*(2.4f * (0.80f + PShockWaveChargeUp[i]))*(2.4f * (0.80f + PShockWaveChargeUp[i])));
				float ForceWeight_Minius = ((1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i])));
				
				if (1000.0f <= ForceWeight_Addition - (ForceWeight_Addition / 2)) { ForceWeight = 1000.0f; }
				else if (ForceWeight_Minius < (ForceWeight_Addition / 2)) { ForceWeight = ForceWeight_Addition - ForceWeight_Minius; }

				glm::vec3 sizeofShockWave;
				if (speedControlSW) { sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f)), 100.0f, 0.25f); }
				else { sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f)), 0.25f); }
				
				sizeofShockWave.y = 01.0f;
				AffectsOnPlayer[i][0].setScale(glm::vec3(sizeofShockWave.x, 01.0f + (0.50f*PShockWaveChargeUp[i]), sizeofShockWave.z));
				AffectsOnPlayer[i][0].setRotation(glm::vec3(0.0f, PShockWaveChargeUp[i] * (180.0f / 3.14159f), 0.0f));
			}
			else { AffectsOnPlayer[i][0].Viewable = false; }
		}//end for
	

	//Idle Enemies will despawn then respawn
	if (IdleEnemiesRespawn) {
		m.A = Objects[0];
		for (int j = 0; j < NumberOfEnemies; j++)
		{
			m.B = Enemies[j];
			float ranPosj = static_cast<float>(rand() % 80 - 40);
			//object is [NOT idle]
			if (!m.B.AreIdle) {
				if (!m.B.inAir &&
					(m.B.Velocity().x >= -0.001f && m.B.Velocity().x <= 0.001f) &&
					(m.B.Velocity().z >= -0.001f && m.B.Velocity().z <= 0.001f))
				{
					m.B.AreIdle = true;
					m.B.IdleTimeCounter = 0.0f;
				}
				else {
					m.B.AreIdle = false;
					m.B.IdleTimeCounter = 0.0f;
				}
			}
			//object [IS idle]
			else if (m.B.AreIdle) {
				if (!m.B.inAir &&
					(m.B.Velocity().x >= -0.001f && m.B.Velocity().x <= 0.001f) &&
					(m.B.Velocity().z >= -0.001f && m.B.Velocity().z <= 0.001f))
				{
					//object [IS idle] for less then 20 seconds
					if (m.B.IdleTimeCounter < 20.0f) {
						m.B.IdleTimeCounter += deltaTasSeconds;
					}
					//object [IS idle] for more then 20 seconds
					else {
						setEnemySpawn(m, j);
						m.B.IdleTimeCounter = 0.0f;
						m.B.AreIdle = false;
						m.B.inAir = true;
					}
				}
				else {}
			}
			else {}
			Enemies[j] = m.B;
		}
	}

	if (randomSpecialTime <= 0.0f)
	{

		float ranPosZ = (rand() % 80 - 40) + ((rand() % 100 - 50)*0.01f); //-39.50 to 40.50
		float ranPosY = (rand() % 20 + 10) + ((rand() % 100 - 50)*0.01f); //  9.50 to 30.50
		float ranPosX = (rand() % 80 - 40) + ((rand() % 100 - 50)*0.01f); //-39.50 to 40.50
		int ranSpec = (rand() % 9 + 1); //1 to 9

		Specials[ranSpec].setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		Specials[ranSpec].setPosition(glm::vec3(ranPosX, ranPosY, ranPosZ));
		Specials[ranSpec].Viewable = true;

		randomSpecialTime = (rand() % 60 + 30) + ((rand() % 100 - 50)*0.01f); //-29.50 to 90.50
	}
	else { randomSpecialTime -= deltaTasSeconds; }


	
	//Updating Targets
	for (int i = 0; i < NumberOfPlayers; i++) { if (Players[i].Viewable) { Players[i].updateP(deltaTasSeconds); } }
	for (int i = 0; i < NumberOfEnemies; i++) { if (Enemies[i].Viewable) { Enemies[i].update(deltaTasSeconds); } }
	
	for (int i = 0; i < NumberOfSpecials; i++){ if (Specials[i].Viewable) {
		 Specials[i].setRotation(glm::vec3(0.0f, abilityRotation[i],0.0f));
		 Specials[i].update(deltaTasSeconds); 
		 abilityRotation[i] += (deltaTasSeconds*73.0f); } }

}



//////////////////////////////////////////////////////////////////////



/* function DisplayCallbackFunction(void)
* Description:
*  - this is the openGL display routine
*  - this draws the sprites appropriately
*/
void DisplayCallbackFunction(void)
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the screen
	if (inMenu) { InMenuDraw(2); }
	else if (inOptions) { InOptionDraw(inOptionsTab); }
	else if (inGame) { for (int i = 0; i < 2; i++) { InGameDraw(i); } }
	else {}

	glutSwapBuffers();
}

/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is pressed
*/
void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	keyDown[key] = true;
	if (inMenu) {
		switch (key)
		{
		case 27: // the escape key
			exitProgram();
			break;
		case ' ':
			setBoardStart();
			inMenu = false; inGame = true;
			break;
		case '+':
		case '=':
			TestFloat += TestFloatIncrementAmount;
			std::cout << "[[" << TestFloat << "]]" << std::endl;
			ButtonForSliders[0].setRotation(glm::vec3(0.0f, (TestFloat + 0.0f), 0.0f));
			break;
		case '_':
		case '-':
			TestFloat -= TestFloatIncrementAmount;
			std::cout << "[[" << TestFloat << "]]" << std::endl;
			ButtonForSliders[0].setRotation(glm::vec3(0.0f, (TestFloat + 0.0f), 0.0f));
			break;
		case 'o':
		case 'O': {
			//bool inMenu = true; bool inGame = false; bool inOptions = false; 
			if (inOptions == false) {
				if (inMenu == true) { inOptions = true; inMenu = false; lastMenu == "inMenu"; }
				else if (inGame == true) { inOptions = true; inGame = false; lastMenu == "inGame"; }
				else if (inOptions == true) { inOptions = true; inOptions = false; lastMenu == "inOptions"; }
				else {}
			}
			else if (inOptions == true) {
				if (lastMenu == "inMenu") { inOptions = false; inMenu = true; }
				else if (lastMenu == "inGame") { inOptions = false; inGame = true; }
				else if (lastMenu == "inOptions") { inOptions = false; inOptions = true; }
				else {}
			}
			break; 
		}
		}
	}
	else if (inGame) {
		switch (key)
		{
		case 27:
			inGame = false; inMenu = true;
			break;
		case 'N':
		case 'n':
			/*
			FMOD_VECTOR positionTemp; positionTemp.x = 0.0f; positionTemp.y = 10.0f; positionTemp.z = 0.0f;
			drum[0].SetPosition(drumChannel, positionTemp);
			drumChannel = drum[0].Play();

			//OR

			drum[0].SetPosition(glm::vec3(110.0f));
			drum[0].Play();
			*/
			
			break;
		case 'R':
		case 'r':
			break;
		case '+':
		case '=':
			TestFloat += TestFloatIncrementAmount;
			std::cout << "[ [ " << TestFloat << " ] ]" << std::endl; 
			break;
		case '_':
		case '-':
			TestFloat -= TestFloatIncrementAmount;
			std::cout << "[ [ " << TestFloat << " ] ]" << std::endl; 
			break;
		case 'V':
		case 'v':
			if (cameraMode == 0) { cameraMode = 1; }
			else if (cameraMode == 1) { cameraMode = 0; }
			break;
		case '0':
			Specials[static_cast<int>(TestFloat)].Viewable = true; Specials[static_cast<int>(TestFloat)].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
			break;
		case '1':
			FxChannel[0] = Fx[0].Play();
			FxChannel[0]->setVolume(1.0);
			break;
		case '2': 
			FxChannel[1] = Fx[1].Play();
			FxChannel[1]->setVolume(0.1);
			break;
		case '3': 
			FxChannel[2] = Fx[2].Play();
			FxChannel[2]->setVolume(0.8);
			break;
		case '4': 
			break;
		}
	}
	else if (inOptions) {
		switch (key)
		{
		case 'o':
		case 'O': {
			if (inOptions == false) {
				if (inMenu == true) { inOptions = true; inMenu = false; lastMenu == "inMenu"; }
				else if (inGame == true) { inOptions = true; inGame = false; lastMenu == "inGame"; }
				else if (inOptions == true) { inOptions = true; inOptions = false; lastMenu == "inOptions"; }
				else {}
			}
			else if (inOptions == true) {
				if (lastMenu == "inMenu") { inOptions = false; inMenu = true; }
				else if (lastMenu == "inGame") { inOptions = false; inGame = true; }
				else if (lastMenu == "inOptions") { inOptions = false; inOptions = true; }
				else {}
			}
			break;
		}
		}
	}
	else {}
}

/* function void KeyboardUpCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is lifted
*/
void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
	keyDown[key] = false;
}

//empty
void idleFunc()
{
	
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - no drawing, just changing the state
*  - changes the frame number and calls for a redisplay
*  - FRAME_DELAY is the number of milliseconds to wait before calling the timer again
*/
void TimerCallbackFunction(int value)
{
	// Calculate new deltaT for potential updates and physics calculations
	static unsigned int elapsedTimeAtLastTick = 0;
	unsigned int totalElapsedTime = glutGet(GLUT_ELAPSED_TIME);
	unsigned int deltaT = totalElapsedTime - elapsedTimeAtLastTick;
	elapsedTimeAtLastTick = totalElapsedTime;
	float deltaTasSeconds = (deltaT / 1000.0f);
	deltaTasSecs = deltaTasSeconds;



	if (gamepad.CheckConnection()) {
		for (int i = 0; i < NumberOfPlayers; i++) {
			gamepad.SetActivePort(i);

			if (gamepad.GetActivePort() == i) {
				gamepad.Refresh();
				ControllerDelayButton(i, deltaTasSeconds);
			}
		}
	}
	else { KeyBoardDelayButton(deltaTasSeconds); }
	

	if (inMenu) { MenuScreen(deltaTasSeconds); }
	else if (inGame) { GameScreen(deltaTasSeconds); }
	else if (inOptions) { OptionScreen(deltaTasSeconds); }
	else {}
	//this call makes it actually show up on screen
	glutPostRedisplay();
	//this call gives it a proper frame delay to hit our target FPS
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*  - currently set up for an orthographic view (2D only)
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	for (int i = 20; i < 120; i++) {
		for (int j = 1; j <= 9; j++) {
			if ((w + j) == (i * 16)) { windowWidth = i * 16; windowHeight = i * 9; }
			else if ((w - j) == (i * 16)) { windowWidth = i * 16; windowHeight = i * 9; }
		}
		for (int j = 1; j <= 5; j++) {
			if ((h + j) == (i * 9)) { windowWidth = i * 16; windowHeight = i * 9; }
			else if ((h - j) == (i * 9)) { windowWidth = i * 16; windowHeight = i * 9; }
		}
	}
	glutReshapeWindow(windowWidth, windowHeight);
	screenpositionX = glutGet((GLenum)GLUT_WINDOW_X); 
	screenpositionY = glutGet((GLenum)GLUT_WINDOW_Y);
}

/* function MouseClickCallbackFunction()
* Description:
*   - this is called when the mouse is clicked
*/
void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	// Handle mouse clicks
	if (state == GLUT_DOWN)
	{
		mouseDown[button] = true;
		if (button == GLUT_LEFT_BUTTON) {
			MPosToOPosX = ((((float)mousepositionX / (float)windowWidth) * 56.0f) - 28.0f);
			MPosToOPosY = -((((float)(windowHeight - mousepositionY) / (float)windowHeight) * 56.0f) - 28.0f);
		}
		if (button == GLUT_RIGHT_BUTTON) {}
		if (button == GLUT_MIDDLE_BUTTON) {}
	}
}

/* function MouseMotionCallbackFunction()
* Description:
*   - this is called when the mouse is clicked and moves
*/
void MouseMotionCallbackFunction(int x, int y) //while a mouse button is pressed
{
	float changex = static_cast<float>(x - mousepositionX);
	float changey = static_cast<float>(y - mousepositionY);
	mousepositionX = x;
	mousepositionY = y;
	MPosToOPosX = ((((float)mousepositionX / (float)windowWidth) * 56) - 28);
	MPosToOPosY = -((((float)(windowHeight - mousepositionY) / (float)windowHeight) * 56) - 28);

	if (inGame) { }
	else if (inMenu) { }
	else if (inOptions) { }
}

/* function MousePassiveMotionCallbackFunction()
* Description:
*   - this is called when the mouse is moved in the window
*/
void MousePassiveMotionCallbackFunction(int x, int y) //while a mouse button isn't pressed
{
	mousepositionX = x;
	mousepositionY = y;

	if (inGame) { }
	else if (inMenu) { }
	else if (inOptions) {
		if (MenuSwitchCounter[0] > 0.0f) { MenuSwitchCounter[0] -= deltaTasSecs; }
		else {
			bool pressedA = false;
			for (unsigned int i = 0; i < NumberOfSliders; i++) {
				//move nob along the slider
				if (Slider[i].moveNob(MPosToOPosX, MPosToOPosY)) { ButtonForSliders[i].setPosition(glm::vec3(MPosToOPosX, 0.02f, Slider[i].SBar_Pos.z)); pressedA = true; }
			}
			if (pressedA) { MenuSwitchCounter[0] = 0.50f; }
		}
	}
}

//////////////////////////////////////////////////////////////////////


void initializeFrameBufferObjects()
{
	fbo.createFrameBuffer(windowWidth, windowHeight, 1, true);
}

void InitializeShaders()
{
	std::string shaderPath;

	//Initialize  Shader
	if (DoesFileExists("..//Assets//shaders//")) { shaderPath = "..//Assets//shaders//"; }
	else if (DoesFileExists("Assets//shaders//")) { shaderPath = "Assets//shaders//"; }
	else { std::cout << "[ERROR] Could not find [Shaders]" << std::endl; }

	//Load vertex shaders
	Shader v_default, v_passThrough, v_textShader;
	v_default.loadShaderFromFile(shaderPath + "default_v.glsl", GL_VERTEX_SHADER);
	v_passThrough.loadShaderFromFile(shaderPath + "passThru_v.glsl", GL_VERTEX_SHADER);
	v_textShader.loadShaderFromFile(shaderPath + "text_v.glsl", GL_VERTEX_SHADER);
	//Load fragment shaders
	Shader f_default, f_passThrough, f_textShader;
	f_default.loadShaderFromFile(shaderPath + "default_f.glsl", GL_FRAGMENT_SHADER);
	f_passThrough.loadShaderFromFile(shaderPath + "passThru_f.glsl", GL_FRAGMENT_SHADER);
	f_textShader.loadShaderFromFile(shaderPath + "text_f.glsl", GL_FRAGMENT_SHADER);


	//Default material that all objects use
	materials["default"] = std::make_shared<Material>();
	materials["default"]->shader->attachShader(v_default);
	materials["default"]->shader->attachShader(f_default);
	materials["default"]->shader->linkProgram();

	//passThrough material that all objects use
	materials["passThrough"] = std::make_shared<Material>();
	materials["passThrough"]->shader->attachShader(v_passThrough);
	materials["passThrough"]->shader->attachShader(f_passThrough);
	materials["passThrough"]->shader->linkProgram();

	//text material that all objects use
	materials["text"] = std::make_shared<Material>();
	materials["text"]->shader->attachShader(v_textShader);
	materials["text"]->shader->attachShader(f_textShader);
	materials["text"]->shader->linkProgram();


}

/* function InitializeTextPlane()
* Description:
*   - this is called when the file first loads
*   - loads all the menu pages into the game
*/
void InitializeTextPlane()
{
	auto defaultMaterial = materials["default"];
	auto passThroughMaterial = materials["passThrough"];



	std::string ImagePath;
	std::string ObjectPath;
	if (DoesFileExists("..//Assets//Img") && DoesFileExists("..//Assets//Obj")) {
		ImagePath = "..//Assets//Img//";
		ObjectPath = "..//Assets//Obj//";
	}
	else if (DoesFileExists("Assets//Img") && DoesFileExists("Assets//Obj")) {
		ImagePath = "Assets//Img//";
		ObjectPath = "Assets//Obj//";
	}
	else { std::cout << "[ERROR] Could not find [Img] and/or [Obj]" << std::endl; }


	planeForText[0].objectLoader(ObjectPath + "PlainForText.obj");
	planeForText[0].setMaterial(passThroughMaterial);
	planeForText[0].setMass(0.0f);
	planeForText[0].Viewable = true;
	planeForText[0].setScale(glm::vec3(39.0f*0.9f, 1.0f, 39.0f*1.6f));
	planeForText[0].setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	planeForText[0].setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	planeForText[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Main_Menu.png").c_str())));

	planeForText[1].objectLoader(&planeForText[0]);
	planeForText[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options.png").c_str())));

	planeForText[2].objectLoader(&planeForText[0]);
	planeForText[2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Controls.png").c_str())));

	planeForText[3].objectLoader(&planeForText[0]);
	planeForText[3].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Power_Ups.png").c_str())));

	planeForText[4].objectLoader(&planeForText[0]);
	planeForText[4].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options.png").c_str())));



	//Start Button
	ButtonObjects[0].objectLoader(ObjectPath + "PlainForText.obj");
	ButtonObjects[0].setMaterial(passThroughMaterial);
	ButtonObjects[0].setScale(glm::vec3(9.0f*0.9f, 1.0f, 7.0f*1.6f));
	ButtonObjects[0].setSizeOfHitBox(glm::vec3(9.0f*1.45f, 1.0f, 7.0f*1.58f));
	ButtonObjects[0].setPosition(glm::vec3(0.0f, 0.01f, -3.0f));
	ButtonObjects[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Start_Button.png").c_str())));
	for (unsigned int i = 1; i < NumberOfButtons; i++) { ButtonObjects[i].objectLoader(&ButtonObjects[0]); }
	//Options Button
	ButtonObjects[1].setPosition(glm::vec3(0.0f, 0.01f, 9.0f));
	ButtonObjects[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Button.png").c_str())));
	//Exit Button
	ButtonObjects[2].setPosition(glm::vec3(0.0f, 0.01f, 21.0f));
	ButtonObjects[2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Exit_Button.png").c_str())));
	//Back Button
	ButtonObjects[3].setPosition(glm::vec3(-21.0f, 0.01f, -21.0f));
	ButtonObjects[3].setScale(glm::vec3(2.0f*0.9f, 1.0f, 3.50f*0.9f));
	ButtonObjects[3].setSizeOfHitBox(glm::vec3(2.0f*1.5f, 1.0f, 3.50f*0.9f));
	ButtonObjects[3].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Back_Button.png").c_str())));
	//Controls Button
	ButtonObjects[4].setPosition(glm::vec3(0.0f, 0.01f, -3.0f));
	ButtonObjects[4].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Controls_Button.png").c_str())));
	//Power_Ups Button
	ButtonObjects[5].setPosition(glm::vec3(0.0f, 0.01f, 9.0f));
	ButtonObjects[5].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Power_Ups_Button.png").c_str())));
	//Config Button
	ButtonObjects[6].setPosition(glm::vec3(0.0f, 0.01f, 21.0f));
	ButtonObjects[6].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Config_Button.png").c_str())));
	//Button
	for (unsigned int i = 0; i < NumberOfButtons; i++) {
		Button[i].SBut_Top = ButtonObjects[i].Top(), Button[i].SBut_Bot = ButtonObjects[i].Bottom(), Button[i].SBut_Pos = ButtonObjects[i].Position(), Button[i].SBut_Rad = (ButtonObjects[i].Radius() / 2.0f);
	}


	//Slider bar
	planeForSliders[0].objectLoader(ObjectPath + "PlainForText.obj");
	planeForSliders[0].setMaterial(passThroughMaterial);
	planeForSliders[0].setScale(glm::vec3(5.0f, 1.0f, 3.50f));
	planeForSliders[0].setSizeOfHitBox(glm::vec3(5.0f*1.6f, 1.0f, 3.50f*0.9f));
	planeForSliders[0].setPosition(glm::vec3(20.0f, 0.01f, -10.0f));
	planeForSliders[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Slider_Bar.png").c_str())));
	//Slider nob
	ButtonForSliders[0].objectLoader(&planeForSliders[0]);
	ButtonForSliders[0].setScale(glm::vec3(1.0f, 1.0f, 3.50f));
	ButtonForSliders[0].setSizeOfHitBox(glm::vec3(1.0f, 1.0f, 3.50f*0.9f));
	ButtonForSliders[0].setPosition(glm::vec3(20.0f, 0.02f, -10.0f));
	ButtonForSliders[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Slider_Nob.png").c_str())));


	//Slider full
	Slider[0].SBar_Top = planeForSliders[0].Top(), Slider[0].SBar_Bot = planeForSliders[0].Bottom(), Slider[0].SBar_Pos = planeForSliders[0].Position(), Slider[0].SBar_Rad = (planeForSliders[0].Radius() / 2.0f);
	Slider[0].SNob_Top = ButtonForSliders[0].Top(), Slider[0].SNob_Bot = ButtonForSliders[0].Bottom(), Slider[0].SNob_Pos = ButtonForSliders[0].Position(), Slider[0].SNob_Rad = (ButtonForSliders[0].Radius() / 2.0f);
	Slider[0].SBar_Length = ((Slider[0].SBar_Top - Slider[0].SNob_Rad) - (Slider[0].SBar_Bot + Slider[0].SNob_Rad));
	Slider[0].SNob_Length = (Slider[0].SNob_Pos - (Slider[0].SBar_Bot + Slider[0].SNob_Rad));
	Slider[0].SNob_Precent = (Slider[0].SNob_Length / Slider[0].SBar_Length)*100.0f;

	for (unsigned int i = 1; i < NumberOfSliders; i++) {
		//Slider bar
		planeForSliders[i].objectLoader(&planeForSliders[i - 1]);
		planeForSliders[i].setPosition(planeForSliders[i - 1].Position() + glm::vec3(0.0f, 0.0f, planeForSliders[i - 1].Radius().z));
		//Slider nob
		ButtonForSliders[i].objectLoader(&ButtonForSliders[i - 1]);
		ButtonForSliders[i].setPosition(ButtonForSliders[i - 1].Position() + glm::vec3(0.0f, 0.0f, ButtonForSliders[i - 1].Radius().z));
		//Slider full
		Slider[i].SBar_Top = planeForSliders[i].Top(), Slider[i].SBar_Bot = planeForSliders[i].Bottom(), Slider[i].SBar_Pos = planeForSliders[i].Position(), Slider[i].SBar_Rad = (planeForSliders[i].Radius() / 2.0f);
		Slider[i].SNob_Top = ButtonForSliders[i].Top(), Slider[i].SNob_Bot = ButtonForSliders[i].Bottom(), Slider[i].SNob_Pos = ButtonForSliders[i].Position(), Slider[i].SNob_Rad = (ButtonForSliders[i].Radius() / 2.0f);
		Slider[i].SBar_Length = ((Slider[i].SBar_Top - Slider[i].SNob_Rad) - (Slider[i].SBar_Bot + Slider[i].SNob_Rad));
		Slider[i].SNob_Length = (Slider[i].SNob_Pos - (Slider[i].SBar_Bot + Slider[i].SNob_Rad));
		Slider[i].SNob_Precent = (Slider[i].SNob_Length / Slider[i].SBar_Length)*100.0f;
	}

	planeForText[0].Viewable = true;


}

/* function InitializeSounds()
* Description:
*   - this is called when the file first loads
*   - loads all the needed sounds into the game
*/
void InitializeSounds() {
	std::string SoundPath;
	if (DoesFileExists("..//Assets//Media")) { SoundPath = "..//Assets//Media//"; }
	else if (DoesFileExists("Assets//Media")) { SoundPath = "Assets//Media//"; }
	else { std::cout << "[ERROR] Could not find [Media]" << std::endl; }

	systemSound.Sys.Init();

	for (int i = 0; i < 2; i++) {
		Sound::Sys.listenerPos[i].x = 0.0f; Sound::Sys.listenerPos[i].y = 0.0f; Sound::Sys.listenerPos[i].z = 0.0f;
	}



	//Power up sound effects

	//Seeker Swarm = 0
	powerup[0].Load(_strdup((SoundPath + "Soundeffects//Voicerecording_seeker.wav").c_str()), TRUE, FALSE);
	//Toss-Up = 1
	powerup[1].Load(_strdup((SoundPath + "Soundeffects//Voicerecording_toss.wav").c_str()), TRUE, FALSE);
	//Health Up = 2
	powerup[2].Load(_strdup((SoundPath + "drumloop.wav").c_str()), TRUE, FALSE);
	//Boost = 3
	powerup[3].Load(_strdup((SoundPath + "Soundeffects//Voicerecording_boost.wav").c_str()), TRUE, FALSE);
	//Flee = 4
	powerup[4].Load(_strdup((SoundPath + "Soundeffects//Voicerecording_flee.wav").c_str()), TRUE, FALSE);
	//Short Circuit = 5
	powerup[5].Load(_strdup((SoundPath + "Soundeffects//Voicerecording_sc.wav").c_str()), TRUE, FALSE);
	//Super Shockwave = 6
	powerup[6].Load(_strdup((SoundPath + "Soundeffects//Voicerecording_ss.wav").c_str()), TRUE, FALSE);
	//Invincibility = 7
	powerup[7].Load(_strdup((SoundPath + "Soundeffects//Voicerecording_invincible.wav").c_str()), TRUE, FALSE);
	//Flipped = 8
	powerup[8].Load(_strdup((SoundPath + "Soundeffects//Voicerecording_flipped.wav").c_str()), TRUE, FALSE);


	//Fx sound effects
	Fx[0].Load(_strdup((SoundPath + "Soundeffects//bounce0.3.wav").c_str()), TRUE, FALSE);//Ball bouncing
	Fx[1].Load(_strdup((SoundPath + "Soundeffects//crowdsound0.1.wav").c_str()), TRUE, TRUE);//Crowd sound effect
	Fx[2].Load(_strdup((SoundPath + "Soundeffects//Cheering0.1.wav").c_str()), TRUE, FALSE);//Cheering effect, for when someone scores a goal

	//Set Inital Volume of FX
	FxChannel[0]->setVolume(1.0);
	FxChannel[1]->setVolume(0.1);
	FxChannel[2]->setVolume(0.8);

	for (int i = 0; i < 2; i++) {
		Sound::Sys.listenerPos[i].x = 0.0f; Sound::Sys.listenerPos[i].y = 0.0f; Sound::Sys.listenerPos[i].z = 0.0f;
	}
}

/* function InitializeObjects()
* Description:
*   - this is called when the file first loads
*   - loads all the needed objects into the game
*/
void InitializeObjects()
{
	auto defaultMaterial = materials["default"];
	auto passThroughMaterial = materials["passThrough"];
	//Load Objects///////////////////////////////////
	std::string ImagePath;
	std::string ObjectPath;
	if (DoesFileExists("..//Assets//Img") && DoesFileExists("..//Assets//Obj")) {
		ImagePath = "..//Assets//Img//";
		ObjectPath = "..//Assets//Obj//";
	}
	else if (DoesFileExists("Assets//Img") && DoesFileExists("Assets//Obj")) {
		ImagePath = "Assets//Img//";
		ObjectPath = "Assets//Obj//";
	}
	else { std::cout << "[ERROR] Could not find [Img] and/or [Obj]" << std::endl; }

	Borders[0].objectLoader(ObjectPath + "Square.obj");
	Borders[0].setMaterial(passThroughMaterial);
	Borders[0].setMass(0.0f);
	Borders[0].setScale(glm::vec3(98.0f, 100.0f, 98.0f));
	Borders[0].setSizeOfHitBox(glm::vec3(98.0f, 100.0f, 98.0f)); //HitBox
	Borders[0].setPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	Borders[0].setColour(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));


	//Player ONE
	Players[0].objectLoader(ObjectPath + "Player//blitzbot.obj");
	Players[0].setMaterial(passThroughMaterial);
	Players[0].setMass(5.0f);
	Players[0].setScale(glm::vec3(5.0f, 5.0f, 5.0f)); //displayed size
	Players[0].setSizeOfHitBox(glm::vec3(10.0f, 2.50f, 10.0f)); //HitBox
	Players[0].setPosition(glm::vec3(15.0f, -1.0f, 0.0f)); //Position of Object
	Players[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//B_blitzbot_diff.png").c_str())));
	for (int i = 1; i < NumberOfPlayers; i++) {
		Players[i].objectLoader(&Players[0]);
		Players[i].setPosition(glm::vec3(-15.0f, -1.0f, 0.0f)); //Position of Object
	}

	//ShockWave
	AffectsOnPlayer[0][0].objectLoader(ObjectPath + "Player//ShockWave.obj");
	AffectsOnPlayer[0][0].setMaterial(passThroughMaterial);
	AffectsOnPlayer[0][0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//ShockWave_01.png").c_str())));
	AffectsOnPlayer[0][0].setMass(10.0f);
	AffectsOnPlayer[0][0].setScale(glm::vec3(5.0f, 5.0f, 5.0f)); //displayed size
	AffectsOnPlayer[0][0].setSizeOfHitBox(glm::vec3(1.0f, 1.0f, 1.0f)); //HitBox
	AffectsOnPlayer[0][0].textureHandle_hasTransparency = true;
	//bolt
	AffectsOnPlayer[0][1].objectLoader(ObjectPath + "Player//bolt.obj");
	AffectsOnPlayer[0][1].setMaterial(passThroughMaterial);
	AffectsOnPlayer[0][1].setScale(glm::vec3(5.0f, 5.0f, 5.0f)); //displayed size
	AffectsOnPlayer[0][1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//bolt.png").c_str())));
	AffectsOnPlayer[0][1].textureHandle_hasTransparency = true;
	AffectsOnPlayer[0][1].Viewable = false;
	//dazzle
	AffectsOnPlayer[0][2].objectLoader(ObjectPath + "Player//dazzle.obj");
	AffectsOnPlayer[0][2].setMaterial(passThroughMaterial);
	AffectsOnPlayer[0][2].setScale(glm::vec3(5.0f, 5.0f, 5.0f)); //displayed size
	AffectsOnPlayer[0][2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//dazzle.png").c_str())));
	AffectsOnPlayer[0][2].textureHandle_hasTransparency = true;
	AffectsOnPlayer[0][2].Viewable = false;

	//set player affect objects
	for (int i = 1; i < NumberOfPlayers; i++) {
		for (int j = 0; j < NumberOfAffects; j++) {
			AffectsOnPlayer[i][j].objectLoader(&AffectsOnPlayer[0][j]);
		}
	}

	//set player team textures
	for (int i = 0; i < NumberOfPlayers; i++) {
		if (PlayerTeam[i] == 0) {
			Players[i].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//B_blitzbot_diff.png").c_str())));
			AffectsOnPlayer[i][0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//ShockWave0.3b1.png").c_str())));
		}
		else if (PlayerTeam[i] == 1) { 
			Players[i].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//R_blitzbot_diff.png").c_str())));
			AffectsOnPlayer[i][0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//ShockWave0.3r1.png").c_str())));
		}
	}


	//Player ONE Tower
	Rifts[0].objectLoader(ObjectPath + "Square.obj");
	Rifts[0].setMaterial(passThroughMaterial);
	Rifts[0].setColour(glm::vec4(1.5f, 1.5f, 0.0f, 0.10f));
	Rifts[0].setMass(0.0f);
	Rifts[0].setScale(glm::vec3(2.0f, 14.0f, 30.0f));
	Rifts[0].setSizeOfHitBox(glm::vec3(2.0f, 14.0f, 30.0f)); //HitBox
	Rifts[0].setPosition(glm::vec3(48.0f, 7.0f, 0.0f));
	//Rifts[0].textureHandle_hasTransparency = true;
	Rifts[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Hexagons.png").c_str())));
	//Player TWO Tower	//Rift_01  Rift_02  
	Rifts[1].objectLoader(&Rifts[0]);
	Rifts[1].setPosition(glm::vec3(-48.0f, 7.0f, 0.0f));
	Rifts[1].setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
	//Rifts[1].textureHandle_hasTransparency = true;
	Rifts[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Hexagons.png").c_str())));

	//Specials
	Specials[0].objectLoader(ObjectPath + "Square.obj");
	Specials[0].setMaterial(passThroughMaterial);
	Specials[0].setColour(glm::vec4(1.5f, 1.5f, 1.5f, 1.0f));
	Specials[0].setMass(1.0f);
	Specials[0].setScale(glm::vec3(4.0f, 4.0f, 4.0f));
	Specials[0].setSizeOfHitBox(glm::vec3(4.0f, 2.0f, 4.0f));
	Specials[0].setSpecialAttribute(0);
	Specials[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Box.png").c_str())));
	Specials[0].Viewable = false;
	//Specials[1].objectLoader(&Specials[0]);
	Specials[1].objectLoader(ObjectPath + "PowerUp_Icons//Seeker_Swarm_Icon.obj");
	Specials[1].setMaterial(passThroughMaterial);
	Specials[1].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[1].setMass(1.0f);
	Specials[1].setSpecialAttribute(1);
	Specials[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[1].Viewable = false;
	//Specials[2].objectLoader(&Specials[0]);
	Specials[2].objectLoader(ObjectPath + "PowerUp_Icons//Toss-Up_Icon.obj");
	Specials[2].setMaterial(passThroughMaterial);
	Specials[2].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[2].setMass(1.0f);
	Specials[2].setSpecialAttribute(2);
	Specials[2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[2].Viewable = false;
	//Specials[3].objectLoader(&Specials[0]);
	Specials[3].objectLoader(ObjectPath + "PowerUp_Icons//Health_Icon.obj");
	Specials[3].setMaterial(passThroughMaterial);
	Specials[3].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[3].setMass(1.0f);
	Specials[3].setSpecialAttribute(3);
	Specials[3].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[3].Viewable = false;
	//Specials[4].objectLoader(&Specials[0]);
	Specials[4].objectLoader(ObjectPath + "PowerUp_Icons//Boost_Icon.obj");
	Specials[4].setMaterial(passThroughMaterial);
	Specials[4].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[4].setMass(1.0f);
	Specials[4].setSpecialAttribute(4);
	Specials[4].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[4].Viewable = false;
	//Specials[5].objectLoader(&Specials[0]);
	Specials[5].objectLoader(ObjectPath + "PowerUp_Icons//Repulsor_Icon.obj");
	Specials[5].setMaterial(passThroughMaterial);
	Specials[5].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[5].setMass(1.0f);
	Specials[5].setSpecialAttribute(5);
	Specials[5].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[5].Viewable = false;
	//Specials[6].objectLoader(&Specials[0]);
	Specials[6].objectLoader(ObjectPath + "PowerUp_Icons//Short_Circuit_Icon.obj");
	Specials[6].setMaterial(passThroughMaterial);
	Specials[6].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[6].setMass(1.0f);
	Specials[6].setSpecialAttribute(6);
	Specials[6].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[6].Viewable = false;
	//Specials[7].objectLoader(&Specials[0]);
	Specials[7].objectLoader(ObjectPath + "PowerUp_Icons//Super_Shockwave_Icon.obj");
	Specials[7].setMaterial(passThroughMaterial);
	Specials[7].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[7].setMass(1.0f);
	Specials[7].setSpecialAttribute(7);
	Specials[7].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[7].Viewable = false;
	//Specials[8].objectLoader(&Specials[0]);
	Specials[8].objectLoader(ObjectPath + "PowerUp_Icons//Invincibility_Icon.obj");
	Specials[8].setMaterial(passThroughMaterial);
	Specials[8].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[8].setMass(1.0f);
	Specials[8].setSpecialAttribute(8);
	Specials[8].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[8].Viewable = false;
	//Specials[9].objectLoader(&Specials[0]);
	Specials[9].objectLoader(ObjectPath + "PowerUp_Icons//Flipped_Icon.obj");
	Specials[9].setMaterial(passThroughMaterial);
	Specials[9].setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
	Specials[9].setMass(1.0f);
	Specials[9].setSpecialAttribute(9);
	Specials[9].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
	Specials[9].Viewable = false;



	//Ground
	Objects[0].objectLoader(ObjectPath + "Rift//Ground.obj");
	Objects[0].setMaterial(passThroughMaterial);
	Objects[0].setColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	Objects[0].setMass(0.0f);
	Objects[0].setSizeOfHitBox(glm::vec3(100.0f, 0.01f, 100.0f)); //HitBox
	Objects[0].setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	Objects[0].setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	Objects[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Ground0.3.png").c_str())));

	//Walls Left
	Objects[1].objectLoader(ObjectPath + "Rift//stadium.obj");
	Objects[1].setMaterial(passThroughMaterial);
	Objects[1].setColour(glm::vec4(0.5f, 0.50f, 0.5f, 1.0f));
	Objects[1].setMass(0.0f);
	Objects[1].setScale(glm::vec3(1.0f, 1.0f, 1.0f)); //size
	Objects[1].setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	Objects[1].setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
	Objects[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Rockwall.png").c_str())));
	//Rift magnets
	Objects[2].objectLoader(ObjectPath + "Rift//Magnet_Left.obj");
	Objects[2].setMaterial(passThroughMaterial);
	Objects[2].setPosition(glm::vec3(47.0f, 0.0f, -20.0f));
	Objects[2].setMass(0.0f);
	Objects[2].setScale(glm::vec3(1.0f, 1.50f, 1.50f));
	Objects[2].setSizeOfHitBox(glm::vec3(2.650f, 7.0f, 3.60f));
	Objects[2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Magnet_Rift_01.png").c_str())));
	Objects[3].objectLoader(ObjectPath + "Rift//Magnet_Right.obj");
	Objects[3].setMaterial(passThroughMaterial);
	Objects[3].setPosition(glm::vec3(47.0f, 0.0f, 20.0f));
	Objects[3].setMass(0.0f);
	Objects[3].setScale(glm::vec3(1.0f, 1.50f, 1.50f));
	Objects[3].setSizeOfHitBox(glm::vec3(2.650f, 7.0f, 3.60f));
	Objects[3].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Magnet_Rift_01.png").c_str())));
	Objects[4].objectLoader(&Objects[2]);//left
	Objects[4].setPosition(glm::vec3(-47.0f, 0.0f, 20.0f));
	Objects[4].setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
	Objects[4].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Magnet_Rift_02.png").c_str())));
	Objects[5].objectLoader(&Objects[3]);//right
	Objects[5].setPosition(glm::vec3(-47.0f, 0.0f, -20.0f));
	Objects[5].setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
	Objects[5].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Magnet_Rift_02.png").c_str())));

	Objects[6].objectLoader(ObjectPath + "Rift//crowd.obj");
	Objects[6].setMaterial(passThroughMaterial);
	Objects[6].setColour(glm::vec4(0.5f, 0.50f, 0.5f, 1.0f));
	Objects[6].setMass(0.0f);
	Objects[6].setScale(glm::vec3(1.0f, 1.0f, 1.0f)); //size
	Objects[6].setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	Objects[6].setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
	Objects[6].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//crowd.png").c_str())));


	//Enemies
	Enemies[0].objectLoader(ObjectPath + "Enemies//Enemie_LP.obj");
	Enemies[0].setMaterial(passThroughMaterial);
	Enemies[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Enemies//Enemy0.2.png").c_str())));
	Enemies[1].objectLoader(&Enemies[0]);
	Enemies[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Enemies//Enemy0.3.png").c_str())));

	ShadowObject[0].objectLoader(ObjectPath + "PlainForShadow.obj");
	ShadowObject[0].setMaterial(passThroughMaterial);
	ShadowObject[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Shadow_01.png").c_str())));
	ShadowObject[0].textureHandle_hasTransparency = true;
	ShadowObject[1].objectLoader(&ShadowObject[0]);
	ShadowObject[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Shadow_01.png").c_str())));

	for (unsigned int i = 2; i < NumberOfEnemies; i++) {
		
		if (i % 2 == 0) {
			Enemies[i].objectLoader(&Enemies[0]);
		}
		else {
			Enemies[i].objectLoader(&Enemies[1]);
		}

	}


	//Load Line Segments
	//morphmath.loadMorphSegments("Obj/" "LineSegment.txt");
	//HTRLoader skeleton;
	//skeleton.loadHTR("assets/animations/Orbit.htr");
	//skeleton.createGameObjects();
	//Players[0].addChild(skeleton.getRootGameObject());

}



void init()
{
	srand(static_cast <unsigned> (time(NULL)));

	//Textures & Texture parameters
	glEnable(GL_TEXTURE_2D);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	///// INIT OpenGL /////
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_COLOR_MATERIAL); // final polygon color will be based on glColor and glMaterial
	glShadeModel(GL_FLAT); //GL_FLAT //GL_SMOOTH
	glEnable(GL_CULL_FACE); //glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);//Enable Z-buffer read and write (for hidden surface removal)
	glEnable(GL_BLEND); //Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //non-premultiplied alpha
	glLineWidth(4.0f);
	glPolygonMode(GL_FRONT, GL_FILL);


	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);

	std::cout << "_____________________________________" << std::endl;
	TextLoader loadTextFile;
	if (DoesFileExists("..//Assets")) { loadTextFile.objectLoader("..//Assets//file.txt"); }
	else if (DoesFileExists("Assets")) { loadTextFile.objectLoader("Assets//file.txt"); }
	else { std::cout << "[ERROR] Could not find [Font]" << std::endl; }

	if (DoesFileExists("..//Assets//Fonts//")) { SystemText.LoadTextFont("..//Assets//Fonts//FreeSerif.ttf", SystemText); }
	else if (DoesFileExists("Assets//Fonts//")) { SystemText.LoadTextFont("Assets//Fonts//FreeSerif.ttf", SystemText); }
	else { std::cout << "[ERROR] Could not find [Font]" << std::endl; }

	if (true) {
		PlayerTeam[0] = 0;
		PlayerTeam[1] = 1;
		PlayerTeam[2] = 0;
		PlayerTeam[3] = 1;
	}
	else {
		PlayerTeam[0] = 1;
		PlayerTeam[1] = 0;
		PlayerTeam[2] = 1;
		PlayerTeam[3] = 0;
	}

	InitializeShaders();
	std::cout << "_____________________________________" << std::endl;
	InitializeTextPlane();
	std::cout << "_____________________________________" << std::endl;
	InitializeSounds();
	std::cout << "_____________________________________" << std::endl;
	InitializeObjects();
	std::cout << "_____________________________________" << std::endl;



	//setting abilitys
	for (int i = 0; i < NumberOfPlayers; i++) {
		//PlayerTeam[i] = i;
		for (int j = 0; j < NumberOfSpecials; j++) {
			AbilityAffected[i][j] = false;
			AbilityCounter[i][j] = 6.0f;
			AbilityLength[i][j] = 6.0f;
		}
	}
	randomSpecialTime = (rand() % 45 + 15) + ((rand() % 100 - 50)*0.01f);
	//setting all four camera's
	for (int i = 0; i < 4; i++) {
		cameralook = i;
		WhatCameraIsLookingAt();
	}

	std::cout << std::endl << "Press [Esc] to exit." << std::endl;
	std::cout << "_____________________________________" << std::endl;
}

/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/
int main(int argc, char **argv)
{
	//Command Screen
	HWND consoleWindow1 = GetConsoleWindow();
	SetConsoleTitle("Rift Ball : Console");
	SetWindowPos(consoleWindow1, NULL, 10, 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//Initialize the window and OpenGL properly
	glutInit(&argc, argv);
	glutInitWindowPosition(350, 10);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Rift Ball");
	glutGetWindow();


	//Initialize OpenGL Extention Wrangler
	GLenum res = glewInit();
	if (res != GLEW_OK) { std::cerr << "Glew failed to initialize!" << std::endl; }
	//check OpenGL version
	printf("Detecting OpenGL version... ");
	const unsigned char *version = glGetString(GL_VERSION);
	printf("Using OpenGL version %s\n", version);
	//set up our function callbacks
	glutDisplayFunc(DisplayCallbackFunction); // draw
	glutKeyboardFunc(KeyboardCallbackFunction); // keyDown
	glutKeyboardUpFunc(KeyboardUpCallbackFunction); // keyUp
	glutReshapeFunc(WindowReshapeCallbackFunction); // windowResized
	glutMouseFunc(MouseClickCallbackFunction); // mouseClick
	glutMotionFunc(MouseMotionCallbackFunction); // mouseMovedActive
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction); // mouseMovedPassive
	glutTimerFunc(1, TimerCallbackFunction, 0); // timer or tick

	init();

	// start the event handler
	glutMainLoop();

	return 0;
}


