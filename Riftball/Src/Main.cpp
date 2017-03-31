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
#include "Useful_Functions\MorphMath.h"
#include "Useful_Functions\GamePadControls.h"
#include "Useful_Functions\ExtraFunctions.cpp"
#include "FMOD\SoundEngine.h"
#include "Other_Functions\FileLoader.h" //not in use atm

// Tutoral Libraries
#include "Shader_Functions\Shader.h"
#include "Shader_Functions\ShaderProgram.h"
#include "Shader_Functions\Material.h"
#include "Useful_Functions\FrameBufferObject.h"
#include "Useful_Functions\ParticleEmitterSoA.h"



//temp
static float TestFloat = 0.0f; static float TestFloatIncrementAmount = 01.0f;
//////////////////////////////////////////////////////////////////////

// Create Shader
Shader *shader; Shader *TextShader;
glm::vec3 lightPosition(0.01f, 100.0f, 0.01f);

// Defines Core variables//
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Miliseconds per frame
float deltaTasSecs;
int windowWidth = (16*80); int windowHeight = (9*80); //set window dimentions
int mousepositionX; int mousepositionY; //
int screenpositionX; int screenpositionY; //
float MPosToOPosX; float MPosToOPosY; //mouse position to object position
bool keyDown[256];  //key input
bool ControllerAPress[4]{ false }; bool mouseDown[3];//check if a key is down or up
//Materials
std::map<std::string, std::shared_ptr<Material>> materials;
//Game Objects//
std::map<std::string, int> MenuObjectsAmount; std::map<std::string, std::shared_ptr<GameObject>> MenuObjects;
std::map<std::string, int> GameObjectsAmount; std::map<std::string, std::shared_ptr<GameObject>> GameObjects;
Sliders *Slider; Buttons *Button;
//Particals
ParticleEmitterSoA PlayerParticleEmitter[4];
ParticleEmitterSoA EnemyParticleEmitter[12];
//Framebuffer objects
std::map<std::string, std::shared_ptr<FrameBufferObject>> FBOs;
//Sounds
Sound powerup[9]; FMOD::Channel *powChannel[3];//Powerup sounds
Sound Fx[3]; FMOD::Channel *FxChannel[3];//Fx sounds


//textures
std::map<std::string, GLuint> Textures;
GLuint TextureNumbers[10]; GLuint PlayerTextures[12]; GLuint PlayerSelectColour[7];
GLuint ParticleTexture;
//structures
Gamepad gamepad; Manifold m; MorphMath morphmath; RenderText SystemText;
PlayerHealth PlayerHp[2]; PlayerInfo *PlayerValues; Camera cameras[4];
Quad myVBO;
//Bool's
bool inMenu = true; bool inGame = false; bool inOptions = false;
int inOptionsTab = 0; std::string lastMenu = "inMenu";
bool ApplyingGravity = true; bool CollisionBetweenObjects = true;
bool IdleEnemiesRespawn = true; bool EnableShadows = true; bool TeamThatLost[2];
float randomSpecialTime;
glm::vec3 speedToWallDegradation = glm::vec3(0.80f, 0.50f, 0.80f);
int cameralook = 0;//camera in use



//////////////////////////////////////////////////////////////////////

/* function setBoardStart()
* Description:
*  - this sets enemies and players positions
*  - set player health
*/
void setBoardStart() {

	if (&Slider[2] != NULL) {
		PlayerHp[0].CurrentHealth = static_cast<int>(Slider[2].SNob_Precent.x);
		PlayerHp[1].CurrentHealth = static_cast<int>(Slider[2].SNob_Precent.x);
	}

	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++)
	{
		if (&GameObjects["Players_0" + to_string(i)] != NULL) {
			float multipliyer002 = 0.0f;
			if (i > 1) { multipliyer002 += 20.0f; }
			if (PlayerValues[i].PlayerTeam == 0) { GameObjects["Players_0" + to_string(i)].get()->setPosition(glm::vec3(20.0f, 0.0f, -10.0f + multipliyer002 + Random(-10.0f, 10.0f))); }
			else if (PlayerValues[i].PlayerTeam == 1) { GameObjects["Players_0" + to_string(i)].get()->setPosition(glm::vec3(-20.0f, 0.0f, -10.0f + multipliyer002 + Random(-10.0f, 10.0f))); }

			GameObjects["Players_0" + to_string(i)].get()->setVelocity(glm::vec3(0.0f));
			GameObjects["Players_0" + to_string(i)].get()->setForceOnObject(glm::vec3(0.0f));
			GameObjects["Players_0" + to_string(i)].get()->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
		}
	}

	if (&GameObjects["Enemies_0" + to_string(0)] != NULL) { m.A = GameObjects["Enemies_0" + to_string(0)]; }
	for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++)
	{
		if (&GameObjects["Enemies_0" + to_string(i)] != NULL) {
			m.B = GameObjects["Enemies_0" + to_string(i)];
			setEnemySpawn(m, i);
			GameObjects["Enemies_0" + to_string(i)] = m.B;
		}
	}

	for (int i = 0; i < GameObjectsAmount["Specials_0"]; i++)
	{
		if (&GameObjects["Specials_0" + to_string(i)] != NULL) {
			GameObjects["Specials_0" + to_string(i)].get()->Viewable = false;
			GameObjects["Specials_0" + to_string(i)].get()->setVelocity(glm::vec3(0.0f));
			GameObjects["Specials_0" + to_string(i)].get()->setForceOnObject(glm::vec3(0.0f));
			GameObjects["Specials_0" + to_string(i)].get()->setRotation(glm::vec3(0.0f));
		}
	}
}

void exitProgram() {

	//if (&ShadowObject != NULL) { ShadowObject.~GameObject(); }
	//for (unsigned int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
	//	if (&Players[i] != NULL) { GameObjects["Players_0" + to_string(i)].get()->~GameObject(); }
	//	for (unsigned int j = 0; j < GameObjectsAmount["Affects_0"]; j++) {
	//		if (&AffectsOnPlayer[i][j] != NULL) { AffectsOnPlayer[i][j].~GameObject(); }
	//	}
	//}
	//for (unsigned int i = 0; i < GameObjectsAmount["Rifts_0"]; i++) { if (&Rifts[i] != NULL) { Rifts[i].~GameObject(); } }
	//for (unsigned int i = 0; i < GameObjectsAmount["Objects_0"]; i++) { if (&Objects[i] != NULL) { Objects[i].~GameObject(); } }
	//for (unsigned int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) { if (&Enemies[i] != NULL) { Enemies[i].~GameObject(); } }
	//for (unsigned int i = 0; i < GameObjectsAmount["Specials_0"]; i++) { if (&Specials[i] != NULL) { GameObjects["Specials_0" + to_string(i)].get()->~GameObject(); } }
	//for (unsigned int i = 0; i < NumberOfPlaneForText; i++) { if (&planeForText[i] != NULL) { planeForText[i].~GameObject();} }
	//for (unsigned int i = 0; i < NumberOfSliders; i++) { 
	//	if (&planeForSliders[i] != NULL) { planeForSliders[i].~GameObject(); }
	//	if (&ButtonForSliders[i] != NULL) { ButtonForSliders[i].~GameObject(); }
	//}
	//for (unsigned int i = 0; i < NumberOfButtons; i++) { if (&ButtonObjects[i] != NULL) { ButtonObjects[i].~GameObject(); } }
	//for (unsigned int i = 0; i < NumberOfBorders; i++) { if (&Borders[i] != NULL) { Borders[i].~GameObject(); } }

	for (std::map<std::string, std::shared_ptr<Material>>::iterator itr = materials.begin(); itr != materials.end(); itr++) { materials.erase(itr); }

	glutLeaveMainLoop();
}


/* function WhatCameraIsLookingAt(void)
* Description:
*  - this sets what the player will be seeing
*/
void WhatCameraIsLookingAt(int PlayerLookAt)//int CameraLookAt
{
	if (PlayerLookAt == 0)
	{
		if (GameObjectsAmount["Players_0"] == 4) {
			for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
				for (int j = 0; j < GameObjectsAmount["Players_0"]; j++) {
					if ((i != j) && (PlayerValues[i].PlayerTeam == 0) && (PlayerValues[i].PlayerTeam == PlayerValues[j].PlayerTeam)) {
						glViewport(0, 0, (windowWidth / 2), windowHeight);
						cameras[0].projMatrix = glm::perspective(45.0f, (windowWidth / windowHeight)*1.0f, 0.01f, 100000.f);

						glm::vec3 CameraLocation = morphmath.Lerp(
							glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x + 50.0f), 50.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z)),
							glm::vec3((GameObjects["Players_0" + to_string(j)].get()->Position().x + 50.0f), 50.0f, (GameObjects["Players_0" + to_string(j)].get()->Position().z)),
							0.5f);
						glm::vec3 CameraLookAt = morphmath.Lerp(
							glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x), 1.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z)),
							glm::vec3((GameObjects["Players_0" + to_string(j)].get()->Position().x), 1.0f, (GameObjects["Players_0" + to_string(j)].get()->Position().z)),
							0.5f);

						if (PlayerValues[i].cameraMode == 0) {
							glm::mat4x4 transform = glm::lookAt(
								CameraLocation,
								CameraLookAt,
								glm::vec3(0.0f, 1.0f, 0.0f));
							cameras[0].viewMatrix = transform;
							PlayerValues[i].cameraPosition = CameraLocation;
							PlayerValues[j].cameraPosition = CameraLocation;
						}
						else if (PlayerValues[i].cameraMode == 1) {
							glm::mat4x4 transform = glm::lookAt(
								PlayerValues[PlayerLookAt].cameraPosition,
								PlayerValues[PlayerLookAt].cameraPosition + PlayerValues[PlayerLookAt].forwardVector,
								glm::vec3(0.0f, 1.0f, 0.0f));
							cameras[0].viewMatrix = transform;
						}
						//cameraViewMatrix[0] = glm::mat4(glm::translate(CameraLocation)) * modelViewMatrix[0];
					}
				}
			}
		}
		else {
			for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
				if (PlayerValues[i].PlayerTeam == 0) {
					glViewport(0, 0, (windowWidth / 2), windowHeight);
					cameras[0].projMatrix = glm::perspective(45.0f, (windowWidth / windowHeight)*1.0f, 0.01f, 10000.f);

					glm::vec3 CameraLocation = glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x + 50.0f), 50.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z));
					glm::vec3 CameraLookAt = glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x), 1.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z));

					if (PlayerValues[i].cameraMode == 0) {
						glm::mat4x4 transform = glm::lookAt(
							CameraLocation,
							CameraLookAt,
							glm::vec3(0.0f, 1.0f, 0.0f));
						PlayerValues[i].cameraPosition = CameraLocation;
						cameras[0].viewMatrix = transform;
					}
					else if (PlayerValues[i].cameraMode == 1) {
						glm::mat4x4 transform = glm::lookAt(
							PlayerValues[PlayerLookAt].cameraPosition,
							PlayerValues[PlayerLookAt].cameraPosition + PlayerValues[PlayerLookAt].forwardVector,
							glm::vec3(0.0f, 1.0f, 0.0f));
						cameras[0].viewMatrix = transform;
					}
					//cameraViewMatrix[0] = glm::mat4(glm::translate(CameraLocation)) * modelViewMatrix[0];
				}
			}
		}
	}
	else if (PlayerLookAt == 1) {
		if (GameObjectsAmount["Players_0"] == 4) {
			for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
				for (int j = 0; j < GameObjectsAmount["Players_0"]; j++) {
					if ((i != j) && (PlayerValues[i].PlayerTeam == 1) && (PlayerValues[i].PlayerTeam == PlayerValues[j].PlayerTeam)) {
						glViewport((windowWidth / 2), 0, (windowWidth / 2), windowHeight);
						cameras[1].projMatrix = glm::perspective(45.0f, (windowWidth / windowHeight)*1.0f, 0.01f, 100000.f);

						glm::vec3 CameraLocation = morphmath.Lerp(
							glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x - 50.0f), 50.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z)),
							glm::vec3((GameObjects["Players_0" + to_string(j)].get()->Position().x - 50.0f), 50.0f, (GameObjects["Players_0" + to_string(j)].get()->Position().z)),
							0.5f);
						glm::vec3 CameraLookAt = morphmath.Lerp(
							glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x), 1.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z)),
							glm::vec3((GameObjects["Players_0" + to_string(j)].get()->Position().x), 1.0f, (GameObjects["Players_0" + to_string(j)].get()->Position().z)),
							0.5f);

						if (PlayerValues[i].cameraMode == 0) {
							glm::mat4x4 transform = glm::lookAt(
								CameraLocation,
								CameraLookAt,
								glm::vec3(0.0f, 1.0f, 0.0f));
							cameras[1].viewMatrix = transform;
							PlayerValues[i].cameraPosition = CameraLocation;
							PlayerValues[j].cameraPosition = CameraLocation;
						}
						else if (PlayerValues[i].cameraMode == 1) {
							glm::mat4x4 transform = glm::lookAt(
								PlayerValues[PlayerLookAt].cameraPosition,
								PlayerValues[PlayerLookAt].cameraPosition + PlayerValues[PlayerLookAt].forwardVector,
								glm::vec3(0.0f, 1.0f, 0.0f));
							cameras[1].viewMatrix = transform;
						}
						//cameraViewMatrix[1] = glm::mat4(glm::translate(CameraLocation)) * modelViewMatrix[1];
					}
				}
			}
		}
		else {
			for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
				if (PlayerValues[i].PlayerTeam == 1) {
					glViewport((windowWidth / 2), 0, (windowWidth / 2), windowHeight);
					cameras[1].projMatrix = glm::perspective(45.0f, (windowWidth / windowHeight)*1.0f, 0.01f, 100000.f);

					glm::vec3 CameraLocation = glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x - 50.0f), 50.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z));
					glm::vec3 CameraLookAt = glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x), 1.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z));

					if (PlayerValues[i].cameraMode == 0) {
						glm::mat4x4 transform = glm::lookAt(
							CameraLocation,
							CameraLookAt,
							glm::vec3(0.0f, 1.0f, 0.0f));
						cameras[1].viewMatrix = transform;
						PlayerValues[i].cameraPosition = CameraLocation;
					}
					else if (PlayerValues[i].cameraMode == 1) {
						glm::mat4x4 transform = glm::lookAt(
							PlayerValues[PlayerLookAt].cameraPosition,
							PlayerValues[PlayerLookAt].cameraPosition + PlayerValues[PlayerLookAt].forwardVector,
							glm::vec3(0.0f, 1.0f, 0.0f));
						cameras[1].viewMatrix = transform;
					}
					//cameraViewMatrix[1] = glm::mat4(glm::translate(CameraLocation)) * modelViewMatrix[1];
				}
			}
		}
	}

	cameras[0].viewProjMatrix = cameras[0].projMatrix * cameras[0].viewMatrix;
	cameras[1].viewProjMatrix = cameras[1].projMatrix * cameras[1].viewMatrix;
}
void WhatCameraIsLookingAt()//int CameraLookAt
{
	if (cameralook == 2 || cameralook == 0)
	{
		glViewport(0, 0, windowWidth, windowHeight);
		cameras[2].projMatrix = glm::perspective(45.0f, (windowWidth / windowHeight)*1.0f, 0.1f, 10000.f);

		glm::mat4x4 transform = glm::lookAt(
			glm::vec3(0.0f, 50.0f, 0.0f),//eye
			glm::vec3(0.01f, 0.01f, 0.01f),//center
			glm::vec3(0.0f, 0.0f, -1.0f));//up
		cameras[2].viewMatrix = transform;
		glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 0.0f);
		//cameraViewMatrix[2] = glm::mat4(glm::translate(cameraPos)) * modelViewMatrix[2];
	}
	else if (cameralook == 3 || cameralook == 1)
	{
		glViewport(0, 0, windowWidth, windowHeight);
		cameras[3].projMatrix = glm::ortho<float>(-windowWidth*0.50f, windowWidth*0.50f, -windowHeight*0.50f, windowHeight*0.50f);

		glm::mat4x4 transform = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 10.0f),//eye
			glm::vec3(0.01f, 0.01f, 0.01f),//center
			glm::vec3(0.0f, 1.0f, 0.0f));//up
		cameras[3].viewMatrix = transform;
		glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 0.0f);
		//cameraViewMatrix[3] = glm::mat4(glm::translate(cameraPos)) * modelViewMatrix[3];
	}
	cameras[2].viewProjMatrix = cameras[2].projMatrix * cameras[2].viewMatrix;
	cameras[3].viewProjMatrix = cameras[3].projMatrix * cameras[3].viewMatrix;
}


/* function ControllerDelayButton()
* Description:
*   - this is called to get inputs from controllers
*/
void ControllerDelayButton(int portNumber, float deltaTasSeconds)
{
	float MovementModifier = 12.0f;


	if (inGame) {
		if (PlayerValues[portNumber].cameraMode == 0) {
			//JoySticks
			if (PlayerValues[portNumber].PlayerTeam == 0) {
				float Tx = 0.0f; float Ty = 0.0f; float Tz = 0.0f; float rotY = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
				if (!PlayerValues[portNumber].FlipedControllers) {
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

				GameObjects["Players_0" + to_string(portNumber)].get()->setForceOnObject(glm::vec3(Tx, Ty, Tz));
				GameObjects["Players_0" + to_string(portNumber)].get()->setVelocity(glm::vec3(Tx, Ty, Tz)*GameObjects["Players_0" + to_string(portNumber)].get()->SprintSpeed);
				GameObjects["Players_0" + to_string(portNumber)].get()->ForwardDirection = (GameObjects["Players_0" + to_string(portNumber)].get()->Position() - (GameObjects["Players_0" + to_string(portNumber)].get()->Position() + GameObjects["Players_0" + to_string(portNumber)].get()->Velocity()));
			}
			if (PlayerValues[portNumber].PlayerTeam == 1) {
				float Tx = 0.0f; float Ty = 0.0f; float Tz = 0.0f; float rotY = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
				if (!PlayerValues[portNumber].FlipedControllers) {
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

				GameObjects["Players_0" + to_string(portNumber)].get()->setForceOnObject(glm::vec3(Tx, Ty, Tz));
				GameObjects["Players_0" + to_string(portNumber)].get()->setVelocity(glm::vec3(Tx, Ty, Tz)*GameObjects["Players_0" + to_string(portNumber)].get()->SprintSpeed);
				GameObjects["Players_0" + to_string(portNumber)].get()->ForwardDirection = (GameObjects["Players_0" + to_string(portNumber)].get()->Position() - (GameObjects["Players_0" + to_string(portNumber)].get()->Position() + GameObjects["Players_0" + to_string(portNumber)].get()->Velocity()));
			}
		}
		else if (PlayerValues[portNumber].cameraMode == 1) {
			if (portNumber >= 0) {
				float Tmovement = 0.0f; float yaw = 0.0f; float pitch = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.

				if (!PlayerValues[portNumber].FlipedControllers) {
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
				PlayerValues[portNumber].rightVector = glm::cross(PlayerValues[portNumber].forwardVector, glm::vec3(0.0f, 1.0f, 0.0f));
				PlayerValues[portNumber].rightVector = glm::normalize(PlayerValues[portNumber].rightVector);
				PlayerValues[portNumber].forwardVector = glm::rotate(PlayerValues[portNumber].forwardVector, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
				//pitch
				PlayerValues[portNumber].rightVector = glm::cross(PlayerValues[portNumber].forwardVector, glm::vec3(0.0f, 1.0f, 0.0f));
				PlayerValues[portNumber].rightVector = glm::normalize(PlayerValues[portNumber].rightVector);
				PlayerValues[portNumber].forwardVector = glm::rotate(PlayerValues[portNumber].forwardVector, pitch, PlayerValues[portNumber].rightVector);
				//player position
				glm::vec3 forwardVectorTemp = glm::vec3(PlayerValues[portNumber].forwardVector.x, 0.0f, PlayerValues[portNumber].forwardVector.z);
				GameObjects["Players_0" + to_string(portNumber)].get()->ForwardDirection = (forwardVectorTemp * -0.001f);
				GameObjects["Players_0" + to_string(portNumber)].get()->setForceOnObject((Tmovement * forwardVectorTemp)*0.5f);
				GameObjects["Players_0" + to_string(portNumber)].get()->setVelocity((Tmovement * forwardVectorTemp)*0.5f*GameObjects["Players_0" + to_string(portNumber)].get()->SprintSpeed);
				//camera position
				PlayerValues[portNumber].cameraPosition = glm::vec3(GameObjects["Players_0" + to_string(portNumber)].get()->Position().x - (PlayerValues[portNumber].forwardVector.x*6.0f),
					GameObjects["Players_0" + to_string(portNumber)].get()->Position().y + (GameObjects["Players_0" + to_string(portNumber)].get()->Radius().y*2.0f),
					GameObjects["Players_0" + to_string(portNumber)].get()->Position().z - (PlayerValues[portNumber].forwardVector.z*6.0f));
				//shockwace position
			}
		}
		//Buttons
		if (portNumber >= 0) {
			if (PlayerValues[portNumber].MenuSwitchCounter > 0.0f) { PlayerValues[portNumber].MenuSwitchCounter -= deltaTasSeconds; }
			else {
				//first press of [LEFT_TRIGGERED]
				if (gamepad.leftTrigger > 0.2 && PlayerValues[portNumber].Left_TRIGGERED == false) { PlayerValues[portNumber].Left_TRIGGERED = true; std::cout << "[LEFT_TRIGGERED][-]"; }
				//[LEFT_TRIGGERED] was pressed last tic
				else if (PlayerValues[portNumber].Left_TRIGGERED == true) {
					//holding [LEFT_TRIGGERED]
					if (gamepad.leftTrigger > 0.2) {}
					//[LEFT_TRIGGERED] released
					else {
						PlayerValues[portNumber].Left_TRIGGERED = false;
						PlayerValues[portNumber].MenuSwitchCounter = 0.70f;
						std::cout << "[LEFT_TRIGGERED][+]";
					}
				}

				//first press of [RIGHT_TRIGGERED]
				if (gamepad.rightTrigger > 0.2 && PlayerValues[portNumber].Right_TRIGGERED == false) { PlayerValues[portNumber].Right_TRIGGERED = true; std::cout << "[RIGHT_TRIGGERED][-]"; }
				//[RIGHT_TRIGGERED] was pressed last tic
				else if (PlayerValues[portNumber].Right_TRIGGERED == true) {
					//holding [RIGHT_TRIGGERED]
					if (gamepad.rightTrigger > 0.2) {
						if (PlayerValues[portNumber].ShockWaveChargeUp < 0.25f) { PlayerValues[portNumber].ShockWaveChargeUp += (deltaTasSeconds*5.0f); }
						//std::cout << "	[C](" << portNumber << ")[" << PShockWaveChargeUp[portNumber] << "]" << std::endl;
					}
					//[RIGHT_TRIGGERED] released
					else {
						if (PlayerValues[portNumber].ShockWaveChargeUp < 0.50f) { PlayerValues[portNumber].ShockWaveChargeUp = 0.0f; }
						PlayerValues[portNumber].Right_TRIGGERED = false;
						PlayerValues[portNumber].ShockWave = true;
						PlayerValues[portNumber].ShockWaveCounter = 0.250f;
						PlayerValues[portNumber].MenuSwitchCounter = 0.60f;
						PlayerValues[portNumber].SWMaxWeight = 0.0f;
						std::cout << "[RIGHT_TRIGGERED][+]";
					}
					GameObjects["Players_0" + to_string(portNumber)].get()->setVelocity(GameObjects["Players_0" + to_string(portNumber)].get()->Velocity()*0.5f);
				}

				if (gamepad.IsPressed(XINPUT_GAMEPAD_A)) {
					if (GameObjects["Players_0" + to_string(portNumber)].get()->inAir == false && GameObjects["Players_0" + to_string(portNumber)].get()->IsJumping == false) {
						GameObjects["Players_0" + to_string(portNumber)].get()->inAir = true;
						GameObjects["Players_0" + to_string(portNumber)].get()->IsJumping = true;
						GameObjects["Players_0" + to_string(portNumber)].get()->onObject = false;
						GameObjects["Players_0" + to_string(portNumber)].get()->InAirCounter = 0.25f;
						GameObjects["Players_0" + to_string(portNumber)].get()->setPosition(glm::vec3(GameObjects["Players_0" + to_string(portNumber)].get()->Position().x, GameObjects["Players_0" + to_string(portNumber)].get()->Position().y + (GameObjects["Players_0" + to_string(portNumber)].get()->Radius().y * 0.30f), GameObjects["Players_0" + to_string(portNumber)].get()->Position().z));
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_B)) {
					if (PlayerValues[portNumber].SprintCounter < PlayerValues[portNumber].SprintCoolDown && GameObjects["Players_0" + to_string(portNumber)].get()->inAir == false) {
						PlayerValues[portNumber].SprintCounter += deltaTasSeconds;
						GameObjects["Players_0" + to_string(portNumber)].get()->setVelocity(GameObjects["Players_0" + to_string(portNumber)].get()->Velocity()*PlayerValues[portNumber].SprintSpeed);
					}
				}
				else {
					if (PlayerValues[portNumber].SprintCounter > 0.0f && GameObjects["Players_0" + to_string(portNumber)].get()->inAir == false) {
						PlayerValues[portNumber].SprintCounter -= deltaTasSeconds;
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_X)) { std::cout << "[X][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_Y)) {
					GameObjects["Players_0" + to_string(portNumber)].get()->setVelocity(GameObjects["Players_0" + to_string(portNumber)].get()->Velocity()*0.50f);
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_THUMB)) { std::cout << "[LEFT_THUMB][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_THUMB)) { std::cout << "[RIGHT_THUMB][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_UP)) { std::cout << "[DPAD_UP][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN)) { std::cout << "[DPAD_DOWN][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT)) { std::cout << "[DPAD_LEFT][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT)) { std::cout << "[DPAD_RIGHT][-]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_START)) {
					std::cout << "[START][-]";
					PlayerValues[portNumber].MenuSwitchCounter = 1.0f;
				}
				else if (gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) {
					std::cout << "[BACK][-]";
					inGame = false; inMenu = true;
					PlayerValues[portNumber].MenuSwitchCounter = 1.0f;
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
			if (Tx != 0.0f || Ty != 0.0f || Tz != 0.0f) { SetCursorPos(glutGet((GLenum)GLUT_WINDOW_X) + mousepositionX, glutGet((GLenum)GLUT_WINDOW_Y) + mousepositionY); }

			if (PlayerValues[portNumber].MenuSwitchCounter > 0.0f) { PlayerValues[portNumber].MenuSwitchCounter -= deltaTasSeconds; }
			else {
				if (gamepad.IsPressed(XINPUT_GAMEPAD_A)) {
					INPUT input;
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP);
					input.mi.mouseData = 0;
					input.mi.dwExtraInfo = NULL;
					input.mi.time = 0;
					SendInput(1, &input, sizeof(INPUT));
					if (inOptionsTab == 3) {
						bool pressedASlider = false;
						for (int i = 0; i < MenuObjectsAmount["HUD_Sliders_0"]; i++) {
							//move nob along the slider
							if (Slider[i].moveNob(MPosToOPosX, MPosToOPosY)) { MenuObjects["HUD_Slider_Button_0" + to_string(i)]->setPosition(glm::vec3(MPosToOPosX, 0.02f, Slider[i].SBar_Pos.z)); pressedASlider = true; }
						}
						if (!pressedASlider) { PlayerValues[portNumber].MenuSwitchCounter = 0.10f; }
					}
					else if (inOptionsTab == 4 ) {
						PlayerValues[portNumber].MenuSwitchCounter = 0.50f;
						ControllerAPress[portNumber] = true;
						PlayerValues[portNumber].ControllerActive = true;
					}
					else { PlayerValues[portNumber].MenuSwitchCounter = 0.50f; }
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_B)) {
					INPUT input;
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP);
					input.mi.mouseData = 0;
					input.mi.dwExtraInfo = NULL;
					input.mi.time = 0;
					SendInput(1, &input, sizeof(INPUT));

					if (inOptions) {
						if (inOptionsTab == 4 && PlayerValues[portNumber].ControllerActive) {
							PlayerValues[portNumber].ControllerActive = false;
							//inOptions = false; inMenu = true; 
						}
						else if (inOptionsTab != 0) { inOptionsTab = 0; }
						else { inOptions = false; inMenu = true; }
						PlayerValues[portNumber].MenuSwitchCounter = 1.0f;
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_X)) { std::cout << "[X]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_Y)) { std::cout << "[Y]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_UP)) {
					if (inOptionsTab == 4 && PlayerValues[portNumber].ControllerActive) {
						if (PlayerValues[portNumber].PlayerColour % 2 == 0) { PlayerValues[portNumber].PlayerColour = 1; }
						else { PlayerValues[portNumber].PlayerColour = 0; }
						PlayerValues[portNumber].MenuSwitchCounter = 0.50f;
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN)) {
					if (inOptionsTab == 4 && PlayerValues[portNumber].ControllerActive) {
						if (PlayerValues[portNumber].PlayerColour % 2 == 0) { PlayerValues[portNumber].PlayerColour = 1; }
						else { PlayerValues[portNumber].PlayerColour = 0; }
						PlayerValues[portNumber].MenuSwitchCounter = 0.50f;
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT)) {
					if (inOptionsTab == 4 && PlayerValues[portNumber].ControllerActive) {
						if (PlayerValues[portNumber].PlayerColour > 1) { PlayerValues[portNumber].PlayerColour -= 2; }
						else { PlayerValues[portNumber].PlayerColour += 4; }
						PlayerValues[portNumber].MenuSwitchCounter = 0.50f;
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT)) {
					if (inOptionsTab == 4 && PlayerValues[portNumber].ControllerActive) {
						if (PlayerValues[portNumber].PlayerColour < 4) { PlayerValues[portNumber].PlayerColour += 2; }
						else { PlayerValues[portNumber].PlayerColour -= 4; }
						PlayerValues[portNumber].MenuSwitchCounter = 0.50f;
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_START)) {
					if (inMenu) {
						inOptionsTab = 4; inOptions = true; inMenu = false;
						//setBoardStart();
						//inMenu = false; inGame = true;
						//PlayerValues[portNumber].MenuSwitchCounter = 1.0f;
					}
					if (inOptions) {
						if (inOptionsTab == 4) {
							int OnTeamZero = 0;
							int ActivePlayers = 0;
							for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
								if (PlayerValues[i].PlayerTeam == 0) { OnTeamZero += 1; }
								if (PlayerValues[i].ControllerActive) { ActivePlayers += 1; }
							}
							if (ActivePlayers == 2) { GameObjectsAmount["Players_0"] = 2; }
							else if (ActivePlayers == 4) { GameObjectsAmount["Players_0"] = 4; }

							if (((ActivePlayers == 2) && (OnTeamZero == 1)) || ((ActivePlayers == 4) && (OnTeamZero == 2))) {
								inOptions = false; inGame = true;
								setBoardStart();
								PlayerValues[portNumber].MenuSwitchCounter = 1.0f;
							}

						}
					}
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) {
					if (inOptions) {
						if (inOptionsTab == 4) { inOptions = false; inMenu = true; }
						else if (inOptionsTab != 0) { inOptionsTab = 0; }
						else { inOptions = false; inMenu = true; }
						PlayerValues[portNumber].MenuSwitchCounter = 1.0f;
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

	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
		if (PlayerValues[i].cameraMode == 0) {
			float Tx[2]{ 0.0f,0.0f }; float Ty[2]{ 0.0f,0.0f }; float Tz[2]{ 0.0f,0.0f };

			if (PlayerValues[i].MenuSwitchCounter > 0.0f) { PlayerValues[i].MenuSwitchCounter -= deltaTasSeconds; }
			else {
				if (i == 0) {
					//player One
					if (!PlayerValues[i].FlipedControllers) {
						if (keyDown['w'] || keyDown['W']) { Tx[i] = -0.20f; }
						if (keyDown['s'] || keyDown['S']) { Tx[i] = 00.20f; }
						if (keyDown['a'] || keyDown['A']) { Tz[i] = 00.20f; }
						if (keyDown['d'] || keyDown['D']) { Tz[i] = -0.20f; }
					}
					else {
						if (keyDown['w'] || keyDown['W']) { Tx[i] = 00.20f; }
						if (keyDown['s'] || keyDown['S']) { Tx[i] = -0.20f; }
						if (keyDown['a'] || keyDown['A']) { Tz[i] = -0.20f; }
						if (keyDown['d'] || keyDown['D']) { Tz[i] = 00.20f; }
					}


					if (keyDown['q'] || keyDown['Q']) {
						if (GameObjects["Players_0" + to_string(i)].get()->inAir == false && GameObjects["Players_0" + to_string(i)].get()->IsJumping == false) {
							GameObjects["Players_0" + to_string(i)].get()->inAir = true;
							GameObjects["Players_0" + to_string(i)].get()->IsJumping = true;
							GameObjects["Players_0" + to_string(i)].get()->onObject = false;
							GameObjects["Players_0" + to_string(i)].get()->InAirCounter = 0.25f;
							GameObjects["Players_0" + to_string(i)].get()->setPosition(glm::vec3(GameObjects["Players_0" + to_string(i)].get()->Position().x, GameObjects["Players_0" + to_string(i)].get()->Position().y + (GameObjects["Players_0" + to_string(i)].get()->Radius().y * 0.30f), GameObjects["Players_0" + to_string(i)].get()->Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['e'] || keyDown['E']) && PlayerValues[i].Right_TRIGGERED == false) { PlayerValues[i].Right_TRIGGERED = true; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (PlayerValues[i].Right_TRIGGERED == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['e'] || keyDown['E']) {
							PlayerValues[i].ShockWaveChargeUp += deltaTasSeconds;
							//std::cout << "	[C](" << 0 << ")[" << PShockWaveChargeUp[0] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PlayerValues[i].ShockWaveChargeUp < 0.50f) { PlayerValues[i].ShockWaveChargeUp = 0.0f; }
							PlayerValues[i].Right_TRIGGERED = false;
							PlayerValues[i].ShockWave = true;
							PlayerValues[i].ShockWaveCounter = 0.250f;
							PlayerValues[i].MenuSwitchCounter = 0.60f;
							//std::cout << "[RIGHT_TRIGGERED][+]";
						}
						GameObjects["Players_0" + to_string(i)].get()->setVelocity(GameObjects["Players_0" + to_string(i)].get()->Velocity()*0.5f);
					}
				}
				else if (i == 1) {
					//player Two
					if (!PlayerValues[i].FlipedControllers) {
						if (keyDown['i'] || keyDown['I']) { Tx[i] = 00.20f; }
						if (keyDown['k'] || keyDown['K']) { Tx[i] = -0.20f; }
						if (keyDown['j'] || keyDown['J']) { Tz[i] = -0.20f; }
						if (keyDown['l'] || keyDown['L']) { Tz[i] = 00.20f; }
					}
					else {
						if (keyDown['i'] || keyDown['I']) { Tx[i] = -0.20f; }
						if (keyDown['k'] || keyDown['K']) { Tx[i] = 00.20f; }
						if (keyDown['j'] || keyDown['J']) { Tz[i] = 00.20f; }
						if (keyDown['l'] || keyDown['L']) { Tz[i] = -0.20f; }
					}
					if (keyDown['o'] || keyDown['O']) {
						if (GameObjects["Players_0" + to_string(i)].get()->inAir == false && GameObjects["Players_0" + to_string(i)].get()->IsJumping == false) {
							GameObjects["Players_0" + to_string(i)].get()->inAir = true; GameObjects["Players_0" + to_string(i)].get()->IsJumping = true;
							GameObjects["Players_0" + to_string(i)].get()->onObject = false;
							GameObjects["Players_0" + to_string(i)].get()->InAirCounter = 0.25f;
							GameObjects["Players_0" + to_string(i)].get()->setPosition(glm::vec3(GameObjects["Players_0" + to_string(i)].get()->Position().x, GameObjects["Players_0" + to_string(i)].get()->Position().y + (GameObjects["Players_0" + to_string(i)].get()->Radius().y * 0.30f), GameObjects["Players_0" + to_string(i)].get()->Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['U'] || keyDown['u']) && PlayerValues[i].Right_TRIGGERED == false) { PlayerValues[i].Right_TRIGGERED = true; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (PlayerValues[i].Right_TRIGGERED == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['U'] || keyDown['u']) {
							PlayerValues[i].ShockWaveChargeUp += deltaTasSeconds;
							//std::cout << "	[C](" << 1 << ")[" << PShockWaveChargeUp[1] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PlayerValues[i].ShockWaveChargeUp < 0.50f) { PlayerValues[i].ShockWaveChargeUp = 0.0f; }
							PlayerValues[i].Right_TRIGGERED = false;
							PlayerValues[i].ShockWave = true;
							PlayerValues[i].ShockWaveCounter = 0.250f;
							PlayerValues[i].MenuSwitchCounter = 0.60f;
							//std::cout << "[RIGHT_TRIGGERED][+]";
						}
						GameObjects["Players_0" + to_string(i)].get()->setVelocity(GameObjects["Players_0" + to_string(i)].get()->Velocity()*0.5f);
					}
				}

				
				GameObjects["Players_0" + to_string(i)].get()->setForceOnObject(glm::vec3(Tx[i], Ty[i], Tz[i]));
				GameObjects["Players_0" + to_string(i)].get()->setVelocity(glm::vec3(Tx[i], Ty[i], Tz[i])*PlayerValues[i].SprintSpeed);
				GameObjects["Players_0" + to_string(i)].get()->ForwardDirection = (GameObjects["Players_0" + to_string(i)].get()->Position() - (GameObjects["Players_0" + to_string(i)].get()->Position() + GameObjects["Players_0" + to_string(i)].get()->Velocity()));
			}
		}
		else if (PlayerValues[i].cameraMode == 1) {
			float Tmovement[2]{ 0.0f,0.0f }; float yaw[2]{ 0.0f,0.0f }; float pitch[2]{ 0.0f,0.0f };


			if (PlayerValues[i].MenuSwitchCounter > 0.0f) { PlayerValues[i].MenuSwitchCounter -= deltaTasSeconds; }
			else {
				if (i == 0) {
					//player One //checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
					if (!PlayerValues[i].FlipedControllers) {
						if (keyDown['w'] || keyDown['W']) { Tmovement[i] = 00.50f; }
						if (keyDown['s'] || keyDown['S']) { Tmovement[i] = -0.50f; }
						if (keyDown['a'] || keyDown['A']) { yaw[i] = 00.05f; }
						if (keyDown['d'] || keyDown['D']) { yaw[i] = -0.05f; }
					}
					else {
						if (keyDown['w'] || keyDown['W']) { Tmovement[i] = -0.50f; }
						if (keyDown['s'] || keyDown['S']) { Tmovement[i] = 00.50f; }
						if (keyDown['a'] || keyDown['A']) { yaw[i] = -0.05f; }
						if (keyDown['d'] || keyDown['D']) { yaw[i] = 00.05f; }
					}

					if (keyDown['q'] || keyDown['Q']) {
						if (GameObjects["Players_0" + to_string(i)].get()->inAir == false && GameObjects["Players_0" + to_string(i)].get()->IsJumping == false) {
							GameObjects["Players_0" + to_string(i)].get()->inAir = true; GameObjects["Players_0" + to_string(i)].get()->IsJumping = true;
							GameObjects["Players_0" + to_string(i)].get()->onObject = false;
							GameObjects["Players_0" + to_string(i)].get()->InAirCounter = 0.25f;
							GameObjects["Players_0" + to_string(i)].get()->setPosition(glm::vec3(GameObjects["Players_0" + to_string(i)].get()->Position().x, GameObjects["Players_0" + to_string(i)].get()->Position().y + (GameObjects["Players_0" + to_string(i)].get()->Radius().y * 0.30f), GameObjects["Players_0" + to_string(i)].get()->Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['e'] || keyDown['E']) && PlayerValues[i].Right_TRIGGERED == false) { PlayerValues[i].Right_TRIGGERED = true; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (PlayerValues[i].Right_TRIGGERED == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['e'] || keyDown['E']) {
							PlayerValues[i].ShockWaveChargeUp += deltaTasSeconds;
							//std::cout << "	[C](" << 0 << ")[" << PShockWaveChargeUp[0] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PlayerValues[i].ShockWaveChargeUp < 0.50f) { PlayerValues[i].ShockWaveChargeUp = 0.0f; }
							PlayerValues[i].Right_TRIGGERED = false;
							PlayerValues[i].ShockWave = true;
							PlayerValues[i].ShockWaveCounter = 0.250f;
							PlayerValues[i].MenuSwitchCounter = 0.60f;
							//std::cout << "[RIGHT_TRIGGERED][+]";
						}
						GameObjects["Players_0" + to_string(i)].get()->setVelocity(GameObjects["Players_0" + to_string(i)].get()->Velocity()*0.5f);
					}
				}
				if (i == 1) {
					//player Two
					if (!PlayerValues[i].FlipedControllers) {
						if (keyDown['i'] || keyDown['I']) { Tmovement[i] = 00.50f; }
						if (keyDown['k'] || keyDown['K']) { Tmovement[i] = -0.50f; }
						if (keyDown['j'] || keyDown['J']) { yaw[i] = 00.05f; }
						if (keyDown['l'] || keyDown['L']) { yaw[i] = -0.05f; }
					}
					else {
						if (keyDown['i'] || keyDown['I']) { Tmovement[i] = -0.50f; }
						if (keyDown['k'] || keyDown['K']) { Tmovement[i] = 00.50f; }
						if (keyDown['j'] || keyDown['J']) { yaw[i] = -0.05f; }
						if (keyDown['l'] || keyDown['L']) { yaw[i] = 00.05f; }
					}
					if (keyDown['o'] || keyDown['O']) {
						if (GameObjects["Players_0" + to_string(i)].get()->inAir == false && GameObjects["Players_0" + to_string(i)].get()->IsJumping == false) {
							GameObjects["Players_0" + to_string(i)].get()->inAir = true; GameObjects["Players_0" + to_string(i)].get()->IsJumping = true;
							GameObjects["Players_0" + to_string(i)].get()->onObject = false;
							GameObjects["Players_0" + to_string(i)].get()->InAirCounter = 0.25f;
							GameObjects["Players_0" + to_string(i)].get()->setPosition(glm::vec3(GameObjects["Players_0" + to_string(i)].get()->Position().x, GameObjects["Players_0" + to_string(i)].get()->Position().y + (GameObjects["Players_0" + to_string(i)].get()->Radius().y * 0.30f), GameObjects["Players_0" + to_string(i)].get()->Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['U'] || keyDown['u']) && PlayerValues[i].Right_TRIGGERED == false) { PlayerValues[i].Right_TRIGGERED = true; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (PlayerValues[i].Right_TRIGGERED == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['U'] || keyDown['u']) {
							PlayerValues[i].ShockWaveChargeUp += deltaTasSeconds;
							//std::cout << "	[C](" << 1 << ")[" << PShockWaveChargeUp[1] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PlayerValues[i].ShockWaveChargeUp < 0.50f) { PlayerValues[i].ShockWaveChargeUp = 0.0f; }
							PlayerValues[i].Right_TRIGGERED = false;
							PlayerValues[i].ShockWave = true;
							PlayerValues[i].ShockWaveCounter = 0.250f;
							PlayerValues[i].MenuSwitchCounter = 0.60f;
							//std::cout << "[RIGHT_TRIGGERED][+]";
						}
						GameObjects["Players_0" + to_string(i)].get()->setVelocity(GameObjects["Players_0" + to_string(i)].get()->Velocity()*0.5f);
					}
				}
			}

			//yaw
			PlayerValues[i].rightVector = glm::cross(PlayerValues[i].forwardVector, glm::vec3(0.0f, 1.0f, 0.0f));
			PlayerValues[i].rightVector = glm::normalize(PlayerValues[i].rightVector);
			PlayerValues[i].forwardVector = glm::rotate(PlayerValues[i].forwardVector, yaw[i], glm::vec3(0.0f, 1.0f, 0.0f));
			//pitch
			PlayerValues[i].rightVector = glm::cross(PlayerValues[i].forwardVector, glm::vec3(0.0f, 1.0f, 0.0f));
			PlayerValues[i].rightVector = glm::normalize(PlayerValues[i].rightVector);
			PlayerValues[i].forwardVector = glm::rotate(PlayerValues[i].forwardVector, pitch[i], PlayerValues[i].rightVector);
			//player position
			glm::vec3 forwardVectorTemp = glm::vec3(PlayerValues[i].forwardVector.x, 0.0f, PlayerValues[i].forwardVector.z);
			GameObjects["Players_0" + to_string(i)].get()->ForwardDirection = (forwardVectorTemp * -0.001f);
			GameObjects["Players_0" + to_string(i)].get()->setForceOnObject((Tmovement[i] * forwardVectorTemp)*0.5f);
			GameObjects["Players_0" + to_string(i)].get()->setVelocity((Tmovement[i] * forwardVectorTemp)*0.5f*PlayerValues[i].SprintSpeed);
			//camera position
			PlayerValues[i].cameraPosition = glm::vec3(GameObjects["Players_0" + to_string(i)].get()->Position().x - (PlayerValues[i].forwardVector.x*6.0f),
				GameObjects["Players_0" + to_string(i)].get()->Position().y + (GameObjects["Players_0" + to_string(i)].get()->Radius().y*2.0f),
				GameObjects["Players_0" + to_string(i)].get()->Position().z - (PlayerValues[i].forwardVector.z*6.0f));
			//shockwace position
		}
	}
}



//////////////////////////////////////////////////////////////////////
void SendUniformsToShaders(std::string SendMe, int Inum) {
	auto _Material = materials[SendMe];
	//_Material->shader->bind();
	_Material->mat4Uniforms["u_mvp"] = cameras[Inum].viewProjMatrix;
	_Material->mat4Uniforms["u_mv"] = cameras[Inum].viewMatrix;
	_Material->mat4Uniforms["u_mp"] = cameras[Inum].projMatrix;
	_Material->mat4Uniforms["u_lightPos"] = (cameras[Inum].viewMatrix * glm::translate(glm::mat4(1.0f), lightPosition));
	_Material->sendUniforms();
}

void setAllObjectMaterials(std::string Name) {
	auto _Material = materials[Name];
	for (auto itr = GameObjects.begin(); itr != GameObjects.end(); itr++) {
		itr->second->material = _Material;
	}
}

void DepthPass(std::string FirstfBO, std::string  SecondfBO, int ScreenNumber) {
	auto _FirstfBO = FBOs[FirstfBO];
	auto _SecondfBO = FBOs[SecondfBO];
	
	_SecondfBO->clearFrameBuffer(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	_SecondfBO->bindFrameBufferForDrawing();

	_FirstfBO->bindTextureForSampling(0, GL_TEXTURE1);
	SendUniformsToShaders("Depth", ScreenNumber);
}


/* function InMenuDraw()
* Description:
*  - Draws the menu screen
*/
void InMenuDraw(int Inum)
{
	Inum = 2;
	cameralook = Inum; //window
	WhatCameraIsLookingAt(); //Resising Window
	SendUniformsToShaders("passThrough", Inum);

	
	if (MenuObjects["HUD_Planes_0" + to_string(0)]->Viewable) { MenuObjects["HUD_Planes_0" + to_string(0)]->drawObject(); }

	for (unsigned int i = 0; i <= 2; i++) {
		if (MenuObjects["HUD_Buttons_0" + to_string(i)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(i)]->drawObject(); }
	}


}

/* function MenuScreen()
* Description:
*  - does all the functions/calculations for the menu screen
*/
void MenuScreen(float deltaTasSeconds)
{
	Sound::Sys.Update();

	FMOD_VECTOR drumPos; drumPos.x = 0; drumPos.y = 0; drumPos.z = 0;
	FMOD_VECTOR drumVel; drumVel.x = 12.0f; drumVel.y = 0.0f; drumVel.z = 0.0f;

	powerup[5].SetPosition(powChannel[0], drumPos, drumVel);
	Fx[0].SetPosition(FxChannel[0], drumPos, drumVel);
	Fx[1].SetPosition(FxChannel[1], drumPos, drumVel);

	if (mouseDown[0]) {
		mouseDown[0] = false;
		if (Button[0].button(MPosToOPosX, MPosToOPosY)) { inOptionsTab = 4; inOptions = true; inMenu = false; }//setBoardStart(); inGame = true;
		if (Button[1].button(MPosToOPosX, MPosToOPosY)) { inOptionsTab = 0; inOptions = true; inMenu = false; }
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
	Inum = 2;
	cameralook = Inum; //window
	WhatCameraIsLookingAt(); //Resising Window

	// Draw our scene
	SendUniformsToShaders("passThrough", Inum);

	//for (auto itr = MenuObjects.begin(); itr != MenuObjects.end(); ++itr) {
	//	auto MenuObjects = itr->second;
	//	MenuObjects->drawObject();
	//}


	if (inOptionsTab == 0) {
		if (MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->Viewable) { MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->drawObject(); }
		if (MenuObjects["HUD_Buttons_0" + to_string(3)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(3)]->drawObject(); }
		for (unsigned int i = 4; i <= 6; i++) {
			if (MenuObjects["HUD_Buttons_0" + to_string(i)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(i)]->drawObject(); }
		}
	}
	else if (inOptionsTab == 1) {
		if (MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->Viewable) { MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->drawObject(); }
		if (MenuObjects["HUD_Buttons_0" + to_string(3)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(3)]->drawObject(); }
	}
	else if (inOptionsTab == 2) {
		if (MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->Viewable) { MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->drawObject(); }
		if (MenuObjects["HUD_Buttons_0" + to_string(3)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(3)]->drawObject(); }
	}
	else if (inOptionsTab == 3) {
		if (MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->Viewable) { MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->drawObject(); }
		if (MenuObjects["HUD_Buttons_0" + to_string(3)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(3)]->drawObject(); }
		for (int i = 0; i < MenuObjectsAmount["HUD_Sliders_0"]; i++) {
			if (MenuObjects["HUD_Slider_Button_0" + to_string(i)]->Viewable) { MenuObjects["HUD_Slider_Button_0" + to_string(i)]->drawObject(); }
			if (MenuObjects["HUD_Slider_Bar_0" + to_string(i)]->Viewable) { MenuObjects["HUD_Slider_Bar_0" + to_string(i)]->drawObject(); }
		}
	}
	//Colour select
	else if (inOptionsTab == 4) {
		if (MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->Viewable) { MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->drawObject(); }
		if (MenuObjects["HUD_Buttons_0" + to_string(3)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(3)]->drawObject(); }
	

		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
			if (PlayerValues[i].ControllerActive) {
				MenuObjects["HUD_Buttons_0" + to_string(7 + i)].get()->setTexture(PlayerSelectColour[PlayerValues[i].PlayerColour]);

				if (PlayerValues[i].PlayerColour % 2 == 0) { PlayerValues[i].PlayerTeam = 0; }
				else { PlayerValues[i].PlayerTeam = 1; }
				PlayerValues[i].PlayerTexture = PlayerTextures[PlayerValues[i].PlayerColour];
				GameObjects["Players_0" + to_string(i)].get()->setTexture(PlayerTextures[PlayerValues[i].PlayerColour]);
				GameObjects["Shockwave_0" + to_string(i)].get()->setTexture(PlayerTextures[PlayerValues[i].PlayerColour+6]);
			}
			else { MenuObjects["HUD_Buttons_0" + to_string(7 + i)].get()->setTexture(PlayerSelectColour[6]); }
		}
		for (unsigned int i = 7; i <= 10; i++) {
			if (MenuObjects["HUD_Buttons_0" + to_string(i)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(i)]->drawObject(); }
		}
	}
	//end screen
	else if (inOptionsTab == 5) {
		if (MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->Viewable) { MenuObjects["HUD_Planes_0" + to_string(inOptionsTab + 1)]->drawObject(); }
		if (MenuObjects["HUD_Buttons_0" + to_string(3)]->Viewable) { MenuObjects["HUD_Buttons_0" + to_string(3)]->drawObject(); }

		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
			if (PlayerValues[i].ControllerActive) {}
			else {}

			if (PlayerValues[i].PlayerTeam == 0) {
				GameObjects["Players_0" + to_string(0)].get()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
				GameObjects["Players_0" + to_string(0)].get()->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			}
			else if (PlayerValues[i].PlayerTeam == 1) {
				GameObjects["Players_0" + to_string(1)].get()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
				GameObjects["Players_0" + to_string(1)].get()->setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
			}

			GameObjects["Players_0" + to_string(i)].get()->drawObject();
		}
	}

}

/* function OptionScreen()
* Description:
*  - does all the functions/calculations for the options screen
*/
void OptionScreen(float deltaTasSeconds)
{
	Sound::Sys.Update();

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
			for (int i = 0; i < MenuObjectsAmount["HUD_Sliders_0"]; i++) {
				//move nob along the slider
				if (Slider[i].moveNob(MPosToOPosX, MPosToOPosY)) { MenuObjects["HUD_Slider_Button_0" + to_string(i)]->setPosition(glm::vec3(MPosToOPosX, 0.02f, Slider[i].SBar_Pos.z)); }
			}
		}
		//Player select screen
		else if (inOptionsTab == 4) {
			//Back
			if (Button[3].button(MPosToOPosX, MPosToOPosY)) { inOptions = false; inMenu = true; }

			for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
				if (PlayerValues[i].ControllerActive) { }
				if (ControllerAPress[i] == true) { ControllerAPress[i] = false; }
			}
			//setBoardStart(); inOptions = false; inGame = true;
		}
		//end screen
		else if (inOptionsTab == 5) {
			//Back
			if (Button[3].button(MPosToOPosX, MPosToOPosY)) { inOptions = false; inMenu = true; }

			for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
				if (PlayerValues[i].ControllerActive) {}
				if (ControllerAPress[i] == true) { ControllerAPress[i] = false; }
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
		for (int i = GameObjectsAmount["Enemies_0"] -1; i > (GameObjectsAmount["Enemies_0"] - static_cast<int>(Slider[1].SNob_Precent.x / 100)); i--) {
			GameObjects["Enemies_0" + to_string(i)].get()->Viewable = false;
		}
		PlayerHp[0].CurrentHealth = PlayerHp[1].CurrentHealth = static_cast<int>(Slider[2].SNob_Precent.x);
	}
	else if (inOptionsTab == 4) {
	}
}

/* function InGameDraw()
* Description:
*  - Draws the game screen
*/
void InGameDraw(int Inum)
{
	cameralook = Inum;
	WhatCameraIsLookingAt(Inum);

	//setAllObjectMaterials("Depth");
	//Draw scene //NEEDS TO BE IN THIS ORDER for particles to work
	SendUniformsToShaders("passThrough", Inum);	 //1
	SendUniformsToShaders("NASD", Inum);		 //2
	SendUniformsToShaders("Depth", Inum);		 //3
	SendUniformsToShaders("particles", Inum);	 //4
	materials["passThrough"]->shader->bind();	 //then bind passThrough
	

	
	for (int i = 0; i < GameObjectsAmount["Objects_0"]; i++) {
		if (GameObjects["Objects_0" + to_string(i)].get()->Viewable) {
			if (GameObjects["Objects_0" + to_string(i)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			GameObjects["Objects_0" + to_string(i)].get()->drawObject();
		}
	}
	for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
		if (GameObjects["Enemies_0" + to_string(i)].get()->Viewable) {
			//Enemies
			if (GameObjects["Enemies_0" + to_string(i)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			GameObjects["Enemies_0" + to_string(i)].get()->drawObject();
			//Shadows
			if (EnableShadows) {
				if (GameObjects["Shadows_0" + to_string(0)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				GameObjects["Shadows_0" + to_string(0)].get()->setPosition(glm::vec3(GameObjects["Enemies_0" + to_string(i)].get()->Position().x, 0.04f, GameObjects["Enemies_0" + to_string(i)].get()->Position().z));
				if ((GameObjects["Enemies_0" + to_string(i)].get()->Position().y*0.05f) > 1.50f) { GameObjects["Shadows_0" + to_string(0)].get()->setScale(GameObjects["Enemies_0" + to_string(i)].get()->Scale()*1.50f); }
				else if ((GameObjects["Enemies_0" + to_string(i)].get()->Position().y*0.05f) < 1.0f) { GameObjects["Shadows_0" + to_string(0)].get()->setScale(GameObjects["Enemies_0" + to_string(i)].get()->Scale()*1.0f); }
				else { GameObjects["Shadows_0" + to_string(0)].get()->setScale(GameObjects["Enemies_0" + to_string(i)].get()->Scale()*(GameObjects["Enemies_0" + to_string(i)].get()->Position().y*0.05f)); }
				GameObjects["Shadows_0" + to_string(0)].get()->drawObject();
			}
		}
	}
	for (int i = 0; i < GameObjectsAmount["Specials_0"]; i++) {
		if (GameObjects["Specials_0" + to_string(i)].get()->Viewable) {
			//Specials
			if (GameObjects["Specials_0" + to_string(i)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			GameObjects["Specials_0" + to_string(i)].get()->drawObject();
			//Shadows
			if (EnableShadows) {
				if (GameObjects["Shadows_0" + to_string(0)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				GameObjects["Shadows_0" + to_string(0)].get()->setPosition(glm::vec3(GameObjects["Specials_0" + to_string(i)].get()->Position().x, 0.06f, GameObjects["Specials_0" + to_string(i)].get()->Position().z));
				GameObjects["Shadows_0" + to_string(0)].get()->setScale(GameObjects["Specials_0" + to_string(i)].get()->Scale());
				GameObjects["Shadows_0" + to_string(0)].get()->setRotation(GameObjects["Specials_0" + to_string(i)].get()->Angle());
				GameObjects["Shadows_0" + to_string(0)].get()->drawObject();
			}
		}
	}
	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
		if (GameObjects["Players_0" + to_string(i)].get()->Viewable) {
			//Players
			if (GameObjects["Players_0" + to_string(i)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			GameObjects["Players_0" + to_string(i)].get()->drawObject();
			//Shadows
			if (EnableShadows) {
				if (GameObjects["Shadows_0" + to_string(0)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				GameObjects["Shadows_0" + to_string(0)].get()->setPosition(glm::vec3(GameObjects["Players_0" + to_string(i)].get()->Position().x, 0.05f, GameObjects["Players_0" + to_string(i)].get()->Position().z));
				GameObjects["Shadows_0" + to_string(0)].get()->setScale(GameObjects["Players_0" + to_string(i)].get()->Scale());
				GameObjects["Shadows_0" + to_string(0)].get()->setRotation(GameObjects["Players_0" + to_string(i)].get()->Angle());
				GameObjects["Shadows_0" + to_string(0)].get()->drawObject();
			}
		}
	}
	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
		//Shockwave
		glDepthMask(GL_FALSE);
		if (GameObjects["Shockwave_0" + to_string(i)].get()->Viewable) {
			if (GameObjects["Shockwave_0" + to_string(i)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			GameObjects["Shockwave_0" + to_string(i)].get()->drawObject();
		}
		glDepthMask(GL_TRUE);
		//Short Circuit [6]
		if (GameObjects["Players_0" + to_string(i)].get()->inShock) {
			GameObjects["Affects_0" + to_string(1)].get()->setPosition(GameObjects["Players_0" + to_string(i)].get()->Position());
			GameObjects["Affects_0" + to_string(2)].get()->setPosition(GameObjects["Players_0" + to_string(i)].get()->Position());
			GameObjects["Affects_0" + to_string(1)].get()->Viewable = true;
			GameObjects["Affects_0" + to_string(2)].get()->Viewable = true;
			GameObjects["Affects_0" + to_string(1)].get()->setRotation(glm::vec3(0.0f, GameObjects["Affects_0" + to_string(1)].get()->Angle().y - (deltaTasSecs*316.0f), 0.0f));
			GameObjects["Affects_0" + to_string(2)].get()->setRotation(glm::vec3(0.0f, GameObjects["Affects_0" + to_string(2)].get()->Angle().y + (deltaTasSecs*147.0f), 0.0f));
		}
		else if (!GameObjects["Players_0" + to_string(i)].get()->inShock) {
			GameObjects["Affects_0" + to_string(1)].get()->Viewable = false;
			GameObjects["Affects_0" + to_string(2)].get()->Viewable = false;
		}
		//Flipped [9]
		if (PlayerValues[i].FlipedControllers) {
			PlayerValues[i].AbilityCounter[9] += deltaTasSecs;
			GameObjects["Affects_0" + to_string(3)].get()->setPosition(GameObjects["Players_0" + to_string(i)].get()->Position());
			GameObjects["Affects_0" + to_string(4)].get()->setPosition(GameObjects["Players_0" + to_string(i)].get()->Position());
			GameObjects["Affects_0" + to_string(3)].get()->Viewable = true;
			GameObjects["Affects_0" + to_string(4)].get()->Viewable = true;
			GameObjects["Affects_0" + to_string(3)].get()->setRotation(glm::vec3(0.0f, GameObjects["Affects_0" + to_string(3)].get()->Angle().y + (deltaTasSecs*147.0f), 0.0f));
			GameObjects["Affects_0" + to_string(4)].get()->setRotation(glm::vec3(0.0f, GameObjects["Affects_0" + to_string(4)].get()->Angle().y - (deltaTasSecs*316.0f), 0.0f));
			if (PlayerValues[i].AbilityCounter[9] > PlayerValues[i].AbilityLength[9]) {
				PlayerValues[i].AbilityCounter[9] = 0.0f;
				PlayerValues[i].FlipedControllers = false;
				GameObjects["Affects_0" + to_string(3)].get()->Viewable = false;
				GameObjects["Affects_0" + to_string(4)].get()->Viewable = false;
			}
		}
		//Affects
		for (int j = 0; j < GameObjectsAmount["Affects_0"]; j++) {
			if (GameObjects["Affects_0" + to_string(j)].get()->Viewable) {
				if (GameObjects["Affects_0" + to_string(j)].get()->textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				GameObjects["Affects_0" + to_string(j)].get()->drawObject();
			}
		}
	}
	for (int i = 0; i < GameObjectsAmount["Rifts_0"]; i++) {
		if (GameObjects["Rifts_0" + to_string(i)]->Viewable) {
			//Rifts
			if (GameObjects["Rifts_0" + to_string(i)]->textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			GameObjects["Rifts_0" + to_string(i)]->drawObject();
		}
	}
	for (int i = 0; i < GameObjectsAmount["HUD_Score_Planes_0"]; i++) {
		if (GameObjects["HUD_Score_Planes_0" + to_string(i)]->Viewable) {
			if (GameObjects["HUD_Score_Planes_0" + to_string(i)]->textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			GameObjects["HUD_Score_Planes_0" + to_string(i)]->drawObject();
		}
	}
	

	GameObjects["Objects_09"].get()->material = materials["Depth"]; //bind an object that is at 0.0.0 to the depth material
	
	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
		if (PlayerParticleEmitter[i].Viewable) {
			PlayerParticleEmitter[i].initialPosition = GameObjects["Players_0" + to_string(i)].get()->Position();
			PlayerParticleEmitter[i].update(deltaTasSecs / 1.0f);
			PlayerParticleEmitter[i].draw(&cameras[Inum]);
			//PlayerParticleEmitter[i].Viewable = false;
		}
	}
	for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
		if (EnemyParticleEmitter[i].Viewable) {
			EnemyParticleEmitter[i].initialPosition = GameObjects["Enemies_0" + to_string(i)].get()->Position();
			EnemyParticleEmitter[i].update(deltaTasSecs / 1.0f);
			EnemyParticleEmitter[i].draw(&cameras[Inum]);
			//EnemyParticleEmitter[i].Viewable = false;
		}
	}


	myVBO.vbo.draw();

	//for (auto itr = GameObjects.begin(); itr != GameObjects.end(); itr++) {
	//	auto GameObjects = itr->second;
	//	if (GameObjects->Viewable) {
	//		if (GameObjects->textureHandle_hasTransparency == true) { disableCulling(); }
	//		else { enableCulling(); }
	//		GameObjects->drawObject();
	//	}
	//}
}

float lastHit = 0.0f;
/* function GameField()
* Description:
*  - does all the functions/calculations for the game screen
*/
void GameScreen(float deltaTasSeconds) 
{
	Sound::Sys.Update();
	//Intialize the crowd sound to play when the game starts
	FxChannel[1] = Fx[1].Play();
	FxChannel[1]->setVolume(0.3f);

	for (int i = 0; i < 2; i++) {
		Sound::Sys.listenerPos[i].x = GameObjects["Players_0" + to_string(i)].get()->Position().x*10.0f;
		Sound::Sys.listenerPos[i].y = GameObjects["Players_0" + to_string(i)].get()->Position().y*10.0f;
		Sound::Sys.listenerPos[i].z = GameObjects["Players_0" + to_string(i)].get()->Position().z*10.0f;
	}
	
	if (lastHit > 0.0f) { lastHit -= deltaTasSeconds; }


	//Collision between things
	if (CollisionBetweenObjects) {
		//std::cout << "[Hit][" << j << "]" << std::endl;

		//Specials to Borders
		for (int i = 0; i < GameObjectsAmount["Specials_0"]; i++) {
			m.A = GameObjects["Specials_0" + to_string(i)];
			bool InBorder = false;

			//check to see if player is in a bounding box
			for (int j = 0; j < GameObjectsAmount["Borders_0"]; j++) {
				m.B = GameObjects["Borders_0" + to_string(j)];
				if (ObjectInBox(m)) { InBorder = true; }
			}

			if (InBorder) {}
			//if the player is not in any box, seek to nearest one
			else {
				float distanceToNearestBox; int boxNumber_1 = 0; glm::vec3 boxWithLargerRadius(0.0f); int boxNumber_2 = 0;
				distanceToNearestBox = glm::distance(m.A->Position(), GameObjects["Borders_0" + to_string(0)].get()->Position());
				//find nearest bounding box
				for (int j = 0; j < GameObjectsAmount["Borders_0"]; j++) {
					m.B = GameObjects["Borders_0" + to_string(j)];
					glm::vec3 distNearPos = glm::vec3(glm::distance(m.A->Position().x, m.B->Position().x), glm::distance(m.A->Position().y, m.B->Position().y), glm::distance(m.A->Position().z, m.B->Position().z));
					if ((distNearPos.x < m.B->Radius().x) && (distNearPos.y < m.B->Radius().y) && (distNearPos.z < m.B->Radius().z)) {
						//box with larger radius
						if (boxWithLargerRadius.x < m.B->Radius().x && boxWithLargerRadius.y < m.B->Radius().y && boxWithLargerRadius.z < m.B->Radius().z) {
							boxWithLargerRadius = m.B->Radius();
							boxNumber_2 = j;
						}
						//closest box
						if (distanceToNearestBox > glm::distance(m.A->Position(), m.B->Position())) {
							distanceToNearestBox = glm::distance(m.A->Position(), m.B->Position());
							boxNumber_1 = j;
						}
					}
				}
				//switch the m.A & m.B cause only m.B is affected
				m.B = GameObjects["Borders_0" + to_string(boxNumber_2)];
				if (CheckCollision(m)) { m.A = GameObjects["Borders_0" + to_string(boxNumber_2)]; m.B = GameObjects["Specials_0" + to_string(i)]; }
				else { m.A = GameObjects["Borders_0" + to_string(boxNumber_1)]; m.B = GameObjects["Specials_0" + to_string(i)]; }


				//determine the wall collision
				if (m.B->Position().x + (m.B->Radius().x*0.5f) > m.A->Top().x) {
					m.B->setPosition(glm::vec3(m.A->Top().x - (m.B->Radius().x*0.5f), m.B->Position().y, m.B->Position().z));
					m.B->setVelocity(glm::vec3(-(m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().x - (m.B->Radius().x*0.5f) < m.A->Bottom().x) {
					m.B->setPosition(glm::vec3(m.A->Bottom().x + (m.B->Radius().x*0.5f), m.B->Position().y, m.B->Position().z));
					m.B->setVelocity(glm::vec3(-(m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B->Position().y + (m.B->Radius().y*0.5f) > m.A->Top().y) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.A->Top().y - (m.B->Radius().y*0.5f), m.B->Position().z));
					m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), -(m.B->Velocity().y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().y - (m.B->Radius().y*0.5f) < m.A->Bottom().y) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.A->Bottom().y + (m.B->Radius().y*0.5f), m.B->Position().z));
					m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), -(m.B->Velocity().y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B->Position().z + (m.B->Radius().z*0.5f) > m.A->Top().z) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.B->Position().y, m.A->Top().z - (m.B->Radius().z*0.5f)));
					m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), -(m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().z - (m.B->Radius().z*0.5f) < m.A->Bottom().z) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.B->Position().y, m.A->Bottom().z + (m.B->Radius().z*0.5f)));
					m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), -(m.B->Velocity().z*speedToWallDegradation.z)));
				}
				////If Specials are nearing the walls
				//if (t_PPos.x > (t_BTop.x*0.95)) { FleeFromDirection(m, 1.0f, "x"); }
				//else if (t_PPos.x < (t_BBot.x*0.95)) { FleeFromDirection(m, 1.0f, "-x"); }
				//if (t_PPos.z > (t_BTop.z*0.95)) { FleeFromDirection(m, 1.0f, "z"); }
				//else if (t_PPos.z < (t_BBot.z*0.95)) { FleeFromDirection(m, 1.0f, "-z"); }
				GameObjects["Specials_0" + to_string(i)] = m.B;
			}
		}
		//Specials to Objects
		for (int i = 0; i < GameObjectsAmount["Specials_0"]; i++) {
			m.B = GameObjects["Specials_0" + to_string(i)];
			for (int j = 1; j < GameObjectsAmount["Objects_0"]; j++) {
				m.A = GameObjects["Objects_0" + to_string(j)];
				if (checkRadialCollision(m)) { ResolveCollision(m, 0.1f); }
				GameObjects["Objects_0" + to_string(j)] = m.A;
			}//end for
			GameObjects["Specials_0" + to_string(i)] = m.B;
		}//end for

		 //Players to Borders
		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
			m.A = GameObjects["Players_0" + to_string(i)];
			bool InBorder = false;

			//check to see if player is in a bounding box
			for (int j = 0; j < GameObjectsAmount["Borders_0"]; j++) {
				m.B = GameObjects["Borders_0" + to_string(j)];
				if (ObjectInBox(m)) { InBorder = true; }
			}

			if (InBorder) { }
			//if the player is not in any box, seek to nearest one
			else {
				float distanceToNearestBox; int boxNumber_1 = 0; glm::vec3 boxWithLargerRadius(0.0f); int boxNumber_2 = 0;
				distanceToNearestBox = glm::distance(m.A->Position(), GameObjects["Borders_0" + to_string(0)].get()->Position());
				//find nearest bounding box
				for (int j = 0; j < GameObjectsAmount["Borders_0"]; j++) {
					m.B = GameObjects["Borders_0" + to_string(j)];
					glm::vec3 distNearPos = glm::vec3(glm::distance(m.A->Position().x, m.B->Position().x), glm::distance(m.A->Position().y, m.B->Position().y), glm::distance(m.A->Position().z, m.B->Position().z));
					if ((distNearPos.x < m.B->Radius().x) && (distNearPos.y < m.B->Radius().y) && (distNearPos.z < m.B->Radius().z)) {
						//box with larger radius
						if (boxWithLargerRadius.x < m.B->Radius().x && boxWithLargerRadius.y < m.B->Radius().y && boxWithLargerRadius.z < m.B->Radius().z) {
							boxWithLargerRadius = m.B->Radius();
							boxNumber_2 = j;
						}
						//closest box
						if (distanceToNearestBox > glm::distance(m.A->Position(), m.B->Position())) {
							distanceToNearestBox = glm::distance(m.A->Position(), m.B->Position());
							boxNumber_1 = j;
						}
					}
				}
				
				//switch the m.A & m.B cause only m.B is affected
				if (boxNumber_1 != boxNumber_2) {
					m.B = GameObjects["Borders_0" + to_string(boxNumber_2)];
					if (CheckIfObjectInBorderOfBox(m)) { 
						m.B = GameObjects["Borders_0" + to_string(boxNumber_1)];
						if (CheckIfObjectInBorderOfBox(m)) {

							glm::vec3 pointTopL(GameObjects["Borders_0" + to_string(boxNumber_2)].get()->Position().x - m.A->Radius().x, m.A->Position().y, GameObjects["Borders_0" + to_string(boxNumber_1)].get()->Position().z);
							glm::vec3 pointTopR(GameObjects["Borders_0" + to_string(boxNumber_2)].get()->Position().x, m.A->Position().y, GameObjects["Borders_0" + to_string(boxNumber_1)].get()->Position().z);
							glm::vec3 pointBotL(GameObjects["Borders_0" + to_string(boxNumber_2)].get()->Position().x - m.A->Radius().x, m.A->Position().y, GameObjects["Borders_0" + to_string(boxNumber_1)].get()->Position().z - m.A->Radius().z);
							glm::vec3 pointBotR(GameObjects["Borders_0" + to_string(boxNumber_2)].get()->Position().x, m.A->Position().y, GameObjects["Borders_0" + to_string(boxNumber_1)].get()->Position().z - m.A->Radius().z);



							m.A = GameObjects["Borders_0" + to_string(boxNumber_2)]; m.B = GameObjects["Players_0" + to_string(i)]; /*std::cout << "[Double border collid to bigger box] \n";*/ 
						}
						else {
							if (CheckCollision(m)) { m.A = GameObjects["Borders_0" + to_string(boxNumber_1)]; m.B = GameObjects["Players_0" + to_string(i)]; /*std::cout << "[In border collid to nearest box] \n";*/ }
							else { m.A = GameObjects["Borders_0" + to_string(boxNumber_2)]; m.B = GameObjects["Players_0" + to_string(i)]; /*std::cout << "[In border collid to bigger box] \n";*/ }
						}
					}
					else if (CheckCollision(m)) { m.A = GameObjects["Borders_0" + to_string(boxNumber_2)]; m.B = GameObjects["Players_0" + to_string(i)]; /*std::cout << "[Collid to bigger box] \n";*/ }
					else { m.A = GameObjects["Borders_0" + to_string(boxNumber_1)]; m.B = GameObjects["Players_0" + to_string(i)]; /*std::cout << "[Collid to nearest box] \n";*/ }
				}
				else { m.A = GameObjects["Borders_0" + to_string(boxNumber_1)]; m.B = GameObjects["Players_0" + to_string(i)]; /*std::cout << "[Collid to only box] \n";*/ }

				


				//determine the wall collision
				if (m.B->Position().x + (m.B->Radius().x*0.5f) > m.A->Top().x) {
					m.B->setPosition(glm::vec3(m.A->Top().x - (m.B->Radius().x*0.5f), m.B->Position().y, m.B->Position().z));
					//m.B->setVelocity(glm::vec3(-(m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().x - (m.B->Radius().x*0.5f) < m.A->Bottom().x) {
					m.B->setPosition(glm::vec3(m.A->Bottom().x + (m.B->Radius().x*0.5f), m.B->Position().y, m.B->Position().z));
					//m.B->setVelocity(glm::vec3(-(m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B->Position().y + (m.B->Radius().y*0.5f) > m.A->Top().y) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.A->Top().y - (m.B->Radius().y*0.5f), m.B->Position().z));
					//m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), -(m.B->Velocity().y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().y - (m.B->Radius().y*0.5f) < m.A->Bottom().y) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.A->Bottom().y + (m.B->Radius().y*0.5f), m.B->Position().z));
					//m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), -(m.B->Velocity().y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B->Position().z + (m.B->Radius().z*0.5f) > m.A->Top().z) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.B->Position().y, m.A->Top().z - (m.B->Radius().z*0.5f)));
					//m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), -(m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().z - (m.B->Radius().z*0.5f) < m.A->Bottom().z) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.B->Position().y, m.A->Bottom().z + (m.B->Radius().z*0.5f)));
					//m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), -(m.B->Velocity().z*speedToWallDegradation.z)));
				}


				GameObjects["Players_0" + to_string(i)] = m.B;
			}
		}
		 //Players to Specials
		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
			m.A = GameObjects["Players_0" + to_string(i)];
			for (int j = 0; j < GameObjectsAmount["Specials_0"]; j++) {
				m.B = GameObjects["Specials_0" + to_string(j)];
				if (checkRadialCollision(m)) {
					if (m.B->SpecialAttribute() == 0) {}
					//Seeker Swarm
					else if (m.B->SpecialAttribute() == 1) {
						powChannel[0] = powerup[0].Play();
						m.B->Viewable = false;
						PlayerValues[i].AbilityAffected[1] = true;
						PlayerValues[i].AbilityCounter[1] = 0.0f;
					}
					//Toss-Up
					else if (m.B->SpecialAttribute() == 2) {
						powChannel[0] = powerup[1].Play();
						m.B->Viewable = false;
						for (int ij = 0; ij < GameObjectsAmount["Enemies_0"]; ij++) {
							float ranPosY = static_cast<float>(rand() % 1000 + 100); //100 to 1100
							GameObjects["Enemies_0" + to_string(ij)].get()->setForceOnObject(GameObjects["Enemies_0" + to_string(ij)].get()->ForceOnObject() + glm::vec3(0.0f, ranPosY, 0.0f));
						}

						PlayerValues[i].AbilityAffected[2] = true;
						PlayerValues[i].AbilityCounter[2] = 0.0f;
					}
					//Health Up
					else if (m.B->SpecialAttribute() == 3) {
						powChannel[0] = powerup[2].Play();
						m.B->Viewable = false;
						PlayerHp[PlayerValues[i].PlayerTeam].CurrentHealth += 10;
						PlayerValues[i].AbilityAffected[3] = true;
						PlayerValues[i].AbilityCounter[3] = 0.0f;
					}
					//Boost
					else if (m.B->SpecialAttribute() == 4) {
						powChannel[0] = powerup[3].Play();
						m.B->Viewable = false;
						PlayerValues[i].AbilityAffected[4] = true;
						PlayerValues[i].AbilityCounter[4] = 0.0f;
					}
					//Flee
					else if (m.B->SpecialAttribute() == 5) {
						powChannel[0] = powerup[4].Play();
						m.B->Viewable = false;
						PlayerValues[i].AbilityAffected[5] = true;
						PlayerValues[i].AbilityCounter[5] = 0.0f;
					}
					//Short Circuit
					else if (m.B->SpecialAttribute() == 6) {
						powChannel[0] = powerup[5].Play();
						m.B->Viewable = false;
						PlayerValues[i].AbilityAffected[6] = true;
						PlayerValues[i].AbilityCounter[6] = 0.0f;
					}
					//Super Shockwave
					else if (m.B->SpecialAttribute() == 7) {
						powChannel[0] = powerup[6].Play();
						m.B->Viewable = false;
						PlayerValues[i].AbilityAffected[7] = true;
						PlayerValues[i].AbilityCounter[7] = 0.0f;
					}
					//Invincibility
					else if (m.B->SpecialAttribute() == 8) {
						powChannel[0] = powerup[7].Play();
						m.B->Viewable = false;
						PlayerValues[i].AbilityAffected[8] = true;
						PlayerValues[i].AbilityCounter[8] = 0.0f;
					}
					//Flipped
					else if (m.B->SpecialAttribute() == 9) {
						powChannel[0] = powerup[8].Play();
						m.B->Viewable = false;
						PlayerValues[i].AbilityAffected[9] = true;
						PlayerValues[i].AbilityCounter[9] = 0.0f;
					}
				}
				if (checkRadialCollision(m)) { ResolveCollision(m, 0.01f); }
				GameObjects["Specials_0" + to_string(j)] = m.B;
			}//end for
			GameObjects["Players_0" + to_string(i)] = m.A;
		}//end for
		 //Players to Objects
		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
			m.B = GameObjects["Players_0" + to_string(i)];
			bool onAObjectTemp = false;
			for (int j = 0; j < GameObjectsAmount["Objects_0"]; j++) {
				if (GameObjects["Objects_0" + to_string(j)].get()->Viewable && (j != 7 && j != 8 && j != 9)) {
					m.A = GameObjects["Objects_0" + to_string(j)];
					//avoid objects
					applyRadialAvoidingSystem(m, 0.00f, 01.0f);
					//collid with objects
					if (checkRadialCollision(m)) { ResolveCollision(m, 0.01f); }
					//stand ontop of objects
					if (CheckIfOnObject(m, 0.0f, false)) {
						if (m.B->onObjectNum != j) {
							std::cout << "[P:" << i << "]on[O:" << j << "]" << std::endl;
							onAObjectTemp = true; m.B->onObjectNum = j;
						}
					}

					/*
					if (CheckCollision(m)) {
						bool positiveXYZ[3] { false, false, false };
						bool negitiveXYZ[3] { false, false, false };

						if ((m.B->Position().x - (m.B->Radius().x*0.50f) < m.A->Top().x) && (m.B->Position().x - (m.B->Radius().x*0.50f) > m.A->Bottom().x)) {
							positiveXYZ[0] = true; negitiveXYZ[0] = false;
						}
						else if ((m.B->Position().x + (m.B->Radius().x*0.50f) > m.A->Bottom().x) && (m.B->Position().x + (m.B->Radius().x*0.50f) < m.A->Top().x)) {
							negitiveXYZ[0] = true; positiveXYZ[0] = false;
						}
						else { positiveXYZ[0] = false; negitiveXYZ[0] = false; }

						//if (m.B->Position().y + (m.B->Radius().y*0.5f) > m.A->Top().y) {
							//	m.B->setPosition(glm::vec3(m.B->Position().x, m.A->Top().y - (m.B->Radius().y*0.5f), m.B->Position().z));
							//	m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), -(m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
							//}
							//else if (m.B->Position().y - (m.B->Radius().y*0.5f) < m.A->Bottom().y) {
							//	m.B->setPosition(glm::vec3(m.B->Position().x, m.A->Bottom().y + (m.B->Radius().y*0.5f), m.B->Position().z));
							//	m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), -(m.B->Velocity().y), (m.B->Velocity().z*speedToWallDegradation.z)));
							//}
						if ((m.B->Position().z - (m.B->Radius().z*0.50f) < m.A->Top().z) && (m.B->Position().z - (m.B->Radius().z*0.50f) > m.A->Bottom().z)) {
							positiveXYZ[2] = true; negitiveXYZ[2] = false;
						}
						else if ((m.B->Position().z + (m.B->Radius().z*0.50f) > m.A->Bottom().z) && (m.B->Position().z + (m.B->Radius().z*0.50f) < m.A->Top().z)) {
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
							m.B->setPosition(glm::vec3(m.A->Top().x + (m.B->Radius().x*0.50f), m.B->Position().y, m.B->Position().z));
							m.B->setVelocity(glm::vec3(-(m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
						}
						else if (negitiveXYZ[0]) {
							m.B->setPosition(glm::vec3(m.A->Bottom().x - (m.B->Radius().x*0.50f), m.B->Position().y, m.B->Position().z));
							m.B->setVelocity(glm::vec3(-(m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
						}

						if (positiveXYZ[2]) {
							m.B->setPosition(glm::vec3(m.B->Position().x, m.B->Position().y, m.A->Top().z + (m.B->Radius().z*0.50f)));
							m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), -(m.B->Velocity().z*speedToWallDegradation.z)));
						}
						else if (negitiveXYZ[2]) {
							m.B->setPosition(glm::vec3(m.B->Position().x, m.B->Position().y, m.A->Bottom().z - (m.B->Radius().z*0.50f)));
							m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), -(m.B->Velocity().z*speedToWallDegradation.z)));
						}
					}
					*/

					GameObjects["Objects_0" + to_string(j)] = m.A;
				}
			}//end for

			if (onAObjectTemp && !m.B->IsJumping) { m.B->onObject = true; m.B->inAir = false; }
			else { m.B->onObject = false; m.B->inAir = true; }
			GameObjects["Players_0" + to_string(i)] = m.B;
		}//end for
		 //Players to Enemies
		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
			m.A = GameObjects["Players_0" + to_string(i)];
			for (int j = 0; j < GameObjectsAmount["Enemies_0"]; j++) {
				m.B = GameObjects["Enemies_0" + to_string(j)];
				if (CheckCollision(m)) {
					ResolveCollision(m, 0.0f, 1.0f);
					applySeekSystem(m, -0.50f);
					if (lastHit <= 0.0f) {
						lastHit = 0.25;
						FxChannel[0] = Fx[0].PlayNow();
					}
				}
				GameObjects["Enemies_0" + to_string(j)] = m.B;
			}//end for
			//GameObjects["Players_0" + to_string(i)] = m.A;
		}//end for

		 //Enemies to Rifts
		for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
			m.B = GameObjects["Enemies_0" + to_string(i)];
			for (int j = 0; j < GameObjectsAmount["Rifts_0"]; j++) {
				m.A = GameObjects["Rifts_0" + to_string(j)];
				if (ObjectsWithinRange(m, 15.0f)) { applySeekSystem(m, 2.0f); }
				if (CheckCollision(m)) {
					setEnemySpawn(m, i);
					if (!PlayerValues[j].AbilityAffected[8]) { PlayerHp[j].CurrentHealth -= 1; }
				}//end if
			}//end for
			GameObjects["Enemies_0" + to_string(i)] = m.B;
		}//end for
		 //Enemies to Objects
		for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
			m.B = GameObjects["Enemies_0" + to_string(i)];
			bool onAObjectTemp = false;
			for (int j = 0; j < GameObjectsAmount["Objects_0"]; j++) {
				m.A = GameObjects["Objects_0" + to_string(j)];
				//if (CheckCollision(m)) { ResolveCollision(m, 1.0f); }
				if (checkRadialCollision(m)) { ResolveCollision(m, 01.0f); }
				if (CheckIfOnObject(m, 0.0f, true)) { onAObjectTemp = true; }
				GameObjects["Objects_0" + to_string(j)] = m.A;
			}//end for
			if (onAObjectTemp) { m.B->onObject = true; m.B->inAir = false; }
			else { m.B->onObject = false; m.B->inAir = true; }
			GameObjects["Enemies_0" + to_string(i)] = m.B;
		}//end for
		 //Enemies to Specials
		for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
			m.A = GameObjects["Enemies_0" + to_string(i)];
			for (int j = 0; j < GameObjectsAmount["Specials_0"]; j++) {
				m.B = GameObjects["Specials_0" + to_string(j)];
				if (checkRadialCollision(m)) { ResolveCollision(m, 2.0f, 0.01f); }
				GameObjects["Specials_0" + to_string(j)] = m.B;
			}//end for
			GameObjects["Enemies_0" + to_string(i)] = m.A;
		}//end for
		 //Enemies to Enemies
		for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
			m.A = GameObjects["Enemies_0" + to_string(i)];
			for (int j = 0; j < GameObjectsAmount["Enemies_0"]; j++) {
				if (i != j) {
					m.B = GameObjects["Enemies_0" + to_string(j)];
					applyRadialAvoidingSystem(m, 01.0f, 0.20f);
					if (checkRadialCollision(m)) {
						ResolveCollision(m, 0.50f);
						//FxChannel[0] = Fx[0].Play();
					}
					GameObjects["Enemies_0" + to_string(j)] = m.B;
				}//end if
				else {}//end else
			}//end for
			GameObjects["Enemies_0" + to_string(i)] = m.A;
		}//end for
		 //Enemies to Borders
		for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
			m.A = GameObjects["Enemies_0" + to_string(i)];
			bool InBorder = false;

			//check to see if player is in a bounding box
			for (int j = 0; j < GameObjectsAmount["Borders_0"]; j++) {
				m.B = GameObjects["Borders_0" + to_string(j)];
				if (ObjectInBox(m)) { InBorder = true; }
			}

			if (InBorder) {
				m.A = GameObjects["Borders_0" + to_string(0)]; 
				m.B = GameObjects["Enemies_0" + to_string(i)];
				//If players are nearing the walls
				if (m.B->Position().x >(m.A->Top().x*0.90)) { FleeFromDirection(m, 2.0f, "x"); }
				else if (m.B->Position().x < (m.A->Bottom().x*0.90)) { FleeFromDirection(m, 2.0f, "-x"); }
				if (m.B->Position().z >(m.A->Top().z*0.90)) { FleeFromDirection(m, 2.0f, "z"); }
				else if (m.B->Position().z < (m.A->Bottom().z*0.90)) { FleeFromDirection(m, 2.0f, "-z"); }
				GameObjects["Enemies_0" + to_string(i)] = m.B;
			}
			//if the player is not in any box, seek to nearest one
			else {
				float distanceToNearestBox; int boxNumber_1 = 0; glm::vec3 boxWithLargerRadius(0.0f); int boxNumber_2 = 0;
				distanceToNearestBox = glm::distance(m.A->Position(), GameObjects["Borders_0" + to_string(0)].get()->Position());
				//find nearest bounding box
				for (int j = 0; j < GameObjectsAmount["Borders_0"]; j++) {
					m.B = GameObjects["Borders_0" + to_string(j)];
					glm::vec3 distNearPos = glm::vec3(glm::distance(m.A->Position().x, m.B->Position().x), glm::distance(m.A->Position().y, m.B->Position().y), glm::distance(m.A->Position().z, m.B->Position().z));
					if ((distNearPos.x < m.B->Radius().x) && (distNearPos.y < m.B->Radius().y) && (distNearPos.z < m.B->Radius().z)) {
						//box with larger radius
						if (boxWithLargerRadius.x < m.B->Radius().x && boxWithLargerRadius.y < m.B->Radius().y && boxWithLargerRadius.z < m.B->Radius().z) {
							boxWithLargerRadius = m.B->Radius();
							boxNumber_2 = j;
						}
						//closest box
						if (distanceToNearestBox > glm::distance(m.A->Position(), m.B->Position())) {
							distanceToNearestBox = glm::distance(m.A->Position(), m.B->Position());
							boxNumber_1 = j;
						}
					}
				}
				//switch the m.A & m.B cause only m.B is affected
				m.B = GameObjects["Borders_0" + to_string(boxNumber_2)];
				if (CheckCollision(m)) { m.A = GameObjects["Borders_0" + to_string(boxNumber_2)]; m.B = GameObjects["Enemies_0" + to_string(i)]; }
				else { m.A = GameObjects["Borders_0" + to_string(boxNumber_1)]; m.B = GameObjects["Enemies_0" + to_string(i)]; }


				//determine the wall collision
				if (m.B->Position().x + (m.B->Radius().x*0.5f) > m.A->Top().x) {
					m.B->setPosition(glm::vec3(m.A->Top().x - (m.B->Radius().x*0.5f), m.B->Position().y, m.B->Position().z));
					m.B->setVelocity(glm::vec3(-(m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().x - (m.B->Radius().x*0.5f) < m.A->Bottom().x) {
					m.B->setPosition(glm::vec3(m.A->Bottom().x + (m.B->Radius().x*0.5f), m.B->Position().y, m.B->Position().z));
					m.B->setVelocity(glm::vec3(-(m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B->Position().y + (m.B->Radius().y*0.5f) > m.A->Top().y) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.A->Top().y - (m.B->Radius().y*0.5f), m.B->Position().z));
					m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), -(m.B->Velocity().y*speedToWallDegradation.y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().y - (m.B->Radius().y*0.5f) < m.A->Bottom().y) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.A->Bottom().y + (m.B->Radius().y*0.5f), m.B->Position().z));
					m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), -(m.B->Velocity().y), (m.B->Velocity().z*speedToWallDegradation.z)));
				}
				if (m.B->Position().z + (m.B->Radius().z*0.5f) > m.A->Top().z) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.B->Position().y, m.A->Top().z - (m.B->Radius().z*0.5f)));
					m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), -(m.B->Velocity().z*speedToWallDegradation.z)));
				}
				else if (m.B->Position().z - (m.B->Radius().z*0.5f) < m.A->Bottom().z) {
					m.B->setPosition(glm::vec3(m.B->Position().x, m.B->Position().y, m.A->Bottom().z + (m.B->Radius().z*0.5f)));
					m.B->setVelocity(glm::vec3((m.B->Velocity().x*speedToWallDegradation.x), (m.B->Velocity().y*speedToWallDegradation.y), -(m.B->Velocity().z*speedToWallDegradation.z)));
				}


				//If players are nearing the walls
				if (m.B->Position().x >(m.A->Top().x*0.90)) { FleeFromDirection(m, 2.0f, "x"); }
				else if (m.B->Position().x < (m.A->Bottom().x*0.90)) { FleeFromDirection(m, 2.0f, "-x"); }
				if (m.B->Position().z >(m.A->Top().z*0.90)) { FleeFromDirection(m, 2.0f, "z"); }
				else if (m.B->Position().z < (m.A->Bottom().z*0.90)) { FleeFromDirection(m, 2.0f, "-z"); }


				GameObjects["Enemies_0" + to_string(i)] = m.B;
			}
		}
	}

	//Applying Gravity to everything
	if (ApplyingGravity) {
		m.A = GameObjects["Objects_0" + to_string(0)];
		//apply gravity relative to object[0]
		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) { m.B = GameObjects["Players_0" + to_string(i)]; applyGravitationalForces(m, -01.0f); GameObjects["Players_0" + to_string(i)] = m.B; }
		for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) { m.B = GameObjects["Enemies_0" + to_string(i)]; applyGravitationalForces(m, -5.0f); GameObjects["Enemies_0" + to_string(i)] = m.B; }
		for (int i = 0; i < GameObjectsAmount["Specials_0"]; i++) { m.B = GameObjects["Specials_0" + to_string(i)]; applyGravitationalForces(m, -2.0f); GameObjects["Specials_0" + to_string(i)] = m.B; }
	}

	
	bool enableAbilitys = true;
	//Applys all over-time abilitys
	if (enableAbilitys) {
		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {

			//Enemies seek towers [1]
			if (PlayerValues[i].AbilityAffected[1] && PlayerValues[i].AbilityCounter[1] < PlayerValues[i].AbilityLength[1]) {
				PlayerValues[i].AbilityCounter[1] += deltaTasSeconds;
				if (PlayerValues[i].PlayerTeam == 0) { m.A = GameObjects["Rifts_0" + to_string(1)]; }
				else if (PlayerValues[i].PlayerTeam == 1) { m.A = GameObjects["Rifts_0" + to_string(0)]; }
				for (int j = 0; j < GameObjectsAmount["Enemies_0"]; j++) {
					m.B = GameObjects["Enemies_0" + to_string(j)];
					applySeekSystem(m, 3.0f);
					GameObjects["Enemies_0" + to_string(j)] = m.B;
					EnemyParticleEmitter[j].Viewable = true;
				}
			}
			else { PlayerValues[i].AbilityAffected[1] = false; }
			
			//Toss-Up [2]
			if (PlayerValues[i].AbilityAffected[2] && PlayerValues[i].AbilityCounter[2] < PlayerValues[i].AbilityLength[2]) {
				PlayerValues[i].AbilityCounter[2] += deltaTasSeconds;
				for (int j = 0; j < GameObjectsAmount["Enemies_0"]; j++) { EnemyParticleEmitter[j].Viewable = true; }
			}
			else { PlayerValues[i].AbilityAffected[2] = false; }

			//Health Up [3]
			if (PlayerValues[i].AbilityAffected[3] && PlayerValues[i].AbilityCounter[3] < PlayerValues[i].AbilityLength[3]) {
				PlayerValues[i].AbilityCounter[3] += deltaTasSeconds;
			}
			else { PlayerValues[i].AbilityAffected[3] = false; }

			//Boost [4]
			if (PlayerValues[i].AbilityAffected[4] && PlayerValues[i].AbilityCounter[4] < PlayerValues[i].AbilityLength[4]) {
				PlayerValues[i].AbilityCounter[4] += deltaTasSeconds;
				GameObjects["Players_0" + to_string(i)].get()->SprintSpeed = PlayerValues[i].SprintSpeed;
			}
			else {
				GameObjects["Players_0" + to_string(i)].get()->SprintSpeed = 1.0f;
				PlayerValues[i].AbilityAffected[4] = false;
			}

			//Flee [5]
			if (PlayerValues[i].AbilityAffected[5] && PlayerValues[i].AbilityCounter[5] < PlayerValues[i].AbilityLength[5]) {
				PlayerValues[i].AbilityCounter[5] += deltaTasSeconds;
				m.A = GameObjects["Players_0" + to_string(i)];
				for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
					m.B = GameObjects["Enemies_0" + to_string(i)];
					if (applyRadialFleeingSystem(m, 20.0f, 10.0f)) {
						EnemyParticleEmitter[i].Viewable = true;
					}
					GameObjects["Enemies_0" + to_string(i)] = m.B;
				}
			}
			else { PlayerValues[i].AbilityAffected[5] = false; }

			//Short Circuit [6]
			if (PlayerValues[i].AbilityAffected[6] && PlayerValues[i].AbilityCounter[6] < PlayerValues[i].AbilityLength[6]) {
				PlayerValues[i].AbilityCounter[6] += deltaTasSeconds;
				if (PlayerValues[i].PlayerTeam == 0) {
					for (int j = 0; j < GameObjectsAmount["Players_0"]; j++) {
						if (PlayerValues[j].PlayerTeam == 1) {
							GameObjects["Players_0" + to_string(j)].get()->inShock = true;
						}
					}
				}
				else if (PlayerValues[i].PlayerTeam == 1) {
					for (int j = 0; j < GameObjectsAmount["Players_0"]; j++) {
						if (PlayerValues[j].PlayerTeam == 0) {
							GameObjects["Players_0" + to_string(j)].get()->inShock = true;
						}
					}
				}
			}
			else { PlayerValues[i].AbilityAffected[6] = false; }

			//Super Shockwave [7] //not done yet
			if (PlayerValues[i].AbilityAffected[7] && PlayerValues[i].AbilityCounter[7] < PlayerValues[i].AbilityLength[7]) {
				PlayerValues[i].AbilityCounter[7] += deltaTasSeconds;
			}
			else { PlayerValues[i].AbilityAffected[7] = false; }

			//Invincibility [8] 
			if (PlayerValues[i].AbilityAffected[8] && PlayerValues[i].AbilityCounter[8] < PlayerValues[i].AbilityLength[8]) {
				PlayerValues[i].AbilityCounter[8] += deltaTasSeconds;
			}
			else { PlayerValues[i].AbilityAffected[8] = false; }

			//Flipped [9]
			if (PlayerValues[i].AbilityAffected[9] && PlayerValues[i].AbilityCounter[9] < PlayerValues[i].AbilityLength[9]) {
				PlayerValues[i].AbilityCounter[9] += deltaTasSeconds;
				if (PlayerValues[i].PlayerTeam == 0) {
					for (int j = 0; j < GameObjectsAmount["Players_0"]; j++) {
						if (PlayerValues[j].PlayerTeam == 1) {
							PlayerValues[j].FlipedControllers = true;
						}
					}
				}
				else if (PlayerValues[i].PlayerTeam == 1) {
					for (int j = 0; j < GameObjectsAmount["Players_0"]; j++) {
						if (PlayerValues[j].PlayerTeam == 0) {
							PlayerValues[j].FlipedControllers = true;
						}
					}
				}
			}
			else { PlayerValues[i].AbilityAffected[9] = false; }


			for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
				if (GameObjects["Players_0" + to_string(i)].get()->inShock) { PlayerParticleEmitter[i].Viewable = true; }
			}

		}//end for

		for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
			if (GameObjects["Enemies_0" + to_string(i)].get()->inShock) { EnemyParticleEmitter[i].Viewable = true; }
		}
	}//end enableAbilitys


	//Can the users use the shock wave ability
	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
		//std::cout << "[]";
		if (PlayerValues[i].ShockWave) {
				GameObjects["Shockwave_0" + to_string(i)].get()->Viewable = true;
				if (PlayerValues[i].ShockWaveCounter > 0.0f) { PlayerValues[i].ShockWaveCounter -= deltaTasSeconds; }
				else { PlayerValues[i].ShockWave = false; PlayerValues[i].ShockWaveChargeUp = 0.0f; PlayerValues[i].SWMaxWeight = 0.0f; }

				float ForceWeight_Addition = 70.0f + ((2.4f * (0.80f + PlayerValues[i].ShockWaveChargeUp))*(2.4f * (0.80f + PlayerValues[i].ShockWaveChargeUp))*(2.4f * (0.80f + PlayerValues[i].ShockWaveChargeUp)));
				float ForceWeight_Minius = ((1.1f * (0.60f + PlayerValues[i].ShockWaveChargeUp))*(1.1f * (0.60f + PlayerValues[i].ShockWaveChargeUp))*(1.1f * (0.60f + PlayerValues[i].ShockWaveChargeUp))*(1.1f * (0.60f + PlayerValues[i].ShockWaveChargeUp)));
				if (PlayerValues[i].SWMaxWeight < 350.0f) { PlayerValues[i].SWMaxWeight = (ForceWeight_Addition - ForceWeight_Minius); }

				float ForceModifier = -(40.0f + 1.5f * PlayerValues[i].SWMaxWeight);

				glm::vec3 sizeofShockWave;
				sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (PlayerValues[i].SWMaxWeight*0.1f), 5.0f + (PlayerValues[i].SWMaxWeight*0.1f), 5.0f + (PlayerValues[i].SWMaxWeight*0.1f)), 0.25f);
				sizeofShockWave.y = 1.0f;

				GameObjects["Shockwave_0" + to_string(i)].get()->setPosition(GameObjects["Players_0" + to_string(i)].get()->Position());
				GameObjects["Shockwave_0" + to_string(i)].get()->setRotation(glm::vec3(0.0f, (GameObjects["Shockwave_0" + to_string(i)].get()->Angle().y + deltaTasSeconds*150.0f), 0.0f));
				GameObjects["Shockwave_0" + to_string(i)].get()->setScale(sizeofShockWave);
				sizeofShockWave.y = 100.0f;
				GameObjects["Shockwave_0" + to_string(i)].get()->setSizeOfHitBox(sizeofShockWave);

				m.A = GameObjects["Shockwave_0" + to_string(i)];
				//collision to Enemies
				for (int j = 0; j < GameObjectsAmount["Enemies_0"]; j++) {
					m.B = GameObjects["Enemies_0" + to_string(j)];
					if (checkRadialCollision(m)) { 
						applySeekSystem(m, ForceModifier);
						ResolveCollision(m, 0.0f, 1.0f);
						m.B->inShock = true;
						EnemyParticleEmitter[j].Viewable = true;
					}
					GameObjects["Enemies_0" + to_string(j)] = m.B;
				}//end for
				//collision to Specials
				for (int j = 0; j < GameObjectsAmount["Specials_0"]; j++) {
					m.B = GameObjects["Specials_0" + to_string(j)];
					if (checkRadialCollision(m)) { applySeekSystem(m, ForceModifier); ResolveCollision(m, 0.0f, 1.0f); m.B->inShock = true; }
					GameObjects["Specials_0" + to_string(j)] = m.B;
				}//end for
			}//end if
		else if (PlayerValues[i].ShockWave == false && PlayerValues[i].ShockWaveChargeUp > 0.250f) {
				
				GameObjects["Shockwave_0" + to_string(i)].get()->Viewable = true;
				float ForceWeight_Addition = 70.0f + ((2.4f * (0.80f + PlayerValues[i].ShockWaveChargeUp))*(2.4f * (0.80f + PlayerValues[i].ShockWaveChargeUp))*(2.4f * (0.80f + PlayerValues[i].ShockWaveChargeUp)));
				float ForceWeight_Minius = ((1.1f * (0.60f + PlayerValues[i].ShockWaveChargeUp))*(1.1f * (0.60f + PlayerValues[i].ShockWaveChargeUp))*(1.1f * (0.60f + PlayerValues[i].ShockWaveChargeUp))*(1.1f * (0.60f + PlayerValues[i].ShockWaveChargeUp)));
				
				if (PlayerValues[i].SWMaxWeight < 350.0f) { PlayerValues[i].ShockWaveChargeUp += (deltaTasSeconds*5.0f); PlayerValues[i].SWMaxWeight = (ForceWeight_Addition - ForceWeight_Minius); }
				else { PlayerValues[i].SWMaxWeight; }

				glm::vec3 sizeofShockWave;
				sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (PlayerValues[i].SWMaxWeight*0.1f), 5.0f + (PlayerValues[i].SWMaxWeight*0.1f), 5.0f + (PlayerValues[i].SWMaxWeight*0.1f)), 0.25f);
				sizeofShockWave.y = 1.0f;

				GameObjects["Shockwave_0" + to_string(i)].get()->setPosition(GameObjects["Players_0" + to_string(i)].get()->Position());
				GameObjects["Shockwave_0" + to_string(i)].get()->setScale(sizeofShockWave);
				GameObjects["Shockwave_0" + to_string(i)].get()->setRotation(glm::vec3(0.0f, (GameObjects["Shockwave_0" + to_string(i)].get()->Angle().y + deltaTasSeconds*150.0f), 0.0f));
			}
		else { GameObjects["Shockwave_0" + to_string(i)].get()->Viewable = false; }
	}//end for
	


	//Idle Enemies will despawn then respawn
	if (IdleEnemiesRespawn) {
		m.A = GameObjects["Objects_0" + to_string(0)];
		for (int j = 0; j < GameObjectsAmount["Enemies_0"]; j++)
		{
			m.B = GameObjects["Enemies_0" + to_string(j)];
			float ranPosj = static_cast<float>(rand() % 80 - 40);
			//object is [NOT idle]
			if (!m.B->AreIdle) {
				if (!m.B->inAir &&
					(m.B->Velocity().x >= -0.001f && m.B->Velocity().x <= 0.001f) &&
					(m.B->Velocity().z >= -0.001f && m.B->Velocity().z <= 0.001f))
				{
					m.B->AreIdle = true;
					m.B->IdleTimeCounter = 0.0f;
				}
				else {
					m.B->AreIdle = false;
					m.B->IdleTimeCounter = 0.0f;
				}
			}
			//object [IS idle]
			else if (m.B->AreIdle) {
				if (!m.B->inAir &&
					(m.B->Velocity().x >= -0.001f && m.B->Velocity().x <= 0.001f) &&
					(m.B->Velocity().z >= -0.001f && m.B->Velocity().z <= 0.001f))
				{
					//object [IS idle] for less then 20 seconds
					if (m.B->IdleTimeCounter < 20.0f) {
						m.B->IdleTimeCounter += deltaTasSeconds;
					}
					//object [IS idle] for more then 20 seconds
					else {
						setEnemySpawn(m, j);
						m.B->IdleTimeCounter = 0.0f;
						m.B->AreIdle = false;
						m.B->inAir = true;
					}
				}
				else {}
			}
			else {}
			GameObjects["Enemies_0" + to_string(j)] = m.B;
		}
	}

	if (randomSpecialTime <= 0.0f)
	{
		float ranPosZ = Random(-40.0f, 40.0f);
		float ranPosY = Random(20.0f, 100.0f);
		float ranPosX = Random(-40.0f, 40.0f);
		int ranSpec = static_cast<int>(Random(1.0f, 9.0f)); //1 to 9
		if (ranSpec == 7) { ranSpec = 6; }

		GameObjects["Specials_0" + to_string(ranSpec)].get()->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		GameObjects["Specials_0" + to_string(ranSpec)].get()->setPosition(glm::vec3(ranPosX, ranPosY, ranPosZ));
		GameObjects["Specials_0" + to_string(ranSpec)].get()->Viewable = true;

		randomSpecialTime = Random(10.0f, 30.0f);
	}
	else { randomSpecialTime -= deltaTasSeconds; }

	//enemie rotation
	for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
		if (GameObjects["Enemies_0" + to_string(i)].get()->Viewable) {
			GameObjects["Enemies_0" + to_string(i)].get()->setRotation(glm::vec3(GameObjects["Enemies_0" + to_string(i)].get()->Angle().x - (GameObjects["Enemies_0" + to_string(i)].get()->Velocity().z*10.0f), GameObjects["Enemies_0" + to_string(i)].get()->Angle().y, GameObjects["Enemies_0" + to_string(i)].get()->Angle().z - (GameObjects["Enemies_0" + to_string(i)].get()->Velocity().x*10.0f)));
		}
	}
	
	//Updating Targets
	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) { if (GameObjects["Players_0" + to_string(i)].get()->Viewable) { GameObjects["Players_0" + to_string(i)].get()->updateP(deltaTasSeconds); } }
	for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) { if (GameObjects["Enemies_0" + to_string(i)].get()->Viewable) { GameObjects["Enemies_0" + to_string(i)].get()->update(deltaTasSeconds); } }
	for (int i = 0; i < GameObjectsAmount["Specials_0"]; i++){ 
		if (GameObjects["Specials_0" + to_string(i)].get()->Viewable) {
			GameObjects["Specials_0" + to_string(i)].get()->update(deltaTasSeconds);
			GameObjects["Specials_0" + to_string(i)].get()->setRotation(glm::vec3(0.0f, GameObjects["Specials_0" + to_string(i)].get()->Angle().y + (deltaTasSeconds*73.0f), 0.0f));
		}
	}

	//Score
	if (changeInHealth(PlayerHp[0])) {
		if (PlayerHp[0].CurrentHealth < 0) {
			PlayerHp[0].CurrentHealth = 0;
			TeamThatLost[0] = true;
			inGame = false; inOptions = true;
			inOptionsTab = 5;
		}
		if (PlayerHp[0].CurrentHealth > PlayerHp[0].MaxHealth) { PlayerHp[0].CurrentHealth = PlayerHp[0].MaxHealth; }
		//Player One
		GameObjects["HUD_Score_Planes_0" + to_string(0)]->setTexture(TextureNumbers[static_cast<int>(PlayerHp[0].CurrentHealth % 10)]);
		GameObjects["HUD_Score_Planes_0" + to_string(1)]->setTexture(TextureNumbers[static_cast<int>(PlayerHp[0].CurrentHealth / 10 % 10)]);
		GameObjects["HUD_Score_Planes_0" + to_string(4)]->setTexture(TextureNumbers[static_cast<int>(PlayerHp[0].CurrentHealth % 10)]);
		GameObjects["HUD_Score_Planes_0" + to_string(5)]->setTexture(TextureNumbers[static_cast<int>(PlayerHp[0].CurrentHealth / 10 % 10)]);
	}
	if (changeInHealth(PlayerHp[1])) {
		if (PlayerHp[1].CurrentHealth < 0) { 
			PlayerHp[1].CurrentHealth = 0;
			TeamThatLost[1] = true;
			inGame = false; inOptions = true;
			inOptionsTab = 5;
		}
		if (PlayerHp[1].CurrentHealth > PlayerHp[1].MaxHealth) { PlayerHp[1].CurrentHealth = PlayerHp[1].MaxHealth; }
		//Player Two
		GameObjects["HUD_Score_Planes_0" + to_string(2)]->setTexture(TextureNumbers[static_cast<int>(PlayerHp[1].CurrentHealth % 10)]);
		GameObjects["HUD_Score_Planes_0" + to_string(3)]->setTexture(TextureNumbers[static_cast<int>(PlayerHp[1].CurrentHealth / 10 % 10)]);
		GameObjects["HUD_Score_Planes_0" + to_string(6)]->setTexture(TextureNumbers[static_cast<int>(PlayerHp[1].CurrentHealth % 10)]);
		GameObjects["HUD_Score_Planes_0" + to_string(7)]->setTexture(TextureNumbers[static_cast<int>(PlayerHp[1].CurrentHealth / 10 % 10)]);
	}

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
	else if (inGame) {
		for (int i = 0; i <= 1; i++) { InGameDraw(i); }

		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
			if (PlayerParticleEmitter[i].Viewable) { PlayerParticleEmitter[i].Viewable = false; }
		}
		for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
			if (EnemyParticleEmitter[i].Viewable) { EnemyParticleEmitter[i].Viewable = false; }
		}
	}
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
			break;
		case '_':
		case '-':
			TestFloat -= TestFloatIncrementAmount;
			std::cout << "[[" << TestFloat << "]]" << std::endl;
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
			for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
				if (PlayerValues[i].cameraMode == 0) { PlayerValues[i].cameraMode = 1; }
				else if (PlayerValues[i].cameraMode == 1) { PlayerValues[i].cameraMode = 0; }
			}
			break;
		case '0':
			if (TestFloat < 10 && TestFloat > 0) {
				GameObjects["Specials_0" + to_string(static_cast<int>(TestFloat))].get()->Viewable = true;
				GameObjects["Specials_0" + to_string(static_cast<int>(TestFloat))].get()->setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
				GameObjects["Specials_0" + to_string(static_cast<int>(TestFloat))].get()->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			}
			break;
		case '1':
			FxChannel[0] = Fx[0].Play();
			FxChannel[0]->setVolume(1.0f);
			break;
		case '2': 
			FxChannel[1] = Fx[1].Play();
			FxChannel[1]->setVolume(0.1f);
			break;
		case '3': 
			Fx[2].Play();
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

/*empty*/
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
		for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
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
		for (int j = 0; j < GameObjectsAmount["Players_0"]; j++) {
			if (PlayerValues[j].MenuSwitchCounter > 0.0f) { PlayerValues[j].MenuSwitchCounter -= deltaTasSecs; }
			else {
				if (inOptionsTab == 3) {
					bool pressedA = false;
					for (int i = 0; i < MenuObjectsAmount["HUD_Sliders_0"]; i++) {
						//move nob along the slider
						if (Slider[i].moveNob(MPosToOPosX, MPosToOPosY)) {
							MenuObjects["HUD_Slider_Button_0" + to_string(i)]->setPosition(glm::vec3(MPosToOPosX, 0.02f, Slider[i].SBar_Pos.z));
							pressedA = true;
						}
					}
					if (pressedA) { PlayerValues[j].MenuSwitchCounter = 0.10f; }
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////


void InitializeParticles() {

	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
		PlayerParticleEmitter[i].lifeRange = glm::vec3(01.0, 02.0, 00.0);
		PlayerParticleEmitter[i].initialForceMin = glm::vec3(-1.0, 03.0, -1.0);
		PlayerParticleEmitter[i].initialForceMax = glm::vec3(01.0, 10.0, 01.0);
		PlayerParticleEmitter[i].setMaterial(materials["particles"]);
		PlayerParticleEmitter[i].setTexture(ParticleTexture);
		PlayerParticleEmitter[i].initialize(300);
		PlayerParticleEmitter[i].play();
	}

	//for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) { EnemyParticleEmitter = new ParticleEmitterSoA; }

	for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) {
		EnemyParticleEmitter[i].lifeRange = glm::vec3(0.250, 01.0, 00.0);
		EnemyParticleEmitter[i].initialForceMin = glm::vec3(-1.0, -03.0, -1.0);
		EnemyParticleEmitter[i].initialForceMax = glm::vec3(01.0, -10.0, 01.0);
		EnemyParticleEmitter[i].setMaterial(materials["particles"]);
		EnemyParticleEmitter[i].setTexture(ParticleTexture);
		EnemyParticleEmitter[i].initialize(100);
		EnemyParticleEmitter[i].play();
	}
}

/**/
void InitializeVariables() {
	//Menu Objects Amounts
	MenuObjectsAmount["HUD_Planes_0"] = 8;
	MenuObjectsAmount["HUD_Messages_0"] = 1;
	MenuObjectsAmount["HUD_Buttons_0"] = 11;
	MenuObjectsAmount["HUD_Sliders_0"] = 10;
	for (int i = 0; i < MenuObjectsAmount["HUD_Planes_0"]; i++) { MenuObjects["HUD_Planes_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < MenuObjectsAmount["HUD_Messages_0"]; i++) { MenuObjects["HUD_Messages_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < MenuObjectsAmount["HUD_Buttons_0"]; i++) { MenuObjects["HUD_Buttons_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < MenuObjectsAmount["HUD_Sliders_0"]; i++) {
		MenuObjects["HUD_Slider_Bar_0" + to_string(i)] = std::make_shared<GameObject>();
		MenuObjects["HUD_Slider_Button_0" + to_string(i)] = std::make_shared<GameObject>();
	}
	Slider = new Sliders[MenuObjectsAmount["HUD_Sliders_0"]];
	Button = new Buttons[MenuObjectsAmount["HUD_Buttons_0"]];


	//Game Objects amounts
	GameObjectsAmount["Borders_0"] = 1;
	GameObjectsAmount["Objects_0"] = 10;
	GameObjectsAmount["HUD_Score_Planes_0"] = 8;
	GameObjectsAmount["Shadows_0"] = 1;
	GameObjectsAmount["Affects_0"] = 5;
	GameObjectsAmount["Specials_0"] = 10;
	GameObjectsAmount["Enemies_0"] = 12;
	GameObjectsAmount["Players_0"] = 4;
	GameObjectsAmount["Rifts_0"] = 2;
	for (int i = 0; i < GameObjectsAmount["Objects_0"]; i++) { GameObjects["Objects_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < GameObjectsAmount["Borders_0"]; i++) { GameObjects["Borders_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < GameObjectsAmount["HUD_Score_Planes_0"]; i++) { GameObjects["HUD_Score_Planes_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < GameObjectsAmount["Shadows_0"]; i++) { GameObjects["Shadows_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < GameObjectsAmount["Affects_0"]; i++) { GameObjects["Affects_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < GameObjectsAmount["Specials_0"]; i++) { GameObjects["Specials_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < GameObjectsAmount["Enemies_0"]; i++) { GameObjects["Enemies_0" + to_string(i)] = std::make_shared<GameObject>(); }
	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
		GameObjects["Players_0" + to_string(i)] = std::make_shared<GameObject>();
		GameObjects["Shockwave_0" + to_string(i)] = std::make_shared<GameObject>();
	}
	for (int i = 0; i < GameObjectsAmount["Rifts_0"]; i++) { GameObjects["Rifts_0" + to_string(i)] = std::make_shared<GameObject>(); }


	
	PlayerValues = new PlayerInfo[GameObjectsAmount["Players_0"]];
	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {

		PlayerValues[i].ControllerActive = false;

		if (i % 2 == 0) {
			PlayerValues[i].PlayerTeam = 0;
			PlayerValues[i].PlayerColour = 0;
			PlayerValues[i].forwardVector = glm::vec3(-1.0f, 0.0f, 0.0f);
		}
		else {
			PlayerValues[i].PlayerTeam = 1;
			PlayerValues[i].PlayerColour = 1;
			PlayerValues[i].forwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
		}

		//camera
		if (PlayerValues[i].PlayerTeam == 0) {
			PlayerValues[i].cameraTransform = glm::lookAt(
				glm::vec3(50.0f, 50.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			PlayerValues[i].cameraTransform = glm::lookAt(
				glm::vec3(-50.0f, 50.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}
		PlayerValues[i].cameraPosition = glm::vec3(0.0f);
		PlayerValues[i].rightVector = glm::vec3(1.0f);
		PlayerValues[i].cameraMode = 0;

		PlayerValues[i].MenuSwitchCounter = 0.0f;
		//Shock wave attributes
		PlayerValues[i].Right_TRIGGERED = false;
		PlayerValues[i].Left_TRIGGERED = false;
		PlayerValues[i].ShockWave = false;
		PlayerValues[i].ShockWaveCounter = 0.0f;
		PlayerValues[i].ShockWaveChargeUp = 0.0f;
		//Sprint attributes
		PlayerValues[i].SprintSpeed = 1.5f;
		PlayerValues[i].SprintCounter = 0.0f;
		PlayerValues[i].SprintCoolDown = 2.0f;
		//Abilitys
		PlayerValues[i].FlipedControllers = false;
		PlayerValues[i].AbilityAffected = new bool[GameObjectsAmount["Specials_0"]];
		PlayerValues[i].AbilityCounter = new float[GameObjectsAmount["Specials_0"]];
		PlayerValues[i].AbilityLength = new float[GameObjectsAmount["Specials_0"]];
		//setting abilitys
		for (int j = 0; j < GameObjectsAmount["Specials_0"]; j++) {
			PlayerValues[i].AbilityAffected[j] = false;
			PlayerValues[i].AbilityCounter[j] = 5.0f;
			PlayerValues[i].AbilityLength[j] = 5.0f;
		}
		//Seeker Swarm = 1
		//Toss-Up = 2
		//Health Up = 3
		//Boost = 4
		//Flee = 5
		//Short Circuit = 6
		PlayerValues[i].AbilityLength[6] = 1.0f;
		//Super Shockwave = 7
		//Invincibility = 8
		//Flipped = 9
	}
	//health
	for (int i = 0; i < 2; i++) {
		PlayerHp[i].MaxHealth = 99;
		PlayerHp[i].MinHealth = 0;
		PlayerHp[i].CurrentHealth = 50;
		PlayerHp[i].PreviousHealth = 0;
	}
}

/**/
void InitializeFrameBufferObjects()
{
	// Triangle 1
	myVBO.vertices.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	myVBO.textureCoordinates.push_back(glm::vec2(1.0f, 1.0f));
	myVBO.vertices.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
	myVBO.textureCoordinates.push_back(glm::vec2(0.0f, 1.0f));
	myVBO.vertices.push_back(glm::vec3(-1.0, -1.0, 0.0f));
	myVBO.textureCoordinates.push_back(glm::vec2(0.0f, 0.0f));
	// Triangle 2
	myVBO.vertices.push_back(glm::vec3(1.0, 1.0, 0.0f));
	myVBO.textureCoordinates.push_back(glm::vec2(1.0f, 1.0f));
	myVBO.vertices.push_back(glm::vec3(-1.0, -1.0, 0.0f));
	myVBO.textureCoordinates.push_back(glm::vec2(0.0f, 0.0f));
	myVBO.vertices.push_back(glm::vec3(1.0, -1.0, 0.0f));
	myVBO.textureCoordinates.push_back(glm::vec2(1.0f, 0.0f));
	createVBO(myVBO);


	FBOs["1"] = std::make_shared<FrameBufferObject>();
	FBOs["1"]->createFrameBuffer(windowWidth, windowHeight, 1, true);
	FBOs["2"] = std::make_shared<FrameBufferObject>();
	FBOs["2"]->createFrameBuffer(windowWidth, windowHeight, 1, true);
}

/**/
void InitializeShaders()
{
	std::string shaderPath;

	//Initialize  Shader
	if (DoesFileExists("..//Assets//shaders//")) { shaderPath = "..//Assets//shaders//"; }
	else if (DoesFileExists("Assets//shaders//")) { shaderPath = "Assets//shaders//"; }
	else { std::cout << "[ERROR] Could not find [Shaders]" << std::endl; }

	//Load vertex shaders
	Shader v_default, v_passThrough, v_textShader, v_simpleDepthShader;
	v_default.loadShaderFromFile(shaderPath + "default_v.glsl", GL_VERTEX_SHADER);
	v_passThrough.loadShaderFromFile(shaderPath + "passThru_v.glsl", GL_VERTEX_SHADER);
	v_textShader.loadShaderFromFile(shaderPath + "text_v.glsl", GL_VERTEX_SHADER);
	v_simpleDepthShader.loadShaderFromFile(shaderPath + "simpleDepthShader_v.glsl", GL_VERTEX_SHADER);
	//Load fragment shaders
	Shader f_default, f_passThrough, f_textShader, f_NASD, f_Depth;
	f_default.loadShaderFromFile(shaderPath + "default_f.glsl", GL_FRAGMENT_SHADER);
	f_passThrough.loadShaderFromFile(shaderPath + "passThru_f.glsl", GL_FRAGMENT_SHADER);
	f_NASD.loadShaderFromFile(shaderPath + "NASD_f.glsl", GL_FRAGMENT_SHADER);
	f_textShader.loadShaderFromFile(shaderPath + "text_f.glsl", GL_FRAGMENT_SHADER);
	f_Depth.loadShaderFromFile(shaderPath + "Depth_f.glsl", GL_FRAGMENT_SHADER);
	// Load Geometry Shaders
	Shader g_normals, g_particles;
	g_normals.loadShaderFromFile(shaderPath + "normals_g.glsl", GL_GEOMETRY_SHADER);
	g_particles.loadShaderFromFile(shaderPath + "particles_g.glsl", GL_GEOMETRY_SHADER);


	//Default material that all objects use
	materials["default"] = std::make_shared<Material>();
	materials["default"]->shader->attachShader(v_default);
	materials["default"]->shader->attachShader(f_default);
	materials["default"]->shader->linkProgram();
	//passThrough material that all objects use
	materials["passThrough"] = std::make_shared<Material>();
	materials["passThrough"]->shader->attachShader(v_default);
	materials["passThrough"]->shader->attachShader(f_passThrough);
	materials["passThrough"]->shader->linkProgram();
	//NASD material that all objects use
	materials["NASD"] = std::make_shared<Material>();
	materials["NASD"]->shader->attachShader(v_default);
	materials["NASD"]->shader->attachShader(f_NASD);
	materials["NASD"]->shader->linkProgram();

	//NASD material that all objects use
	materials["Depth"] = std::make_shared<Material>();
	materials["Depth"]->shader->attachShader(v_simpleDepthShader);
	materials["Depth"]->shader->attachShader(f_Depth);
	materials["Depth"]->shader->linkProgram();


	// Default lambert material with normal visualization  geometry shader
	materials["normals"] = std::make_shared<Material>();
	materials["normals"]->shader->attachShader(v_passThrough); // Vertex Shader
	materials["normals"]->shader->attachShader(f_NASD); // Fragment Shader
	materials["normals"]->shader->attachShader(g_normals); // Geometry Shader!
	materials["normals"]->shader->linkProgram();
	// Unlit texture material with point-to-quad geometry shader
	materials["particles"] = std::make_shared<Material>();
	materials["particles"]->shader->attachShader(v_passThrough);
	materials["particles"]->shader->attachShader(g_particles); // Geometry Shader!
	materials["particles"]->shader->attachShader(f_NASD);
	materials["particles"]->shader->linkProgram();


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
	



	////////////////	//Text planes
	MenuObjects["HUD_Planes_0" + to_string(0)].get()->objectLoader(ObjectPath + "PlainForText.obj");
	MenuObjects["HUD_Planes_0" + to_string(0)].get()->setMaterial(passThroughMaterial);
	MenuObjects["HUD_Planes_0" + to_string(0)].get()->setMass(0.0f);
	MenuObjects["HUD_Planes_0" + to_string(0)].get()->Viewable = true;
	MenuObjects["HUD_Planes_0" + to_string(0)].get()->setScale(glm::vec3(39.0f*0.9f, 1.0f, 39.0f*1.6f));
	MenuObjects["HUD_Planes_0" + to_string(0)].get()->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	MenuObjects["HUD_Planes_0" + to_string(0)].get()->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	for (int i = 1; i < MenuObjectsAmount["HUD_Planes_0"]; i++) {
		MenuObjects["HUD_Planes_0" + to_string(i)].get()->objectLoader(&MenuObjects["HUD_Planes_0" + to_string(0)]);
	}
	MenuObjects["HUD_Planes_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Main_Menu.png").c_str())));
	MenuObjects["HUD_Planes_0" + to_string(1)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options.png").c_str())));
	MenuObjects["HUD_Planes_0" + to_string(2)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Controls.png").c_str())));
	MenuObjects["HUD_Planes_0" + to_string(3)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Power_Ups.png").c_str())));
	MenuObjects["HUD_Planes_0" + to_string(4)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options.png").c_str())));
	MenuObjects["HUD_Planes_0" + to_string(5)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Player_Select.png").c_str())));
	MenuObjects["HUD_Planes_0" + to_string(6)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//End_Screen.png").c_str())));
	MenuObjects["HUD_Planes_0" + to_string(7)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Game_Mode.png").c_str())));
	//

	MenuObjects["HUD_Messages_0" + to_string(0)].get()->objectLoader(ObjectPath + "PlainForText.obj");
	MenuObjects["HUD_Messages_0" + to_string(0)].get()->setMaterial(passThroughMaterial);
	MenuObjects["HUD_Messages_0" + to_string(0)].get()->setMass(0.0f);
	MenuObjects["HUD_Messages_0" + to_string(0)].get()->Viewable = true;
	MenuObjects["HUD_Messages_0" + to_string(0)].get()->setScale(glm::vec3(39.0f*0.9f, 1.0f, 39.0f*1.6f));
	MenuObjects["HUD_Messages_0" + to_string(0)].get()->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	for (int i = 1; i < MenuObjectsAmount["HUD_Messages_0"]; i++) {
		MenuObjects["HUD_Messages_0" + to_string(i)].get()->objectLoader(&MenuObjects["HUD_Messages_0" + to_string(0)]);
	}
	MenuObjects["HUD_Messages_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//PlayerSelect//Press_Start_to_Start.png").c_str())));


	////////////////	//Button
	MenuObjects["HUD_Buttons_0" + to_string(0)].get()->objectLoader(ObjectPath + "PlainForText.obj");
	MenuObjects["HUD_Buttons_0" + to_string(0)].get()->setMaterial(passThroughMaterial);
	MenuObjects["HUD_Buttons_0" + to_string(0)].get()->setScale(glm::vec3(9.0f*0.9f, 1.0f, 7.0f*1.6f));
	MenuObjects["HUD_Buttons_0" + to_string(0)].get()->setSizeOfHitBox(glm::vec3(9.0f*1.45f, 1.0f, 7.0f*1.58f));
	MenuObjects["HUD_Buttons_0" + to_string(0)].get()->setPosition(glm::vec3(0.0f, 0.01f, -3.0f));
	MenuObjects["HUD_Buttons_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Start_Button.png").c_str())));
	for (int i = 1; i < MenuObjectsAmount["HUD_Buttons_0"]; i++) {
		MenuObjects["HUD_Buttons_0" + to_string(i)].get()->objectLoader(&MenuObjects["HUD_Buttons_0" + to_string(0)]);
	}
	MenuObjects["HUD_Buttons_0" + to_string(1)].get()->setPosition(glm::vec3(0.0f, 0.01f, 9.0f));
	MenuObjects["HUD_Buttons_0" + to_string(1)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Options_Button.png").c_str())));
	MenuObjects["HUD_Buttons_0" + to_string(2)].get()->setPosition(glm::vec3(0.0f, 0.01f, 21.0f));
	MenuObjects["HUD_Buttons_0" + to_string(2)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Exit_Button.png").c_str())));
	MenuObjects["HUD_Buttons_0" + to_string(3)].get()->setPosition(glm::vec3(-21.0f, 0.01f, -21.0f));
	MenuObjects["HUD_Buttons_0" + to_string(3)].get()->setScale(glm::vec3(2.0f*0.9f, 1.0f, 3.50f*0.9f));
	MenuObjects["HUD_Buttons_0" + to_string(3)].get()->setSizeOfHitBox(glm::vec3(2.0f*1.5f, 1.0f, 3.50f*0.9f));
	MenuObjects["HUD_Buttons_0" + to_string(3)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Back_Button.png").c_str())));
	MenuObjects["HUD_Buttons_0" + to_string(4)].get()->setPosition(glm::vec3(0.0f, 0.01f, -3.0f));
	MenuObjects["HUD_Buttons_0" + to_string(4)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Controls_Button.png").c_str())));
	MenuObjects["HUD_Buttons_0" + to_string(5)].get()->setPosition(glm::vec3(0.0f, 0.01f, 9.0f));
	MenuObjects["HUD_Buttons_0" + to_string(5)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Power_Ups_Button.png").c_str())));
	MenuObjects["HUD_Buttons_0" + to_string(6)].get()->setPosition(glm::vec3(0.0f, 0.01f, 21.0f));
	MenuObjects["HUD_Buttons_0" + to_string(6)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Config_Button.png").c_str())));


	for (int i = 0; i < 3; i++) {
		PlayerSelectColour[(i * 2)] = ilutGLLoadImage(_strdup((ImagePath + "Player//Blue//PlayerSelect_B_0" + to_string(i + 1) + ".png").c_str()));
		PlayerSelectColour[(i * 2) + 1] = ilutGLLoadImage(_strdup((ImagePath + "Player//Red//PlayerSelect_R_0" + to_string(i + 1) + ".png").c_str()));
	}
	PlayerSelectColour[6] = ilutGLLoadImage(_strdup((ImagePath + "Menu//PlayerSelect//Press_A_to_Join.png").c_str()));
	MenuObjects["HUD_Buttons_0" + to_string(7)].get()->setPosition(glm::vec3(-9.0f, 0.01f, -3.0f));
	MenuObjects["HUD_Buttons_0" + to_string(7)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//PlayerSelect//Press_A_to_Join.png").c_str())));
	MenuObjects["HUD_Buttons_0" + to_string(8)].get()->setPosition(glm::vec3(9.0f, 0.01f, -3.0f));
	MenuObjects["HUD_Buttons_0" + to_string(8)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//PlayerSelect//Press_A_to_Join.png").c_str())));
	MenuObjects["HUD_Buttons_0" + to_string(9)].get()->setPosition(glm::vec3(-9.0f, 0.01f, 9.0f));
	MenuObjects["HUD_Buttons_0" + to_string(9)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//PlayerSelect//Press_A_to_Join.png").c_str())));
	MenuObjects["HUD_Buttons_0" + to_string(10)].get()->setPosition(glm::vec3(9.0f, 0.01f, 9.0f));
	MenuObjects["HUD_Buttons_0" + to_string(10)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//PlayerSelect//Press_A_to_Join.png").c_str())));
	for (int i = 0; i < MenuObjectsAmount["HUD_Buttons_0"]; i++) {
		Button[i].SBut_Top = MenuObjects["HUD_Buttons_0" + to_string(i)]->Top();
		Button[i].SBut_Bot = MenuObjects["HUD_Buttons_0" + to_string(i)]->Bottom();
		Button[i].SBut_Pos = MenuObjects["HUD_Buttons_0" + to_string(i)]->Position();
		Button[i].SBut_Rad = (MenuObjects["HUD_Buttons_0" + to_string(i)]->Radius() / 2.0f);
	}


	////////////////	//Slider
	MenuObjects["HUD_Slider_Bar_0" + to_string(0)].get()->objectLoader(ObjectPath + "PlainForText.obj");
	MenuObjects["HUD_Slider_Bar_0" + to_string(0)].get()->setMaterial(passThroughMaterial);
	MenuObjects["HUD_Slider_Bar_0" + to_string(0)].get()->setScale(glm::vec3(5.0f, 1.0f, 3.50f));
	MenuObjects["HUD_Slider_Bar_0" + to_string(0)].get()->setSizeOfHitBox(glm::vec3(5.0f*1.6f, 1.0f, 3.50f*0.9f));
	MenuObjects["HUD_Slider_Bar_0" + to_string(0)].get()->setPosition(glm::vec3(20.0f, 0.01f, -10.0f));
	MenuObjects["HUD_Slider_Bar_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Slider_Bar.png").c_str())));
	MenuObjects["HUD_Slider_Button_0" + to_string(0)].get()->objectLoader(&MenuObjects["HUD_Slider_Bar_0" + to_string(0)]);
	MenuObjects["HUD_Slider_Button_0" + to_string(0)].get()->setScale(glm::vec3(1.0f, 1.0f, 3.50f));
	MenuObjects["HUD_Slider_Button_0" + to_string(0)].get()->setSizeOfHitBox(glm::vec3(1.0f, 1.0f, 3.50f*0.9f));
	MenuObjects["HUD_Slider_Button_0" + to_string(0)].get()->setPosition(glm::vec3(20.0f, 0.02f, -10.0f));
	MenuObjects["HUD_Slider_Button_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Menu//Options_Buttons//Slider_Nob.png").c_str())));
	Slider[0].SBar_Top = MenuObjects["HUD_Slider_Bar_0" + to_string(0)]->Top(), Slider[0].SBar_Bot = MenuObjects["HUD_Slider_Bar_0" + to_string(0)]->Bottom(), Slider[0].SBar_Pos = MenuObjects["HUD_Slider_Bar_0" + to_string(0)]->Position(), Slider[0].SBar_Rad = (MenuObjects["HUD_Slider_Bar_0" + to_string(0)]->Radius() / 2.0f);
	Slider[0].SNob_Top = MenuObjects["HUD_Slider_Button_0" + to_string(0)]->Top(), Slider[0].SNob_Bot = MenuObjects["HUD_Slider_Button_0" + to_string(0)]->Bottom(), Slider[0].SNob_Pos = MenuObjects["HUD_Slider_Button_0" + to_string(0)]->Position(), Slider[0].SNob_Rad = (MenuObjects["HUD_Slider_Button_0" + to_string(0)]->Radius() / 2.0f);
	Slider[0].SBar_Length = ((Slider[0].SBar_Top - Slider[0].SNob_Rad) - (Slider[0].SBar_Bot + Slider[0].SNob_Rad));
	Slider[0].SNob_Length = (Slider[0].SNob_Pos - (Slider[0].SBar_Bot + Slider[0].SNob_Rad));
	Slider[0].SNob_Precent = (Slider[0].SNob_Length / Slider[0].SBar_Length)*100.0f;
	for (int i = 1; i < MenuObjectsAmount["HUD_Sliders_0"]; i++) {
		//Slider bar
		MenuObjects["HUD_Slider_Bar_0" + to_string(i)].get()->objectLoader(&MenuObjects["HUD_Slider_Bar_0" + to_string(0)]);
		MenuObjects["HUD_Slider_Bar_0" + to_string(i)].get()->setPosition(MenuObjects["HUD_Slider_Bar_0" + to_string(i-1)]->Position() + glm::vec3(0.0f, 0.0f, MenuObjects["HUD_Slider_Bar_0" + to_string(i - 1)]->Radius().z));
		//Slider nob
		MenuObjects["HUD_Slider_Button_0" + to_string(i)].get()->objectLoader(&MenuObjects["HUD_Slider_Button_0" + to_string(0)]);
		MenuObjects["HUD_Slider_Button_0" + to_string(i)].get()->setPosition(MenuObjects["HUD_Slider_Button_0" + to_string(i - 1)]->Position() + glm::vec3(0.0f, 0.0f, MenuObjects["HUD_Slider_Button_0" + to_string(i - 1)]->Radius().z));
		//Slider full
		Slider[i].SBar_Top = MenuObjects["HUD_Slider_Bar_0" + to_string(i)]->Top(), Slider[i].SBar_Bot = MenuObjects["HUD_Slider_Bar_0" + to_string(i)]->Bottom(), Slider[i].SBar_Pos = MenuObjects["HUD_Slider_Bar_0" + to_string(i)]->Position(), Slider[i].SBar_Rad = (MenuObjects["HUD_Slider_Bar_0" + to_string(i)]->Radius() / 2.0f);
		Slider[i].SNob_Top = MenuObjects["HUD_Slider_Button_0" + to_string(i)]->Top(), Slider[i].SNob_Bot = MenuObjects["HUD_Slider_Button_0" + to_string(i)]->Bottom(), Slider[i].SNob_Pos = MenuObjects["HUD_Slider_Button_0" + to_string(i)]->Position(), Slider[i].SNob_Rad = (MenuObjects["HUD_Slider_Button_0" + to_string(i)]->Radius() / 2.0f);
		Slider[i].SBar_Length = ((Slider[i].SBar_Top - Slider[i].SNob_Rad) - (Slider[i].SBar_Bot + Slider[i].SNob_Rad));
		Slider[i].SNob_Length = (Slider[i].SNob_Pos - (Slider[i].SBar_Bot + Slider[i].SNob_Rad));
		Slider[i].SNob_Precent = (Slider[i].SNob_Length / Slider[i].SBar_Length)*100.0f;
	}
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

	Sound::Sys.Init();

	for (int i = 0; i < 2; i++) {
		Sound::Sys.listenerPos[i].x = 0.0f; Sound::Sys.listenerPos[i].y = 0.0f; Sound::Sys.listenerPos[i].z = 0.0f;
	}

	//Power up sound effects

	//Seeker Swarm = 0
	powerup[0].Load(_strdup((SoundPath + "PowerUps//Voicerecording_seeker.wav").c_str()), TRUE, FALSE);
	//Toss-Up = 1
	powerup[1].Load(_strdup((SoundPath + "PowerUps//Voicerecording_toss.wav").c_str()), TRUE, FALSE);
	//Health Up = 2
	powerup[2].Load(_strdup((SoundPath + "PowerUps//Voicerecording_healthup.wav").c_str()), TRUE, FALSE);
	//Boost = 3
	powerup[3].Load(_strdup((SoundPath + "PowerUps//Voicerecording_boost.wav").c_str()), TRUE, FALSE);
	//Flee = 4
	powerup[4].Load(_strdup((SoundPath + "PowerUps//Voicerecording_flee.wav").c_str()), TRUE, FALSE);
	//Short Circuit = 5
	powerup[5].Load(_strdup((SoundPath + "PowerUps//Voicerecording_shortcircuit.wav").c_str()), TRUE, FALSE);
	//Super Shockwave = 6
	powerup[6].Load(_strdup((SoundPath + "PowerUps//Voicerecording_supershockwave.wav").c_str()), TRUE, FALSE);
	//Invincibility = 7
	powerup[7].Load(_strdup((SoundPath + "PowerUps//Voicerecording_invincible.wav").c_str()), TRUE, FALSE);
	//Flipped = 8
	powerup[8].Load(_strdup((SoundPath + "PowerUps//Voicerecording_flipped.wav").c_str()), TRUE, FALSE);


	//Fx sound effects
	Fx[0].Load(_strdup((SoundPath + "Soundeffects//bounce_03.wav").c_str()), TRUE, FALSE);//Ball bouncing
	Fx[1].Load(_strdup((SoundPath + "Soundeffects//crowdsound_01.wav").c_str()), TRUE, TRUE);//Crowd sound effect
	Fx[2].Load(_strdup((SoundPath + "Soundeffects//Cheering_01.wav").c_str()), TRUE, FALSE);//Cheering effect, for when someone scores a goal

	//Set Inital Volume of FX
	FxChannel[0]->setVolume(1.0f);
	FxChannel[1]->setVolume(0.1f);
	FxChannel[2]->setVolume(0.8f);
}

/**/
void InitializeNumbers() {
	auto passThroughMaterial = materials["passThrough"];

	std::string ImagePath; std::string ObjectPath;
	if (DoesFileExists("..//Assets//Img") && DoesFileExists("..//Assets//Obj")) {
		ImagePath = "..//Assets//Img//";
		ObjectPath = "..//Assets//Obj//";
	}
	else if (DoesFileExists("Assets//Img") && DoesFileExists("Assets//Obj")) {
		ImagePath = "Assets//Img//";
		ObjectPath = "Assets//Obj//";
	}
	else { std::cout << "[ERROR] Could not find [Img] and/or [Obj]" << std::endl; }


	GameObjects["HUD_Score_Planes_0" + to_string(0)].get()->objectLoader(ObjectPath + "PlainForText.obj");
	GameObjects["HUD_Score_Planes_0" + to_string(0)].get()->setMaterial(passThroughMaterial);
	GameObjects["HUD_Score_Planes_0" + to_string(0)].get()->textureHandle_hasTransparency = true;
	GameObjects["HUD_Score_Planes_0" + to_string(0)].get()->setMass(0.0f);
	GameObjects["HUD_Score_Planes_0" + to_string(0)].get()->setScale(glm::vec3(2.50f, 2.0f, 5.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "HUD//Numbers//Numbers_00.png").c_str())));
	for (int i = 1; i < GameObjectsAmount["HUD_Score_Planes_0"]; i++) { GameObjects["HUD_Score_Planes_0" + to_string(i)].get()->objectLoader(&GameObjects["HUD_Score_Planes_0" + to_string(0)]); }
	//Player One
	GameObjects["HUD_Score_Planes_0" + to_string(0)].get()->setRotation(glm::vec3(000.00f, 90.00f, 270.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(0)].get()->setPosition(glm::vec3(-48.50f, 05.50f, 029.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(1)].get()->setRotation(glm::vec3(000.00f, 90.00f, 270.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(1)].get()->setPosition(glm::vec3(-48.50f, 05.50f, 033.50f));
	//Player Two
	GameObjects["HUD_Score_Planes_0" + to_string(2)].get()->setRotation(glm::vec3(000.00f, 90.00f, 270.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(2)].get()->setPosition(glm::vec3(-48.50f, 05.50f, -33.50f));
	GameObjects["HUD_Score_Planes_0" + to_string(3)].get()->setRotation(glm::vec3(000.00f, 90.00f, 270.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(3)].get()->setPosition(glm::vec3(-48.50f, 05.50f, -29.0f));
	//Player One
	GameObjects["HUD_Score_Planes_0" + to_string(4)].get()->setRotation(glm::vec3(180.00f, 270.0f, 270.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(4)].get()->setPosition(glm::vec3(048.50f, 05.50f, -29.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(5)].get()->setRotation(glm::vec3(180.00f, 270.0f, 270.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(5)].get()->setPosition(glm::vec3(048.50f, 05.50f, -33.50f));
	//Player Two
	GameObjects["HUD_Score_Planes_0" + to_string(6)].get()->setRotation(glm::vec3(180.00f, 270.0f, 270.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(6)].get()->setPosition(glm::vec3(048.50f, 05.50f, 033.50f));
	GameObjects["HUD_Score_Planes_0" + to_string(7)].get()->setRotation(glm::vec3(180.00f, 270.0f, 270.0f));
	GameObjects["HUD_Score_Planes_0" + to_string(7)].get()->setPosition(glm::vec3(048.50f, 05.50f, 029.0f));

	for (int i = 0; i < 10; i++) {
		TextureNumbers[i] = ilutGLLoadImage(_strdup((ImagePath + "HUD//Numbers//Numbers_0" + to_string(i) + ".png").c_str()));
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
	auto NASDMaterial = materials["NASD"];//Depth //NASD
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





	GameObjects["Borders_0" + to_string(0)].get()->objectLoader(ObjectPath + "Square.obj");
	GameObjects["Borders_0" + to_string(0)].get()->setMaterial(NASDMaterial);
	GameObjects["Borders_0" + to_string(0)].get()->Viewable = false;
	GameObjects["Borders_0" + to_string(0)].get()->setMass(0.0f);
	GameObjects["Borders_0" + to_string(0)].get()->setScale(glm::vec3(98.0f, 100.0f, 98.0f));
	GameObjects["Borders_0" + to_string(0)].get()->setSizeOfHitBox(glm::vec3(98.0f, 100.0f, 98.0f)); //HitBox
	GameObjects["Borders_0" + to_string(0)].get()->setPosition(glm::vec3(0.0f, 50.0f, 0.0f));

	//PlayerTextures
	for (int i = 0; i < 3; i++) {
		PlayerTextures[(i*2)] = ilutGLLoadImage(_strdup((ImagePath + "Player//Blue//B_blitzbot_diff_0" + to_string(i+1) + ".png").c_str()));
		PlayerTextures[(i*2)+1] = ilutGLLoadImage(_strdup((ImagePath + "Player//Red//R_blitzbot_diff_0" + to_string(i+1) + ".png").c_str()));

		PlayerTextures[(i*2)+6] = ilutGLLoadImage(_strdup((ImagePath + "Player//Blue//ShockWave_B_0" + to_string(i+1) + ".png").c_str()));
		PlayerTextures[(i*2)+7] = ilutGLLoadImage(_strdup((ImagePath + "Player//Red//ShockWave_R_0" + to_string(i+1) + ".png").c_str()));
	}
	GameObjects["Players_0" + to_string(0)].get()->objectLoader(ObjectPath + "Player//blitzbot.obj");
	GameObjects["Players_0" + to_string(0)].get()->setMaterial(NASDMaterial);
	GameObjects["Players_0" + to_string(0)].get()->Viewable = true;
	GameObjects["Players_0" + to_string(0)].get()->setMass(5.0f);
	GameObjects["Players_0" + to_string(0)].get()->setScale(glm::vec3(5.0f, 5.0f, 5.0f)); //displayed size
	GameObjects["Players_0" + to_string(0)].get()->setSizeOfHitBox(glm::vec3(10.0f, 2.50f, 10.0f)); //HitBox
	GameObjects["Players_0" + to_string(0)].get()->setPosition(glm::vec3(15.0f, -1.0f, 0.0f)); //Position of Object
	GameObjects["Players_0" + to_string(0)].get()->setTexture(PlayerTextures[0]);
	//ShockWave
	GameObjects["Shockwave_0" + to_string(0)].get()->objectLoader(ObjectPath + "Player//ShockWave.obj");
	GameObjects["Shockwave_0" + to_string(0)].get()->setMaterial(NASDMaterial);
	GameObjects["Shockwave_0" + to_string(0)].get()->Viewable = false;
	GameObjects["Shockwave_0" + to_string(0)].get()->textureHandle_hasTransparency = true;
	GameObjects["Shockwave_0" + to_string(0)].get()->setMass(5.0f);
	for (int i = 1; i < GameObjectsAmount["Players_0"]; i++) {
		GameObjects["Players_0" + to_string(i)].get()->objectLoader(&GameObjects["Players_0" + to_string(0)]);
		GameObjects["Players_0" + to_string(i)].get()->setPosition(glm::vec3(-15.0f, -1.0f, 0.0f)); //Position of Object
		GameObjects["Shockwave_0" + to_string(i)].get()->objectLoader(&GameObjects["Shockwave_0" + to_string(0)]);
	}
	//set player team textures
	for (int i = 0; i < GameObjectsAmount["Players_0"]; i++) {
		
		if (PlayerValues[i].PlayerTeam == 0) {
			PlayerValues[i].PlayerColour = 0;
			PlayerValues[i].PlayerTexture = PlayerTextures[0];
			GameObjects["Players_0" + to_string(i)].get()->setTexture(PlayerTextures[0]);
			GameObjects["Shockwave_0" + to_string(i)].get()->setTexture(PlayerTextures[6]);
		}
		else if (PlayerValues[i].PlayerTeam == 1) {
			PlayerValues[i].PlayerColour = 1;
			PlayerValues[i].PlayerTexture = PlayerTextures[1];
			GameObjects["Players_0" + to_string(i)].get()->setTexture(PlayerTextures[1]);
			GameObjects["Shockwave_0" + to_string(i)].get()->setTexture(PlayerTextures[7]);
		}

		//camera
		if (PlayerValues[i].PlayerTeam == 0) {
			PlayerValues[i].cameraTransform = glm::lookAt(
				glm::vec3((GameObjects["Players_0" + to_string(0)].get()->Position().x + 50.0f), 50.0f, (GameObjects["Players_0" + to_string(0)].get()->Position().z)),
				glm::vec3((GameObjects["Players_0" + to_string(0)].get()->Position().x), 1.0f, (GameObjects["Players_0" + to_string(0)].get()->Position().z)),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			PlayerValues[i].cameraTransform = glm::lookAt(
				glm::vec3((GameObjects["Players_0" + to_string(0)].get()->Position().x - 50.0f), 50.0f, (GameObjects["Players_0" + to_string(0)].get()->Position().z)),
				glm::vec3((GameObjects["Players_0" + to_string(0)].get()->Position().x), 1.0f, (GameObjects["Players_0" + to_string(0)].get()->Position().z)),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}
		PlayerValues[i].cameraPosition = glm::vec3((GameObjects["Players_0" + to_string(i)].get()->Position().x - 50.0f), 50.0f, (GameObjects["Players_0" + to_string(i)].get()->Position().z));

	}


	//Tower
	GameObjects["Rifts_0" + to_string(0)].get()->objectLoader(ObjectPath + "Square.obj");
	GameObjects["Rifts_0" + to_string(0)].get()->setMaterial(NASDMaterial);
	GameObjects["Rifts_0" + to_string(0)].get()->Viewable = true;
	GameObjects["Rifts_0" + to_string(0)].get()->setMass(0.0f);
	GameObjects["Rifts_0" + to_string(0)].get()->setScale(glm::vec3(2.0f, 16.0f, 30.0f));
	GameObjects["Rifts_0" + to_string(0)].get()->setSizeOfHitBox(glm::vec3(2.0f, 24.0f, 30.0f));
	GameObjects["Rifts_0" + to_string(0)].get()->setPosition(glm::vec3(48.0f, 8.0f, 0.0f));
	GameObjects["Rifts_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Hexagons.png").c_str())));
	GameObjects["Rifts_0" + to_string(1)].get()->objectLoader(&GameObjects["Rifts_0" + to_string(0)]);
	GameObjects["Rifts_0" + to_string(1)].get()->setPosition(glm::vec3(-48.0f, 8.0f, 0.0f));
	GameObjects["Rifts_0" + to_string(1)].get()->setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
	GameObjects["Rifts_0" + to_string(1)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Hexagons.png").c_str())));



	GameObjects["Affects_0" + to_string(0)].get()->objectLoader(ObjectPath + "Player//dazzle.obj");
	GameObjects["Affects_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//dazzle.png").c_str())));
	GameObjects["Affects_0" + to_string(1)].get()->objectLoader(ObjectPath + "Player//bolt.obj");
	GameObjects["Affects_0" + to_string(1)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//bolt.png").c_str())));
	GameObjects["Affects_0" + to_string(2)].get()->objectLoader(ObjectPath + "Player//dazzle.obj");
	GameObjects["Affects_0" + to_string(2)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//dazzle.png").c_str())));
	GameObjects["Affects_0" + to_string(3)].get()->objectLoader(ObjectPath + "Player//spiral_01.obj");
	GameObjects["Affects_0" + to_string(3)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//swirl.png").c_str())));
	GameObjects["Affects_0" + to_string(4)].get()->objectLoader(ObjectPath + "Player//spiral_02.obj");
	GameObjects["Affects_0" + to_string(4)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Player//swirl.png").c_str())));
	//set player affect objects
	for (int i = 0; i < GameObjectsAmount["Affects_0"]; i++) {
		GameObjects["Affects_0" + to_string(i)].get()->setMaterial(NASDMaterial);
		GameObjects["Affects_0" + to_string(i)].get()->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
		GameObjects["Affects_0" + to_string(i)].get()->textureHandle_hasTransparency = true;
		GameObjects["Affects_0" + to_string(i)].get()->Viewable = false;
	}

	//Specials
	GameObjects["Specials_0" + to_string(0)].get()->objectLoader(ObjectPath + "Square.obj");
	GameObjects["Specials_0" + to_string(1)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Seeker_Swarm_Icon.obj");
	GameObjects["Specials_0" + to_string(2)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Toss-Up_Icon.obj");
	GameObjects["Specials_0" + to_string(3)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Health_Icon.obj");
	GameObjects["Specials_0" + to_string(4)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Boost_Icon.obj");
	GameObjects["Specials_0" + to_string(5)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Repulsor_Icon.obj");
	GameObjects["Specials_0" + to_string(6)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Short_Circuit_Icon.obj");
	GameObjects["Specials_0" + to_string(7)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Super_Shockwave_Icon.obj");
	GameObjects["Specials_0" + to_string(8)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Invincibility_Icon.obj");
	GameObjects["Specials_0" + to_string(9)].get()->objectLoader(ObjectPath + "PowerUp_Icons//Flipped_Icon.obj");
	for (int i = 1; i < GameObjectsAmount["Specials_0"]; i++) {
		GameObjects["Specials_0" + to_string(i)].get()->setMaterial(NASDMaterial);
		GameObjects["Specials_0" + to_string(i)].get()->setSizeOfHitBox(glm::vec3(2.0f, 1.0f, 2.0f));
		GameObjects["Specials_0" + to_string(i)].get()->setMass(1.0f);
		GameObjects["Specials_0" + to_string(i)].get()->setSpecialAttribute(i);
		GameObjects["Specials_0" + to_string(i)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups//Blue_background.png").c_str())));
		GameObjects["Specials_0" + to_string(i)].get()->Viewable = false;
		GameObjects["Specials_0" + to_string(i)].get()->WillRotateForward = false;
	}




	//Ground
	GameObjects["Objects_0" + to_string(0)].get()->objectLoader(ObjectPath + "Rift//Ground.obj");
	GameObjects["Objects_0" + to_string(0)].get()->setMaterial(NASDMaterial);
	GameObjects["Objects_0" + to_string(0)].get()->Viewable = true;
	GameObjects["Objects_0" + to_string(0)].get()->setMass(0.0f);
	GameObjects["Objects_0" + to_string(0)].get()->setSizeOfHitBox(glm::vec3(100.0f, 0.01f, 100.0f)); //HitBox
	GameObjects["Objects_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Ground_03.png").c_str())));
	
	//Walls Left
	GameObjects["Objects_0" + to_string(1)].get()->objectLoader(ObjectPath + "Rift//stadium.obj");
	GameObjects["Objects_0" + to_string(1)].get()->setMaterial(NASDMaterial);
	GameObjects["Objects_0" + to_string(1)].get()->Viewable = true;
	GameObjects["Objects_0" + to_string(1)].get()->setMass(0.0f);
	GameObjects["Objects_0" + to_string(1)].get()->setScale(glm::vec3(1.0f, 1.0f, 1.0f)); //size
	GameObjects["Objects_0" + to_string(1)].get()->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	GameObjects["Objects_0" + to_string(1)].get()->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
	GameObjects["Objects_0" + to_string(1)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Rockwall.png").c_str())));
	//Rift magnets
	GameObjects["Objects_0" + to_string(2)].get()->objectLoader(ObjectPath + "Rift//Magnet_Left.obj");
	GameObjects["Objects_0" + to_string(2)].get()->setMaterial(NASDMaterial);
	GameObjects["Objects_0" + to_string(2)].get()->Viewable = true;
	GameObjects["Objects_0" + to_string(2)].get()->setPosition(glm::vec3(53.0f, 08.0f, -20.0f));
	GameObjects["Objects_0" + to_string(2)].get()->setMass(0.0f);
	GameObjects["Objects_0" + to_string(2)].get()->setScale(glm::vec3(1.0f, 1.50f, 1.50f));
	GameObjects["Objects_0" + to_string(2)].get()->setSizeOfHitBox(glm::vec3(2.650f, 7.0f, 3.60f));
	GameObjects["Objects_0" + to_string(2)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Magnet_Rift_02.png").c_str())));
	GameObjects["Objects_0" + to_string(3)].get()->objectLoader(ObjectPath + "Rift//Magnet_Right.obj");
	GameObjects["Objects_0" + to_string(3)].get()->setMaterial(NASDMaterial);
	GameObjects["Objects_0" + to_string(3)].get()->Viewable = true;
	GameObjects["Objects_0" + to_string(3)].get()->setPosition(glm::vec3(53.0f, 08.0f, 20.0f));
	GameObjects["Objects_0" + to_string(3)].get()->setMass(0.0f);
	GameObjects["Objects_0" + to_string(3)].get()->setScale(glm::vec3(1.0f, 1.50f, 1.50f));
	GameObjects["Objects_0" + to_string(3)].get()->setSizeOfHitBox(glm::vec3(2.650f, 7.0f, 3.60f));
	GameObjects["Objects_0" + to_string(3)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Magnet_Rift_02.png").c_str())));
	GameObjects["Objects_0" + to_string(4)].get()->objectLoader(&GameObjects["Objects_0" + to_string(2)]);//left
	GameObjects["Objects_0" + to_string(4)].get()->setPosition(glm::vec3(-53.0f, 08.0f, 20.0f));
	GameObjects["Objects_0" + to_string(4)].get()->setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
	GameObjects["Objects_0" + to_string(4)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Magnet_Rift_01.png").c_str())));
	GameObjects["Objects_0" + to_string(5)].get()->objectLoader(&GameObjects["Objects_0" + to_string(3)]);//right
	GameObjects["Objects_0" + to_string(5)].get()->setPosition(glm::vec3(-53.0f, 08.0f, -20.0f));
	GameObjects["Objects_0" + to_string(5)].get()->setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
	GameObjects["Objects_0" + to_string(5)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//Magnet_Rift_01.png").c_str())));
	//crowd
	GameObjects["Objects_0" + to_string(6)].get()->objectLoader(ObjectPath + "Rift//crowd.obj");
	GameObjects["Objects_0" + to_string(6)].get()->setMaterial(NASDMaterial);
	GameObjects["Objects_0" + to_string(6)].get()->Viewable = true;
	GameObjects["Objects_0" + to_string(6)].get()->setMass(0.0f);
	GameObjects["Objects_0" + to_string(6)].get()->setScale(glm::vec3(1.0f, 1.0f, 1.0f)); //size
	GameObjects["Objects_0" + to_string(6)].get()->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	GameObjects["Objects_0" + to_string(6)].get()->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
	GameObjects["Objects_0" + to_string(6)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//crowd.png").c_str())));
	//banners
	GameObjects["Objects_0" + to_string(7)].get()->objectLoader(ObjectPath + "Rift//baners_01.obj");
	GameObjects["Objects_0" + to_string(7)].get()->setMaterial(NASDMaterial);
	GameObjects["Objects_0" + to_string(7)].get()->Viewable = true;
	GameObjects["Objects_0" + to_string(7)].get()->setMass(0.0f);
	GameObjects["Objects_0" + to_string(7)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "HUD//HUD_Display_B.png").c_str())));
	GameObjects["Objects_0" + to_string(8)].get()->objectLoader(ObjectPath + "Rift//baners_02.obj");
	GameObjects["Objects_0" + to_string(8)].get()->Viewable = true;
	GameObjects["Objects_0" + to_string(8)].get()->setMaterial(NASDMaterial);
	GameObjects["Objects_0" + to_string(8)].get()->setMass(0.0f);
	GameObjects["Objects_0" + to_string(8)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "HUD//HUD_Display_R.png").c_str())));
	//sky
	GameObjects["Objects_0" + to_string(9)].get()->objectLoader(ObjectPath + "Rift//Sky.obj");
	GameObjects["Objects_0" + to_string(9)].get()->setMaterial(NASDMaterial);
	GameObjects["Objects_0" + to_string(9)].get()->Viewable = true;
	GameObjects["Objects_0" + to_string(9)].get()->setMass(0.0f);
	GameObjects["Objects_0" + to_string(9)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rifts//sky.jpg").c_str())));


	//Enemies
	GameObjects["Enemies_0" + to_string(0)].get()->objectLoader(ObjectPath + "Enemies//Enemie_LP.obj");
	GameObjects["Enemies_0" + to_string(0)].get()->setMaterial(NASDMaterial);
	GameObjects["Enemies_0" + to_string(0)].get()->Viewable = true;
	GameObjects["Enemies_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Enemies//Enemy_02.png").c_str())));
	GameObjects["Enemies_0" + to_string(1)].get()->objectLoader(&GameObjects["Enemies_0" + to_string(0)]);
	GameObjects["Enemies_0" + to_string(1)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Enemies//Enemy_03.png").c_str())));

	for (int i = 2; i < GameObjectsAmount["Enemies_0"]; i++) {
		if (i % 2 == 0) { GameObjects["Enemies_0" + to_string(i)].get()->objectLoader(&GameObjects["Enemies_0" + to_string(0)]); }
		else { GameObjects["Enemies_0" + to_string(i)].get()->objectLoader(&GameObjects["Enemies_0" + to_string(1)]); }
	}
	
	GameObjects["Shadows_0" + to_string(0)].get()->objectLoader(ObjectPath + "PlainForShadow.obj");
	GameObjects["Shadows_0" + to_string(0)].get()->setMaterial(NASDMaterial);
	GameObjects["Shadows_0" + to_string(0)].get()->Viewable = true;
	GameObjects["Shadows_0" + to_string(0)].get()->setTexture(ilutGLLoadImage(_strdup((ImagePath + "Shadow_01.png").c_str())));
	GameObjects["Shadows_0" + to_string(0)].get()->textureHandle_hasTransparency = true;
	
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

	std::cout << std::endl << "_____________________________________";
	std::cout << std::endl << "[Fonts]";
	TextLoader loadTextFile;
	if (DoesFileExists("..//Assets")) { loadTextFile.objectLoader("..//Assets//file.txt"); }
	else if (DoesFileExists("Assets")) { loadTextFile.objectLoader("Assets//file.txt"); }
	else { std::cout << "[ERROR] Could not find [Font]" << std::endl; }
	if (DoesFileExists("..//Assets//Fonts//")) { SystemText.LoadTextFont("..//Assets//Fonts//FreeSerif.ttf", SystemText); }
	else if (DoesFileExists("Assets//Fonts//")) { SystemText.LoadTextFont("Assets//Fonts//FreeSerif.ttf", SystemText); }
	else { std::cout << "[ERROR] Could not find [Font]" << std::endl; }
	
	std::cout << std::endl << "[Values]";
	InitializeVariables();
	std::cout << std::endl << "[Shaders]";
	InitializeShaders();
	std::cout << std::endl << "[Menu]";
	InitializeTextPlane();
	std::cout << std::endl << "[Sounds]";
	InitializeSounds();
	std::cout << std::endl << "[HUD #]";
	InitializeNumbers();
	std::cout << std::endl << "[Game Objects]";
	InitializeObjects();
	std::cout << std::endl << "[Particles]";
	ParticleTexture = ilutGLLoadImage("Assets//Img//Particles//smoke_256_dm.png");
	InitializeParticles();
	std::cout << std::endl << "[FBO's]";
	InitializeFrameBufferObjects();
	

	randomSpecialTime = Random(10.0f, 30.0f);
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
	Sound::Sys.Init();
	// start the event handler
	glutMainLoop();

	return 0;
}



