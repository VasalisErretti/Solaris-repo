// Memory Leak Detection
#define _CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
		#pragma comment(lib, "DevIL.lib")
		#pragma comment(lib, "glew32.lib")
		#pragma comment(lib, "glut32.lib")
		#pragma comment(lib, "ILU.lib")
		#pragma comment(lib, "ILUT.lib")
		#pragma comment(lib, "XInput.lib")
		#pragma comment(lib, "Include/FMOD/fmod_vc.lib")
	#endif
#endif  
// _DEBUG
#include <stdlib.h>
#include <crtdbg.h>

// Core Libraries
#define WIN32_LEAN_AND_MEAN //cmd
#include <Windows.h> //cmd
#include <iostream>
#include <string>
#include <math.h>
#include <vector>

// 3rd Party Libraries
#include <GL\glew.h>
#include <GLUT\glut.h>
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
#include "ExtraFunctions.cpp"
#include "HTRLoader.h"
#include "FMOD/SoundEngine.h"

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
const int NumberOfPlayers = 2; GameObject Players[NumberOfPlayers];
GameObject ShockWaves[NumberOfPlayers]; GameObject Rifts[NumberOfPlayers];
const int NumberOfObjects = 13; GameObject Objects[NumberOfObjects];
const int NumberOfSpecials = 5; GameObject Specials[NumberOfSpecials];
const int NumberOfEnemies = 12; GameObject Enemies[12];
GameObject ShadowObject[2];
//Menu Screens
const int NumberOfPlaneForText = 5; GameObject planeForText[NumberOfPlaneForText];
//Sliders
const int NumberOfSliders = 10; GameObject planeForSliders[NumberOfSliders]; GameObject ButtonForSliders[NumberOfSliders];
Sliders Slider[NumberOfSliders];
//Buttons
const int NumberOfButtons = 7; GameObject ButtonObjects[NumberOfButtons];
Buttons Button[NumberOfButtons];
//Boarders
const int NumberOfBorders = 1; GameObject Borders[NumberOfBorders];

//Random Variables//

//camera viewport
int cameralook = 0; int cameraMode = 0;
glm::vec3 cameraPosition[2];
glm::vec3 forwardVector[2]{ glm::vec3(-1.0f, 0.0f, 0.0f) , glm::vec3(1.0f, 0.0f, 0.0f) };
glm::vec3 rightVector;
//player health
int Health[2]{40,40};
//Bool's for the game///////////////////////////
bool inMenu = true; bool inGame = false;
bool inOptions = false; int inOptionsTab = 0;
std::string lastMenu = "inMenu"; float MenuSwitchCounter[2] = { 0.0f, 0.0f };
const bool ApplyingGravity = true;
const bool CollisionBetweenObjects = true;
const bool EnemiesSeekTowers = true; int EnemiesSeekingTower = -1; float EnemiesSeekTowerLength = 6.0f; float EnemiesSeekTowerCounter = 0.0f;
const bool IdleEnemiesRespawn = true;
const bool EnableShadows = true;
//Shock wave attributes
bool Right_TRIGGERED[2]; bool Left_TRIGGERED[2]; bool speedControlSW = false;
bool AButtonDown = false; bool ShockWaveOn = true;
bool PShockWave[2] = { false }; float PShockWaveCounter[2] = { 0.0f }; float PShockWaveChargeUp[2] = { 0.0f };
//Sprint attributes
float PSprintCounter[2] = { 0.0f }; float PSprintCoolDown = 2.0f; 
float SprintSpeed = 1.5f;
glm::vec3 speedToWallDegradation(0.80f, 0.50f, 0.80f);
//Gamepad controls
Gamepad gamepad;
MorphMath morphmath;
//Sounds
Sound systemSound;
Sound drum[2];
//Text
RenderText SystemText;

//////////////////////////////////////////////////////////////////////

/* function setBoardStart()
* Description:
*  - this sets enemies and players positions
*  - set player health
*/
void setBoardStart() {
	Manifold m;

	Players[0].setPosition(glm::vec3(20.0f, 0.0f, 0.0f));
	Players[0].setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	Players[1].setPosition(glm::vec3(-20.0f, 0.0f, 0.0f));
	Players[1].setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	Health[0] = (Slider[2].SNob_Precent.x);
	Health[1] = (Slider[2].SNob_Precent.x);
	m.A = Enemies[0];
	for (int i = 0; i < NumberOfEnemies; i++)
	{
		m.B = Enemies[i];
		setEnemySpawn(m, i);
		Enemies[i] = m.B;
	}

	m.~Manifold();
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

//////////////////////////////////////////////////////////////////////

void DrawTextOnScreen(int iNum)
{
	shader->bind();
	cameralook = iNum; //window
	WhatCameraIsLookingAt(); //Resising Window
	// Draw our scene
	shader->uniformMat4x4("mvm", &modelViewMatrix[2]);
	shader->uniformMat4x4("prm", &projectionMatrix[2]);
	//shader->uniformFloat("dispNormals", dispNormals);
	shader->uniformVector("lightPosition_01", &lightPosition_01);
	shader->uniformVector("lightPosition_02", &lightPosition_02);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	shader->uniformTex("tex1", textureHandle, 0);

	//no longer works
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	// compute local transformation
	glm::mat4x4 scaleMatrix(1.0f);
	glm::mat4x4 rotationX = glm::rotate(0.0f, glm::vec3(1.0, 0.0, 0.0));
	glm::mat4x4 rotationY = glm::rotate(0.0f, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4x4 rotationZ = glm::rotate(0.0f, glm::vec3(0.0, 0.0, 1.0));
	glm::mat4x4 rotationMatrix = (rotationZ * rotationY * rotationX);
	glm::mat4x4 translationMatrix = glm::translate(glm::vec3(20.0, 0.0, -10.0));


	glm::mat4x4 transform = translationMatrix * rotationMatrix * scaleMatrix;
	shader->uniformMat4x4("localTransform", &transform);

	writeSomething(1.0f, 1.0f, 1.0f, "Health:" + std::to_string(Health[iNum]));

	shader->unbind();
}

/* function InMenuDraw()
* Description:
*  - Draws the menu screen
*/
void InMenuDraw(int Inum)
{
	shader->bind();
	Inum = 2;
	cameralook = Inum; //window
	WhatCameraIsLookingAt(); //Resising Window

	shader->uniformMat4x4("mvm", &modelViewMatrix[Inum]);
	shader->uniformMat4x4("prm", &projectionMatrix[Inum]);
	shader->uniformMat4x4("u_mvp", &(modelViewMatrix[Inum] * projectionMatrix[Inum]));
	shader->uniformMat4x4("u_mv", &modelViewMatrix[Inum]);
	shader->uniformMat4x4("u_lightPos_01", &(modelViewMatrix[Inum] * glm::translate(glm::mat4(1.0f), lightPosition_01)));
	shader->uniformMat4x4("u_lightPos_02", &(modelViewMatrix[Inum] * glm::translate(glm::mat4(1.0f), lightPosition_02)));

	if (planeForText[1].Viewable) { planeForText[1].drawObject(shader); }

	for (unsigned int i = 0; i <= 2; i++) {
		if (ButtonObjects[i].Viewable) { ButtonObjects[i].drawObject(shader); }
	}

	shader->unbind();
}

/* function MenuScreen()
* Description:
*  - does all the functions/calculations for the menu screen
*/
void MenuScreen(float deltaTasSeconds)
{
	//drum[0].play();
	systemSound.systemUpdate();
	if (mouseDown[0]) {
		mouseDown[0] = false;
		if (Button[0].button(MPosToOPosX, MPosToOPosY)) { setBoardStart(); inGame = true; inMenu = false; }
		if (Button[1].button(MPosToOPosX, MPosToOPosY)) { inOptions = true; inMenu = false; }
		if (Button[2].button(MPosToOPosX, MPosToOPosY)) { exit(0); }
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
	shader->bind();
	Inum = 2;
	cameralook = Inum; //window
	WhatCameraIsLookingAt(); //Resising Window

	// Draw our scene
	shader->uniformMat4x4("mvm", &modelViewMatrix[Inum]);
	shader->uniformMat4x4("prm", &projectionMatrix[Inum]);
	shader->uniformMat4x4("u_mvp", &(modelViewMatrix[Inum] * projectionMatrix[Inum]));
	shader->uniformMat4x4("u_mv", &modelViewMatrix[Inum]);
	shader->uniformMat4x4("u_lightPos_01", &(modelViewMatrix[Inum] * glm::translate(glm::mat4(1.0f), lightPosition_01)));
	shader->uniformMat4x4("u_lightPos_02", &(modelViewMatrix[Inum] * glm::translate(glm::mat4(1.0f), lightPosition_02)));

	


	if (inOptionsTab == 0) {
		if (planeForText[inOptionsTab + 1].Viewable) { planeForText[inOptionsTab + 1].drawObject(shader); }
		if (ButtonObjects[3].Viewable) { ButtonObjects[3].drawObject(shader); }
		for (unsigned int i = 4; i <= 6; i++) {
			if (ButtonObjects[i].Viewable) { ButtonObjects[i].drawObject(shader); }
		}
	}
	else if (inOptionsTab == 1) {
		if (planeForText[inOptionsTab + 1].Viewable) { planeForText[inOptionsTab + 1].drawObject(shader); }
		if (ButtonObjects[3].Viewable) { ButtonObjects[3].drawObject(shader); }
	}
	else if (inOptionsTab == 2) {
		if (planeForText[inOptionsTab + 1].Viewable) { planeForText[inOptionsTab + 1].drawObject(shader); }
		if (ButtonObjects[3].Viewable) { ButtonObjects[3].drawObject(shader); }
	}
	else if (inOptionsTab == 3) {
		if (planeForText[inOptionsTab + 1].Viewable) { planeForText[inOptionsTab + 1].drawObject(shader); }
		if (ButtonObjects[3].Viewable) { ButtonObjects[3].drawObject(shader); }
		for (unsigned int i = 0; i < NumberOfSliders; i++) {
			if (ButtonForSliders[i].Viewable) { ButtonForSliders[i].drawObject(shader); }
			if (planeForSliders[i].Viewable) { planeForSliders[i].drawObject(shader); }
		}
	}

	shader->unbind();
	//SystemText.TextDraw(*TextShader, &projectionMatrix[3], "Text", X, Y, Size, Colour, Right_Middle_Left alinment);
	//SystemText.TextDraw(*TextShader, &projectionMatrix[3], "[0,0]", 0.0f, 0.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), 1);

}

/* function OptionScreen()
* Description:
*  - does all the functions/calculations for the options screen
*/
void OptionScreen(float deltaTasSeconds)
{
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
		for (int i = NumberOfEnemies; i > (NumberOfEnemies - static_cast<int>(Slider[1].SNob_Precent.x / 100)); i--) {
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
	shader->bind();
	cameralook = Inum; //window
	WhatCameraIsLookingAt(); //Resising Window
	
	//Draw scene//cameraViewMatrix //modelViewMatrix
	shader->uniformMat4x4("mvm", &modelViewMatrix[Inum]);
	shader->uniformMat4x4("prm", &projectionMatrix[Inum]);
	shader->uniformMat4x4("u_mvp", &(modelViewMatrix[Inum] * projectionMatrix[Inum]));
	shader->uniformMat4x4("u_mv", &modelViewMatrix[Inum]);
	shader->uniformMat4x4("u_lightPos_01", &(modelViewMatrix[Inum]*glm::translate(glm::mat4(1.0f), lightPosition_01)));
	shader->uniformMat4x4("u_lightPos_02", &(modelViewMatrix[Inum]*glm::translate(glm::mat4(1.0f), lightPosition_02)));




	for (unsigned int i = 0; i < NumberOfPlayers; i++) {
		if (Players[i].Viewable) {
			//Players
			if (Players[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Players[i].drawObject(shader);
			//Shadows
			if (EnableShadows) {
				if (ShadowObject[0].textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				ShadowObject[0].setPosition(glm::vec3(Players[i].Position().x, 0.01f, Players[i].Position().z));
				ShadowObject[0].setScale(Players[i].Scale());
				ShadowObject[0].setRotation(Players[i].Angle());
				ShadowObject[0].drawObject(shader);
			}
			//ShockWaves
			if (ShockWaveOn) {
				if (ShockWaves[i].textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				ShockWaves[i].drawObject(shader);
			}
			//Rifts
			if (Rifts[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Rifts[i].drawObject(shader);

			//Players[i].drawHTR(shader);
		}
	}
	for (unsigned int i = 0; i < NumberOfObjects; i++) {
		if (Objects[i].Viewable) {
			if (Objects[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Objects[i].drawObject(shader);
		}
	}
	for (unsigned int i = 0; i < NumberOfEnemies; i++) {
		//if (NumberOfEnemies > (sizeof(Enemies) / sizeof(Enemies[0]))) {
		//	for (unsigned int i = (sizeof(Enemies) / sizeof(Enemies[0])); i < NumberOfEnemies; i++) {
		//		Enemies[i].objectLoader(&Enemies[0]);
		//		std::cout << "[" << i << "] [created more enemies]" << std::endl;
		//	}
		//	NumberOfEnemies = (sizeof(Enemies) / sizeof(Enemies[0]));
		//}
		if (Enemies[i].Viewable) {
			//Enemies
			if (Enemies[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Enemies[i].drawObject(shader);
			//Shadows
			if (EnableShadows) {
				if (ShadowObject[0].textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				ShadowObject[0].setPosition(glm::vec3(Enemies[i].Position().x, 0.01f, Enemies[i].Position().z));
				ShadowObject[0].setScale(Enemies[i].Scale());
				ShadowObject[0].setRotation(Enemies[i].Angle());
				ShadowObject[0].drawObject(shader);
			}
		}
	}
	for (unsigned int i = 0; i < NumberOfSpecials; i++) {
		if (Specials[i].Viewable) {
			//Specials
			if (Specials[i].textureHandle_hasTransparency == true) { disableCulling(); }
			else { enableCulling(); }
			Specials[i].drawObject(shader);
			//Shadows
			if (EnableShadows) {
				if (ShadowObject[1].textureHandle_hasTransparency == true) { disableCulling(); }
				else { enableCulling(); }
				ShadowObject[1].setPosition(glm::vec3(Specials[i].Position().x, 0.01f, Specials[i].Position().z));
				ShadowObject[1].setScale(Specials[i].Scale());
				ShadowObject[1].setRotation(Specials[i].Angle());
				ShadowObject[1].drawObject(shader);
			}
		}
	}

	shader->unbind();

	cameralook = 3; //window
	WhatCameraIsLookingAt(); //Resising Window

	if (Inum == 0) {
		SystemText.TextDraw(*TextShader, &projectionMatrix[3], "[" + std::to_string(Health[0]) + "]", -(windowWidth / 4), -(windowHeight / 4), 1.0f, glm::vec3(0.8, 0.2f, 0.2f), 1);
		SystemText.TextDraw(*TextShader, &projectionMatrix[3], "[" + std::to_string(Health[1]) + "]", -(windowWidth / 4), (windowHeight / 2) - (windowHeight / 4), 1.0f, glm::vec3(0.8, 0.2f, 0.2f), 1);
	}
	else if (Inum == 1) {
		SystemText.TextDraw(*TextShader, &projectionMatrix[3], "[" + std::to_string(Health[0]) + "]",  (windowWidth / 4), (windowHeight / 2) - (windowHeight / 4), 1.0f, glm::vec3(0.8, 0.2f, 0.2f), 1);
		SystemText.TextDraw(*TextShader, &projectionMatrix[3], "[" + std::to_string(Health[1]) + "]", (windowWidth / 4), -(windowHeight / 4), 1.0f, glm::vec3(0.8, 0.2f, 0.2f), 1);
	}
}

/* function GameField()
* Description:
*  - does all the functions/calculations for the game screen
*/
void GameField(float deltaTasSeconds) 
{

	Manifold m;


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



							m.A = Borders[boxNumber_2]; m.B = Players[i]; std::cout << "[Double border collid to bigger box] \n"; 
						}
						else {
							if (CheckCollision(m)) { m.A = Borders[boxNumber_1]; m.B = Players[i]; std::cout << "[In border collid to nearest box] \n"; }
							else { m.A = Borders[boxNumber_2]; m.B = Players[i]; std::cout << "[In border collid to bigger box] \n"; }
						}
					}
					else if (CheckCollision(m)) { m.A = Borders[boxNumber_2]; m.B = Players[i]; std::cout << "[Collid to bigger box] \n"; }
					else { m.A = Borders[boxNumber_1]; m.B = Players[i]; std::cout << "[Collid to nearest box] \n"; }
				}
				else { m.A = Borders[boxNumber_1]; m.B = Players[i]; std::cout << "[Collid to only box] \n"; }

				


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
					else if (m.B.SpecialAttribute() == 1) {
						m.B.Viewable = false;
						if (i == 0) { EnemiesSeekingTower = 1; }
						else if (i == 1) { EnemiesSeekingTower = 0; }
						else { EnemiesSeekingTower = -1; }
						EnemiesSeekTowerCounter = 0.0f;
					}
					else if (m.B.SpecialAttribute() == 2) {
						m.B.Viewable = false;
						for (int ij = 0; ij < NumberOfEnemies; ij++) {
							float ranPosY = (rand() % 1000 + 100); //100 to 1100
							Enemies[ij].setForceOnObject(Enemies[ij].ForceOnObject() + glm::vec3(0.0f, ranPosY, 0.0f));
						}
					}
					else if (m.B.SpecialAttribute() == 3) {
						m.B.Viewable = false;
						Health[i] += 10; 
					}
					else if (m.B.SpecialAttribute() == 4) {}
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
				if (CheckCollision(m)) { ResolveCollision(m, 1.0f); applySeekSystem(m, -0.50f); }
				Enemies[j] = m.B;
			}//end for
			//Players[i] = m.A;
		}//end for

		 //Enemies to Rifts
		for (int i = 0; i < NumberOfEnemies; i++) {
			m.B = Enemies[i];
			for (int j = 0; j < NumberOfPlayers; j++) {
				m.A = Rifts[j];
				if (ObjectsWithinRange(m, 15.0f)) { applySeekSystem(m, 2.0f); }
				if (CheckCollision(m)) {
					//Tower One
					if (j == 0) {
						setEnemySpawn(m,i);
						Health[0] -= 1;
					}
					//Tower Two
					else if (j == 1) {
						setEnemySpawn(m,i);
						Health[1] -= 1;
					}
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
				if (checkRadialCollision(m)) { ResolveCollision(m, 01.0f); }
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
				if (checkRadialCollision(m)) { ResolveCollision(m, 2.0f, 0.01f); }
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
					if (checkRadialCollision(m)) { ResolveCollision(m, 0.50f); }
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
				////If players are nearing the walls
				//if (m.B.Position().x > (m.A.Top().x*0.95)) { FleeFromDirection(m, 1.0f, "x"); }
				//else if (m.B.Position().x < (m.A.Bottom().x*0.95)) { FleeFromDirection(m, 1.0f, "-x"); }
				//if (m.B.Position().z > (m.A.Top().z*0.95)) { FleeFromDirection(m, 1.0f, "z"); }
				//else if (m.B.Position().z < (m.A.Bottom().z*0.95)) { FleeFromDirection(m, 1.0f, "-z"); }
				Enemies[i] = m.B;
			}
		}
	}

	//Applying Gravity to everything
	if (ApplyingGravity) {
		m.A = Objects[0];
		//apply gravity relative to object[0]
		for (int i = 0; i < NumberOfPlayers; i++) { m.B = Players[i]; applyGravitationalForces(m, 0.0f); Players[i] = m.B; }
		for (int i = 0; i < NumberOfEnemies; i++) { m.B = Enemies[i]; applyGravitationalForces(m, -1.0f); Enemies[i] = m.B; }
		for (int i = 0; i < NumberOfSpecials; i++) { m.B = Specials[i]; applyGravitationalForces(m, -1.0f); Specials[i] = m.B; }
	}

	//Enemies seek towers
	if (EnemiesSeekTowers && EnemiesSeekingTower >= 0 && EnemiesSeekTowerCounter < EnemiesSeekTowerLength) {
		EnemiesSeekTowerCounter += deltaTasSeconds;
		std::cout << "	[S]("<< EnemiesSeekingTower<<")[" << EnemiesSeekTowerCounter << "]" << std::endl;
		for (int j = 0; j < NumberOfEnemies; j++) {
			//Seek towards Player Ones tower
			if (EnemiesSeekingTower == 0) {
				m.A = Rifts[0];
				m.B = Enemies[j];
				applySeekSystem(m, 3.0f);
				m.B.setColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				Enemies[j] = m.B;
			}
			//Seek towards Player Twos tower
			else if (EnemiesSeekingTower == 1) {
				m.A = Rifts[1];
				m.B = Enemies[j];
				applySeekSystem(m, 3.0f);
				m.B.setColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
				Enemies[j] = m.B;
			}
		}
	}

	//Can the users use the shock wave ability
	if (ShockWaveOn) {
		for (int i = 0; i < NumberOfPlayers; i++) {
			if (PShockWave[i]) {
				ShockWaves[i].Viewable = true;
				if (PShockWaveCounter[i] > 0.0f) { PShockWaveCounter[i] -= deltaTasSeconds; }
				else { PShockWave[i] = false; PShockWaveChargeUp[i] = 0.0f; }

				float ForceWeight;
				float ForceWeight_Addition = 70.0f + ((2.4f * (0.80f + PShockWaveChargeUp[i]))*(2.4f * (0.80f + PShockWaveChargeUp[i]))*(2.4f * (0.80f + PShockWaveChargeUp[i])));
				float ForceWeight_Minius = ((1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i])));

				if (1000.0f <= ForceWeight_Addition - (ForceWeight_Addition / 2)) { ForceWeight = 1000.0f; }
				else if (ForceWeight_Minius < (ForceWeight_Addition / 2)) { ForceWeight = ForceWeight_Addition - ForceWeight_Minius; }

				float ForceModifier = -(40.0f + 1.5*ForceWeight);

				glm::vec3 sizeofShockWave;
				if (speedControlSW) { sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f)), 100.0f, PShockWaveCounter[i]); }
				else { sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f)), PShockWaveCounter[i]); }

				ShockWaves[i].setScale(glm::vec3(sizeofShockWave.x, 01.0f + (0.50f*PShockWaveChargeUp[i]), sizeofShockWave.z));
				ShockWaves[i].setRotation(glm::vec3(0.0f, PShockWaveChargeUp[i], 0.0f));
				ShockWaves[i].setSizeOfHitBox(sizeofShockWave);

				m.A = ShockWaves[i];
				//collision to Enemies
				for (int j = 0; j < NumberOfEnemies; j++) {
					m.B = Enemies[j];
					if (checkRadialCollision(m)) { applySeekSystem(m, ForceModifier); ResolveCollision(m, 1.0f); m.B.inShock = true; }
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
				ShockWaves[i].Viewable = true;
				float ForceWeight;
				float ForceWeight_Addition = 70.0f + ((2.4f * (0.80f + PShockWaveChargeUp[i]))*(2.4f * (0.80f + PShockWaveChargeUp[i]))*(2.4f * (0.80f + PShockWaveChargeUp[i])));
				float ForceWeight_Minius = ((1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i]))*(1.1f * (0.60f + PShockWaveChargeUp[i])));
				
				if (1000.0f <= ForceWeight_Addition - (ForceWeight_Addition / 2)) { ForceWeight = 1000.0f; }
				else if (ForceWeight_Minius < (ForceWeight_Addition / 2)) { ForceWeight = ForceWeight_Addition - ForceWeight_Minius; }

				glm::vec3 sizeofShockWave;
				if (speedControlSW) { sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f)), 100.0f, 0.25f); }
				else { sizeofShockWave = morphmath.Lerp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f), 5.0f + (ForceWeight*0.1f)), 0.25f); }
				
				sizeofShockWave.y = 01.0f;
				ShockWaves[i].setScale(glm::vec3(sizeofShockWave.x, 01.0f + (0.50f*PShockWaveChargeUp[i]), sizeofShockWave.z));
				ShockWaves[i].setRotation(glm::vec3(0.0f, PShockWaveChargeUp[i], 0.0f));
			}
			else { ShockWaves[i].Viewable = false; }
		}//end for
	}

	//Idle Enemies will despawn then respawn
	if (IdleEnemiesRespawn) {
		m.A = Objects[0];
		for (int j = 0; j < NumberOfEnemies; j++)
		{
			m.B = Enemies[j];
			float ranPosj = rand() % 80 - 40;
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


	//Updating Targets
	for (int i = 0; i < NumberOfPlayers; i++) { Players[i].updateP(deltaTasSeconds); }
	for (int i = 0; i < NumberOfEnemies; i++) { Enemies[i].update(deltaTasSeconds); }
	for (int i = 0; i < NumberOfSpecials; i++){Specials[i].update(deltaTasSeconds); }

	m.~Manifold();
}


/* function debug()
* Description:
*   - this is called to check certain things for the reasons to debug the program
*/
void ControllerDelayButton(int portNumber,float deltaTasSeconds)
{
	float MovementModifier = 12.0f;
	portNumber = portNumber - 1;
	if (inGame) {
		if (cameraMode == 0) {
			//JoySticks
			if (portNumber == 0) {
				float Tx = 0.0f; float Ty = 0.0f; float Tz = 0.0f; float rotY = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
				if (gamepad.leftStickY < -0.1) { Tx -= gamepad.leftStickY * 0.0666666f; } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
				if (gamepad.leftStickY > 00.1) { Tx -= gamepad.leftStickY * 0.0666666f; } //Y-Up
				if (gamepad.leftStickX < -0.1) { Tz -= gamepad.leftStickX * 0.0666666f; } //X-Left
				if (gamepad.leftStickX > 00.1) { Tz -= gamepad.leftStickX * 0.0666666f; } //X-Right

				if (Tx > 00.055f) { Tx = 00.055f; } else if (Tx < -0.055f) { Tx = -0.055f; }
				if (Tz > 00.055f) { Tz = 00.055f; } else if (Tz < -0.055f) { Tz = -0.055f; }
				Tx = Tx * 1.25f; Ty = Ty * 1.25f; Tz = Tz * 1.25f;

				if (gamepad.rightStickX > 00.1) { rotY -= gamepad.rightStickX * 0.0666666f; }
				if (gamepad.rightStickX < -0.1) { rotY -= gamepad.rightStickX * 0.0666666f; }

				Tx = ((Tx*MovementModifier*0.750f));//(Tx*MovementModifier)*(Tx*MovementModifier));
				Ty = ((Ty*MovementModifier*0.750f));//(Ty*MovementModifier)*(Ty*MovementModifier));
				Tz = ((Tz*MovementModifier*0.750f));//(Tz*MovementModifier)*(Tz*MovementModifier));

				Players[portNumber].setForceOnObject(glm::vec3(Tx, Ty, Tz));
				Players[portNumber].setVelocity(glm::vec3(Tx, Ty, Tz));
				Players[portNumber].ForwardDirection = (Players[portNumber].Position() - (Players[portNumber].Position() + Players[portNumber].Velocity()));
				ShockWaves[portNumber].setPosition(Players[portNumber].Position());
			}
			if (portNumber == 1) {
				float Tx = 0.0f; float Ty = 0.0f; float Tz = 0.0f; float rotY = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
				if (gamepad.leftStickY < -0.1) { Tx += gamepad.leftStickY * 0.0666666f; } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
				if (gamepad.leftStickY > 00.1) { Tx += gamepad.leftStickY * 0.0666666f; } //Y-Up
				if (gamepad.leftStickX < -0.1) { Tz += gamepad.leftStickX * 0.0666666f; } //X-Left
				if (gamepad.leftStickX > 00.1) { Tz += gamepad.leftStickX * 0.0666666f; } //X-Right

				if (Tx > 00.055f) { Tx = 00.055f; } else if (Tx < -0.055f) { Tx = -0.055f; }
				if (Tz > 00.055f) { Tz = 00.055f; } else if (Tz < -0.055f) { Tz = -0.055f; }
				Tx = Tx * 1.25f; Ty = Ty * 1.25f; Tz = Tz * 1.25f;

				if (gamepad.rightStickX > 0.1) { rotY -= gamepad.rightStickX * 0.0666666f; }
				if (gamepad.rightStickX < -0.1) { rotY -= gamepad.rightStickX * 0.0666666f; }

				Tx = ((Tx*MovementModifier*0.750f));//(Tx*MovementModifier)*(Tx*MovementModifier));
				Ty = ((Ty*MovementModifier*0.750f));//(Ty*MovementModifier)*(Ty*MovementModifier));
				Tz = ((Tz*MovementModifier*0.750f));//(Tz*MovementModifier)*(Tz*MovementModifier));

				Players[portNumber].setForceOnObject(glm::vec3(Tx, Ty, Tz));
				Players[portNumber].setVelocity(glm::vec3(Tx, Ty, Tz));
				Players[portNumber].ForwardDirection = (Players[portNumber].Position() - (Players[portNumber].Position() + Players[portNumber].Velocity()));
				ShockWaves[portNumber].setPosition(Players[portNumber].Position());
			}
		}
		else if (cameraMode == 1) {
			if (portNumber >= 0) {
				float Tmovement = 0.0f; float yaw = 0.0f; float pitch = 0.0f;
				//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
				if (gamepad.leftStickY < -0.1) { Tmovement = (MovementModifier * (gamepad.leftStickY * 0.0666666f)); } //Y-Down // divide by [15.0f] or multiplay by [0.0666666f]
				if (gamepad.leftStickY > 00.1) { Tmovement = (MovementModifier * (gamepad.leftStickY * 0.0666666f)); } //Y-Up
				if (gamepad.leftStickX < -0.1) { yaw = (gamepad.leftStickX * -0.0666666f); } //X-Left
				if (gamepad.leftStickX > 00.1) { yaw = (gamepad.leftStickX * -0.0666666f); } //X-Right

				if (gamepad.rightStickY > 00.1) { pitch = gamepad.rightStickY * 0.0666666f; }
				if (gamepad.rightStickY < -0.1) { pitch = gamepad.rightStickY * 0.0666666f; }
				if (gamepad.rightStickX < -0.1) { yaw = (gamepad.rightStickX * -0.0666666f); } //X-Left
				if (gamepad.rightStickX > 00.1) { yaw = (gamepad.rightStickX * -0.0666666f); } //X-Right


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
				Players[portNumber].setVelocity((Tmovement * forwardVectorTemp)*0.5f);
				//camera position
				cameraPosition[portNumber] = glm::vec3(Players[portNumber].Position().x - (forwardVector[portNumber].x*6.0f),
					Players[portNumber].Position().y + (Players[portNumber].Radius().y*2.0f),
					Players[portNumber].Position().z - (forwardVector[portNumber].z*6.0f));
				//shockwace position
				ShockWaves[portNumber].setPosition(Players[portNumber].Position());
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
				//Players[portNumber].update(deltaTasSeconds);
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
			
			mousepositionX -= Tx; mousepositionY += Ty;
			//
			//std::cout << "[" << screen_pos_x << "] [" << screen_pos_y << "]" << std::endl;
			if (Tx != 0.0f || Ty != 0.0f || Tz != 0.0f) { SetCursorPos(glutGet((GLenum)GLUT_WINDOW_X)+mousepositionX, glutGet((GLenum)GLUT_WINDOW_Y)+mousepositionY); }
		
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

					bool pressedA = false;
					for (unsigned int i = 0; i < NumberOfSliders; i++) {
						//move nob along the slider
						if (Slider[i].moveNob(MPosToOPosX, MPosToOPosY)) { ButtonForSliders[i].setPosition(glm::vec3(MPosToOPosX, 0.02f, Slider[i].SBar_Pos.z)); pressedA = true; }
					}
					if (!pressedA) { MenuSwitchCounter[portNumber] = 0.50f; }
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
					exit(0);
				}
				if (gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)) { std::cout << "[LEFT_SHOULDER]"; }
				if (gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)) { std::cout << "[RIGHT_SHOULDER]"; }
			}
		}
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
		case 27: 
			{ exit(0); }// the escape key
			 break;
		case ' ':
			{
				setBoardStart();
				inMenu = false; inGame = true;
			}
			break;
		case '+':
		case '=':
			TestFloat += TestFloatIncrementAmount;
			std::cout << "[[" << TestFloat << "]]" << std::endl;
			ButtonForSliders[0].setRotation(glm::vec3(0.0f, (degToRad*TestFloat + 0.0f), 0.0f));
			break;
		case '_':
		case '-':
			TestFloat -= TestFloatIncrementAmount;
			std::cout << "[[" << TestFloat << "]]" << std::endl;
			ButtonForSliders[0].setRotation(glm::vec3(0.0f, (degToRad*TestFloat + 0.0f), 0.0f));
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
			break;
		case 'R':
		case 'r':
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
		case 'V':
		case 'v':
			if (cameraMode == 0) { cameraMode = 1; }
			else if (cameraMode == 1) { cameraMode = 0; }
			break;
		case '1': 
			Specials[1].Viewable = true; Specials[1].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
				  break;
		case '2': 
			Specials[2].Viewable = true; Specials[2].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
				  break;
		case '3': 
			Specials[3].Viewable = true; Specials[3].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
				  break;
		case '4': 
			Specials[4].Viewable = true; Specials[4].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
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
		//player one
		gamepad.SetActivePort(1); gamepad.Refresh();
		ControllerDelayButton(gamepad.GetActivePort(), deltaTasSeconds);	
		//player two
		gamepad.SetActivePort(2); gamepad.Refresh();
		ControllerDelayButton(gamepad.GetActivePort(), deltaTasSeconds);
	} 
	else {
		if (cameraMode == 0) {
			float Tx[2]{ 0.0f,0.0f }; float Ty[2]{ 0.0f,0.0f }; float Tz[2]{ 0.0f,0.0f };

			if (MenuSwitchCounter[0] > 0.0f) { MenuSwitchCounter[0] -= deltaTasSeconds; }
			else {
				//player One
				if (keyDown['w'] || keyDown['W']) { Tx[0] = -0.20f; }
				if (keyDown['s'] || keyDown['S']) { Tx[0] = 00.20f; }
				if (keyDown['a'] || keyDown['A']) { Tz[0] = 00.20f; }
				if (keyDown['d'] || keyDown['D']) { Tz[0] = -0.20f; }
				if (keyDown['q'] || keyDown['Q']) {
					if (Players[0].inAir == false && Players[0].IsJumping == false) {
						Players[0].inAir = true; Players[0].IsJumping = true;
						Players[0].onObject = false;
						Players[0].InAirCounter = 0.25f;
						Players[0].setPosition(glm::vec3(Players[0].Position().x, Players[0].Position().y + (Players[0].Radius().y * 0.30f), Players[0].Position().z));
					}
				}
				//first press of [RIGHT_TRIGGERED]
				if ((keyDown['e'] || keyDown['E']) && Right_TRIGGERED[0] == false) { Right_TRIGGERED[0] = true; std::cout << "[RIGHT_TRIGGERED][-]"; }
				//[RIGHT_TRIGGERED] was pressed last tic
				else if (Right_TRIGGERED[0] == true) {
					//holding [RIGHT_TRIGGERED]
					if (keyDown['e'] || keyDown['E']) {
						PShockWaveChargeUp[0] += deltaTasSeconds;
						//std::cout << "	[C](" << 0 << ")[" << PShockWaveChargeUp[0] << "]" << std::endl;
					}
					//[RIGHT_TRIGGERED] released
					else {
						if (PShockWaveChargeUp[0] < 0.50f) { PShockWaveChargeUp[0] = 0.0f; }
						Right_TRIGGERED[0] = false;
						PShockWave[0] = true;
						PShockWaveCounter[0] = 0.250f;
						MenuSwitchCounter[0] = 0.60f;
						std::cout << "[RIGHT_TRIGGERED][+]";
					}
					Players[0].setVelocity(Players[0].Velocity()*0.5f);
				}

				Players[0].setForceOnObject(glm::vec3(Tx[0], Ty[0], Tz[0]));
				Players[0].setVelocity(glm::vec3(Tx[0], Ty[0], Tz[0]));
				Players[0].ForwardDirection = (Players[0].Position() - (Players[0].Position() + Players[0].Velocity()));
				ShockWaves[0].setPosition(Players[0].Position());
			}

			if (MenuSwitchCounter[1] > 0.0f) { MenuSwitchCounter[1] -= deltaTasSeconds; }
			else {
				//player Two
				if (keyDown['i'] || keyDown['I']) { Tx[1] = 00.20f; }
				if (keyDown['k'] || keyDown['K']) { Tx[1] = -0.20f; }
				if (keyDown['j'] || keyDown['J']) { Tz[1] = -0.20f; }
				if (keyDown['l'] || keyDown['L']) { Tz[1] = 00.20f; }
				if (keyDown['o'] || keyDown['O']) {
					if (Players[1].inAir == false && Players[1].IsJumping == false) {
						Players[1].inAir = true; Players[1].IsJumping = true;
						Players[1].onObject = false;
						Players[1].InAirCounter = 0.25f;
						Players[1].setPosition(glm::vec3(Players[1].Position().x, Players[1].Position().y + (Players[1].Radius().y * 0.30f), Players[1].Position().z));
					}
				}
				//first press of [RIGHT_TRIGGERED]
				if ((keyDown['U'] || keyDown['u']) && Right_TRIGGERED[1] == false) { Right_TRIGGERED[1] = true; std::cout << "[RIGHT_TRIGGERED][-]"; }
				//[RIGHT_TRIGGERED] was pressed last tic
				else if (Right_TRIGGERED[1] == true) {
					//holding [RIGHT_TRIGGERED]
					if (keyDown['U'] || keyDown['u']) {
						PShockWaveChargeUp[1] += deltaTasSeconds;
						//std::cout << "	[C](" << 1 << ")[" << PShockWaveChargeUp[1] << "]" << std::endl;
					}
					//[RIGHT_TRIGGERED] released
					else {
						if (PShockWaveChargeUp[1] < 0.50f) { PShockWaveChargeUp[1] = 0.0f; }
						Right_TRIGGERED[1] = false;
						PShockWave[1] = true;
						PShockWaveCounter[1] = 0.250f;
						MenuSwitchCounter[1] = 0.60f;
						std::cout << "[RIGHT_TRIGGERED][+]";
					}
					Players[1].setVelocity(Players[1].Velocity()*0.5f);
				}

				Players[1].setForceOnObject(glm::vec3(Tx[1], Ty[1], Tz[1]));
				Players[1].setVelocity(glm::vec3(Tx[1], Ty[1], Tz[1]));
				Players[1].ForwardDirection = (Players[1].Position() - (Players[1].Position() + Players[1].Velocity()));
				ShockWaves[1].setPosition(Players[1].Position());
			}
		}
		else if (cameraMode == 1) {
			for (int playerNumberControl = 0; playerNumberControl < NumberOfPlayers; playerNumberControl++) {
				float Tmovement[2]{ 0.0f,0.0f }; float yaw[2]{ 0.0f,0.0f }; float pitch[2]{ 0.0f,0.0f };
				if (MenuSwitchCounter[0] > 0.0f) { MenuSwitchCounter[0] -= deltaTasSeconds; }
				else {
					//checks to see if the sticks are out of the deadzone, then translates them based on how far the stick is pushed.
					if (keyDown['w'] || keyDown['W']) { Tmovement[0] = 00.50f; }
					if (keyDown['s'] || keyDown['S']) { Tmovement[0] = -0.50f; }
					if (keyDown['a'] || keyDown['A']) { yaw[0] = 00.05f; }
					if (keyDown['d'] || keyDown['D']) { yaw[0] = -0.05f; }
					if (keyDown['q'] || keyDown['Q']) {
						if (Players[0].inAir == false && Players[0].IsJumping == false) {
							Players[0].inAir = true; Players[0].IsJumping = true;
							Players[0].onObject = false;
							Players[0].InAirCounter = 0.25f;
							Players[0].setPosition(glm::vec3(Players[0].Position().x, Players[0].Position().y + (Players[0].Radius().y * 0.30f), Players[0].Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['e'] || keyDown['E']) && Right_TRIGGERED[0] == false) { Right_TRIGGERED[0] = true; std::cout << "[RIGHT_TRIGGERED][-]"; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (Right_TRIGGERED[0] == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['e'] || keyDown['E']) {
							PShockWaveChargeUp[0] += deltaTasSeconds;
							//std::cout << "	[C](" << 0 << ")[" << PShockWaveChargeUp[0] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PShockWaveChargeUp[0] < 0.50f) { PShockWaveChargeUp[0] = 0.0f; }
							Right_TRIGGERED[0] = false;
							PShockWave[0] = true;
							PShockWaveCounter[0] = 0.250f;
							MenuSwitchCounter[0] = 0.60f;
							std::cout << "[RIGHT_TRIGGERED][+]";
						}
						Players[0].setVelocity(Players[0].Velocity()*0.5f);
					}
				}

				if (MenuSwitchCounter[1] > 0.0f) { MenuSwitchCounter[1] -= deltaTasSeconds; }
				else {
					//player Two
					if (keyDown['i'] || keyDown['I']) { Tmovement[1] = 00.50f; }
					if (keyDown['k'] || keyDown['K']) { Tmovement[1] = -0.50f; }
					if (keyDown['j'] || keyDown['J']) { yaw[1] = 00.05f; }
					if (keyDown['l'] || keyDown['L']) { yaw[1] = -0.05f; }
					if (keyDown['o'] || keyDown['O']) {
						if (Players[1].inAir == false && Players[1].IsJumping == false) {
							Players[1].inAir = true; Players[1].IsJumping = true;
							Players[1].onObject = false;
							Players[1].InAirCounter = 0.25f;
							Players[1].setPosition(glm::vec3(Players[1].Position().x, Players[1].Position().y + (Players[1].Radius().y * 0.30f), Players[1].Position().z));
						}
					}
					//first press of [RIGHT_TRIGGERED]
					if ((keyDown['U'] || keyDown['u']) && Right_TRIGGERED[1] == false) { Right_TRIGGERED[1] = true; std::cout << "[RIGHT_TRIGGERED][-]"; }
					//[RIGHT_TRIGGERED] was pressed last tic
					else if (Right_TRIGGERED[1] == true) {
						//holding [RIGHT_TRIGGERED]
						if (keyDown['U'] || keyDown['u']) {
							PShockWaveChargeUp[1] += deltaTasSeconds;
							//std::cout << "	[C](" << 1 << ")[" << PShockWaveChargeUp[1] << "]" << std::endl;
						}
						//[RIGHT_TRIGGERED] released
						else {
							if (PShockWaveChargeUp[1] < 0.50f) { PShockWaveChargeUp[1] = 0.0f; }
							Right_TRIGGERED[1] = false;
							PShockWave[1] = true;
							PShockWaveCounter[1] = 0.250f;
							MenuSwitchCounter[1] = 0.60f;
							std::cout << "[RIGHT_TRIGGERED][+]";
						}
						Players[1].setVelocity(Players[1].Velocity()*0.5f);
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
				Players[playerNumberControl].setVelocity((Tmovement[playerNumberControl] * forwardVectorTemp)*0.5f);
				//camera position
				cameraPosition[playerNumberControl] = glm::vec3(Players[playerNumberControl].Position().x - (forwardVector[playerNumberControl].x*6.0f),
					Players[playerNumberControl].Position().y + (Players[playerNumberControl].Radius().y*2.0f),
					Players[playerNumberControl].Position().z - (forwardVector[playerNumberControl].z*6.0f));
				//shockwace position
				ShockWaves[playerNumberControl].setPosition(Players[playerNumberControl].Position());
			}
		}
	}



	//if (keyDown['-']) { lightPosition_01.y -= 1.0f; }
	//if (keyDown['=']) { lightPosition_01.y += 1.0f; }
	//if (keyDown['i']) { lightPosition_01.x += 1.0f; }
	//if (keyDown['k']) { lightPosition_01.x -= 1.0f; }
	//if (keyDown['j']) { lightPosition_01.z -= 1.0f; }
	//if (keyDown['l']) { lightPosition_01.z += 1.0f; }
	

	if (inMenu) { MenuScreen(deltaTasSeconds); }
	else if (inGame) { GameField(deltaTasSeconds); }
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
	for (int i = 20; i < 100; i++) {
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
	float changex = x - mousepositionX;
	float changey = y - mousepositionY;
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
		for (unsigned int i = 0; i < NumberOfSliders; i++) {
			//move nob along the slider
			if (Slider[i].moveNob(MPosToOPosX, MPosToOPosY)) { ButtonForSliders[i].setPosition(glm::vec3(MPosToOPosX, 0.02f, Slider[i].SBar_Pos.z)); }
		}
	}
}

//////////////////////////////////////////////////////////////////////

void init()
{
	///// INIT OpenGL /////
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Turn on the lights
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL); // final polygon color will be based on glColor and glMaterial
	glShadeModel(GL_FLAT); //GL_FLAT //GL_SMOOTH

	// Call some OpenGL parameters
	glEnable(GL_CULL_FACE); //glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);


	glEnable(GL_DEPTH_TEST);//Enable Z-buffer read and write (for hidden surface removal)
	glEnable(GL_BLEND); //Enable blending
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); //premultiplied alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //non-premultiplied alpha

	//Antialiasing/////////////////
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//Polygon Display Mode/////////////////
	glLineWidth(4.0f);
	glPolygonMode(GL_FRONT,GL_FILL);
	//glPolygonMode(GL_FRONT, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

/* function LoadAllSounds()
* Description:
*   - this is called when the file first loads
*   - loads all the needed sounds into the game
*/
void LoadAllSounds() {
	std::string SoundPath;
	if (DoesFileExists("..//Assets//Media")) {
		SoundPath = "..//Assets//Media//";
	}
	else if (DoesFileExists("Assets//Media")) {
		SoundPath = "Assets//Media//";
	}
	else { std::cout << "[ERROR] Could not find [Media]" << std::endl; }

	systemSound.sys.init();
	drum[0].load(_strdup((SoundPath + "drumloop.wav").c_str()), true);
}

/* function LoadAllObjects()
* Description:
*   - this is called when the file first loads
*   - loads all the needed objects into the game
*/
void LoadAllObjects()
{
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
	Borders[0].setMass(0.0f);
	Borders[0].setScale(glm::vec3(100.0f, 100.0f, 100.0f));
	Borders[0].setSizeOfHitBox(glm::vec3(100.0f, 100.0f, 100.0f)); //HitBox
	Borders[0].setPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	Borders[0].setColour(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	//Borders[1].objectLoader(&Borders[0]);
	//Borders[1].setSizeOfHitBox(glm::vec3(50.0f, 100.0f, 50.0f)); //HitBox
	//Borders[1].setPosition(glm::vec3(0.0f, 50.0f, -60.0f));

	//Player ONE
	Players[0].objectLoader(ObjectPath + "PlayerModel.obj");
	Players[0].setMass(5.0f);
	Players[0].setScale(glm::vec3(5.0f, 5.0f, 5.0f)); //displayed size
	Players[0].setSizeOfHitBox(glm::vec3(10.0f, 2.50f, 10.0f)); //HitBox
	Players[0].setPosition(glm::vec3(10.0f, -1.0f, 0.0f)); //Position of Object
	Players[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "PlayerModelTexture_01.png").c_str())));
	//Player TWO
	Players[1].objectLoader(&Players[0]);
	Players[1].setPosition(glm::vec3(-10.0f, -1.0f, 0.0f)); //Position of Object
	Players[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "PlayerModelTexture_02.png").c_str())));
	//Player ShockWave
	ShockWaves[0].objectLoader(ObjectPath + "ShockWave.obj");
	ShockWaves[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "ShockWave_01.png").c_str())));
	ShockWaves[0].setMass(10.0f);
	ShockWaves[0].setSizeOfHitBox(glm::vec3(1.0f, 1.0f, 1.0f)); //HitBox
	ShockWaves[0].textureHandle_hasTransparency = true;
	ShockWaves[1].objectLoader(&ShockWaves[0]);
	ShockWaves[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "ShockWave_02.png").c_str())));
	//Player ONE Tower
	Rifts[0].objectLoader(ObjectPath + "Square.obj");
	Rifts[0].setColour(glm::vec4(1.5f, 1.5f, 0.0f, 0.10f));
	Rifts[0].setMass(0.0f);
	Rifts[0].setScale(glm::vec3(2.0f, 7.0f, 30.0f));
	Rifts[0].setSizeOfHitBox(glm::vec3(1.0f, 7.0f, 30.0f)); //HitBox
	Rifts[0].setPosition(glm::vec3(48.0f, 3.50f, 0.0f));
	Rifts[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rift_01.png").c_str())));
	//Player TWO Tower
	Rifts[1].objectLoader(&Rifts[0]);
	Rifts[1].setPosition(glm::vec3(-48.0f, 3.50f, 0.0f));
	Rifts[1].setRotation(glm::vec3(0.0f, (degToRad*180.0f), 0.0f));
	Rifts[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Rift_02.png").c_str())));

	//Specials
	Specials[0].objectLoader(ObjectPath + "Square.obj");
	Specials[0].setColour(glm::vec4(1.5f, 1.5f, 1.5f, 1.0f));
	Specials[0].setMass(1.0f);
	Specials[0].setScale(glm::vec3(4.0f, 4.0f, 4.0f));
	Specials[0].setSizeOfHitBox(glm::vec3(4.0f, 2.0f, 4.0f)); //HitBox
	Specials[0].setPosition(glm::vec3(0.0f, 10.0f, 0.0f));
	Specials[0].setSpecialAttribute(0);
	Specials[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Box.png").c_str())));
	Specials[0].Viewable = false;
	Specials[1].objectLoader(&Specials[0]);
	Specials[1].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
	Specials[1].setSpecialAttribute(1);
	Specials[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Box_RushRift.png").c_str())));
	Specials[2].objectLoader(&Specials[0]);
	Specials[2].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
	Specials[2].setSpecialAttribute(2);
	Specials[2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Box_InAir.png").c_str())));
	Specials[3].objectLoader(&Specials[0]);
	Specials[3].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
	Specials[3].setSpecialAttribute(3);
	Specials[3].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Box_Health.png").c_str())));
	Specials[4].objectLoader(&Specials[0]);
	Specials[4].setPosition(glm::vec3(0.0f, 20.0f, 0.0f));
	Specials[4].setSpecialAttribute(4);
	Specials[4].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Box.png").c_str())));

	//*degToRad

	//Map
	Objects[0].objectLoader(ObjectPath + "Map002.obj");
	Objects[0].setColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	Objects[0].setMass(0.0f);
	Objects[0].setSizeOfHitBox(glm::vec3(100.0f, 0.01f, 100.0f)); //HitBox
	Objects[0].setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	Objects[0].setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	Objects[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Ground.png").c_str())));
	//Walls Left
	Objects[1].objectLoader(ObjectPath + "Wall_LR.obj");
	Objects[1].setColour(glm::vec4(0.5f, 0.50f, 0.5f, 1.0f));
	Objects[1].setMass(0.0f);
	Objects[1].setScale(glm::vec3(1.0f, 1.0f, 1.0f)); //size //not HitBox
	//Objects[1].setSizeOfHitBox(glm::vec3(100.0f, 10.0f, 1.0f)); //HitBox
	Objects[1].setPosition(glm::vec3(0.0f, -1.0f, -50.0f));
	Objects[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Walls_LR.png").c_str())));
	//Walls Right
	Objects[2].objectLoader(ObjectPath + "Wall_LR.obj");
	Objects[2].setColour(glm::vec4(0.5f, 0.50f, 0.5f, 1.0f));
	Objects[2].setMass(0.0f);
	Objects[2].setScale(glm::vec3(1.0f, 1.0f, 1.0f)); //size //not HitBox
	//Objects[2].setSizeOfHitBox(glm::vec3(100.0f, 10.0f, 1.0f)); //HitBox
	Objects[2].setPosition(glm::vec3(0.0f, -1.0f, 50.0f));
	Objects[2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Walls_LR.png").c_str())));
	//Walls Front
	Objects[3].objectLoader(ObjectPath + "Wall_FB.obj");
	Objects[3].setColour(glm::vec4(0.5f, 0.50f, 0.5f, 1.0f));
	Objects[3].setMass(0.0f);
	Objects[3].setScale(glm::vec3(1.0f, 1.0f, 1.0f)); //size //not HitBox
	//Objects[3].setSizeOfHitBox(glm::vec3(1.0f, 10.0f, 100.0f)); //HitBox
	Objects[3].setPosition(glm::vec3(-50.0f, -1.0f, 0.0f));
	Objects[3].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Walls_FB.png").c_str())));
	//Walls Back
	Objects[4].objectLoader(ObjectPath + "Wall_FB.obj");
	Objects[4].setColour(glm::vec4(0.5f, 0.50f, 0.5f, 1.0f));
	Objects[4].setMass(0.0f);
	Objects[4].setScale(glm::vec3(1.0f, 1.0f, 1.0f)); //size //not HitBox
	//Objects[4].setSizeOfHitBox(glm::vec3(1.0f, 10.0f, 100.0f)); //HitBox
	Objects[4].setPosition(glm::vec3(50.0f, -1.0f, 0.0f));
	Objects[4].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Walls_FB.png").c_str())));
	//Rift magnets
	Objects[5].objectLoader(ObjectPath + "Rift//Magnet_Left.obj");
	Objects[5].setPosition(glm::vec3(47.0f, 0.0f, -20.0f));
	Objects[5].setMass(0.0f);
	Objects[5].setScale(glm::vec3(1.0f, 1.0f, 2.0f));
	Objects[5].setSizeOfHitBox(glm::vec3(2.650f, 7.0f, 3.60f)); //HitBox
	Objects[5].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Magnet_Rift.png").c_str())));
	Objects[6].objectLoader(ObjectPath + "Rift//Magnet_Right.obj");
	Objects[6].setPosition(glm::vec3(47.0f, 0.0f, 20.0f));
	Objects[6].setMass(0.0f);
	Objects[6].setScale(glm::vec3(1.0f, 1.0f, 2.0f));
	Objects[6].setSizeOfHitBox(glm::vec3(2.650f, 7.0f, 3.60f)); //HitBox
	Objects[6].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Magnet_Rift.png").c_str())));
	Objects[7].objectLoader(&Objects[5]);//left
	Objects[7].setPosition(glm::vec3(-47.0f, 0.0f, 20.0f));
	Objects[7].setRotation(glm::vec3(0.0f, (degToRad*180.0f), 0.0f));
	Objects[8].objectLoader(&Objects[6]);//right
	Objects[8].setPosition(glm::vec3(-47.0f, 0.0f, -20.0f));
	Objects[8].setRotation(glm::vec3(0.0f, (degToRad*180.0f), 0.0f));

	//
	Objects[9].objectLoader(ObjectPath + "Square.obj");
	Objects[9].setMass(0.0f);
	Objects[9].setScale(glm::vec3(4.0f, 4.0f, 4.0f));
	Objects[9].setSizeOfHitBox(glm::vec3(4.0f, 2.0f, 4.0f)); //HitBox
	Objects[9].setPosition(glm::vec3(-15.0f, 2.0f, 10.0f));
	Objects[9].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Box.png").c_str())));
	Objects[10].objectLoader(&Objects[9]);
	Objects[10].setPosition(glm::vec3(10.0f, 2.0f, 20.0f));
	Objects[11].objectLoader(&Objects[9]);
	Objects[11].setPosition(glm::vec3(6.0f, 2.0f, -15.0f));
	Objects[12].objectLoader(&Objects[9]);
	Objects[12].setPosition(glm::vec3(-30.0f, 2.0f, -20.0f));


	//Enemies
	Enemies[0].objectLoader(ObjectPath + "obj_03.obj");
	Enemies[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Enemy.png").c_str())));

	ShadowObject[0].objectLoader(ObjectPath + "PlainForShadow.obj");
	ShadowObject[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Shadow_01.png").c_str())));
	ShadowObject[0].textureHandle_hasTransparency = true;
	ShadowObject[1].objectLoader(&ShadowObject[0]);
	ShadowObject[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Shadow_01.png").c_str())));

	for (unsigned int i = 1; i < NumberOfEnemies; i++) {
		Enemies[i].objectLoader(&Enemies[0]);

	}


	//Load Line Segments
	//morphmath.loadMorphSegments("Obj/" "LineSegment.txt");
	//HTRLoader skeleton;
	//skeleton.loadHTR("assets/animations/Orbit.htr");
	//skeleton.createGameObjects();
	//Players[0].addChild(skeleton.getRootGameObject());
}

/* function LoadAllTextPlane()
* Description:
*   - this is called when the file first loads
*   - loads all the menu pages into the game
*/
void LoadAllTextPlane()
{
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
	planeForText[0].setMass(0.0f);
	planeForText[0].Viewable = true;
	planeForText[0].setScale(glm::vec3(39.0f*0.9f, 1.0f, 39.0f*1.6f));
	planeForText[0].setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	planeForText[0].setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	planeForText[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Main_Menu.png").c_str())));

	planeForText[1].objectLoader(&planeForText[0]);
	planeForText[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Options.png").c_str())));

	planeForText[2].objectLoader(&planeForText[0]);
	planeForText[2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Controls.png").c_str())));

	planeForText[3].objectLoader(&planeForText[0]);
	planeForText[3].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Power_Ups.png").c_str())));

	planeForText[4].objectLoader(&planeForText[0]);
	planeForText[4].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Options.png").c_str())));



	//Start Button
	ButtonObjects[0].objectLoader(ObjectPath + "PlainForText.obj");
	ButtonObjects[0].setScale(glm::vec3(9.0f*0.9f, 1.0f, 7.0f*1.6f));
	ButtonObjects[0].setSizeOfHitBox(glm::vec3(9.0f*1.45f, 1.0f, 7.0f*1.58f));
	ButtonObjects[0].setPosition(glm::vec3(0.0f, 0.01f, -3.0f));
	ButtonObjects[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Start_Button.png").c_str())));
	for (unsigned int i = 1; i < NumberOfButtons; i++) { ButtonObjects[i].objectLoader(&ButtonObjects[0]); }
	//Options Button
	ButtonObjects[1].setPosition(glm::vec3(0.0f, 0.01f, 9.0f));
	ButtonObjects[1].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Options_Button.png").c_str())));
	//Exit Button
	ButtonObjects[2].setPosition(glm::vec3(0.0f, 0.01f, 21.0f));
	ButtonObjects[2].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Exit_Button.png").c_str())));
	//Back Button
	ButtonObjects[3].setPosition(glm::vec3(-21.0f, 0.01f, -21.0f));
	ButtonObjects[3].setScale(glm::vec3(2.0f*0.9f, 1.0f, 3.50f*0.9f));
	ButtonObjects[3].setSizeOfHitBox(glm::vec3(2.0f*1.5f, 1.0f, 3.50f*0.9f));
	ButtonObjects[3].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Back_Button.png").c_str())));
	//Exit Button
	ButtonObjects[4].setPosition(glm::vec3(0.0f, 0.01f, -3.0f));
	ButtonObjects[4].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Exit_Button.png").c_str())));
	//Exit Button
	ButtonObjects[5].setPosition(glm::vec3(0.0f, 0.01f, 9.0f));
	ButtonObjects[5].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Exit_Button.png").c_str())));
	//Exit Button
	ButtonObjects[6].setPosition(glm::vec3(0.0f, 0.01f, 21.0f));
	ButtonObjects[6].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Exit_Button.png").c_str())));
	//_Button.png
	for (unsigned int i = 0; i < NumberOfButtons; i++) {
		Button[i].SBut_Top = ButtonObjects[i].Top(), Button[i].SBut_Bot = ButtonObjects[i].Bottom(), Button[i].SBut_Pos = ButtonObjects[i].Position(), Button[i].SBut_Rad = (ButtonObjects[i].Radius() / 2.0f);
	}
	

	//Slider bar
	planeForSliders[0].objectLoader(ObjectPath + "PlainForText.obj");
	planeForSliders[0].setScale(glm::vec3(5.0f, 1.0f, 3.50f));
	planeForSliders[0].setSizeOfHitBox(glm::vec3(5.0f*1.6f, 1.0f, 3.50f*0.9f));
	planeForSliders[0].setPosition(glm::vec3(20.0f, 0.01f, -10.0f));
	planeForSliders[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Slider_Bar.png").c_str())));
	//Slider nob
	ButtonForSliders[0].objectLoader(&planeForSliders[0]);
	ButtonForSliders[0].setScale(glm::vec3(1.0f, 1.0f, 3.50f));
	ButtonForSliders[0].setSizeOfHitBox(glm::vec3(1.0f, 1.0f, 3.50f*0.9f));
	ButtonForSliders[0].setPosition(glm::vec3(20.0f, 0.02f, -10.0f));
	ButtonForSliders[0].setTexture(ilutGLLoadImage(_strdup((ImagePath + "Slider_Nob.png").c_str())));


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
	SetConsoleTitle("Rift Ball");
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

	//Textures & Texture parameters
	glEnable(GL_TEXTURE_2D);
	init();
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	//Initialize Shader

	if (DoesFileExists("..//Assets//shaders//")) { shader = new Shader("..//Assets//shaders//passthru_v.glsl", "..//Assets//shaders//passthru_f.glsl"); }
	else if (DoesFileExists("Assets//shaders//")) { shader = new Shader("Assets//shaders//passthru_v.glsl", "Assets//shaders//passthru_f.glsl"); }
	else { std::cout << "[ERROR] Could not find [Shaders]" << std::endl; }

	//if (DoesFileExists("..//Assets//shaders//")) { shader = new Shader("..//Assets//shaders//default_v.glsl", "..//Assets//shaders//default_f.glsl"); }
	//else if (DoesFileExists("Assets//shaders//")) { shader = new Shader("Assets//shaders//default_v.glsl", "Assets//shaders//default_f.glsl"); }
	//else { std::cout << "[ERROR] Could not find [Shaders]" << std::endl; }

	//Initialize Font/Text Shader
	if (DoesFileExists("..//Assets//shaders//")) { TextShader = new Shader("..//Assets//shaders//text_v.glsl", "..//Assets//shaders//text_f.glsl"); }
	else if (DoesFileExists("Assets//shaders//")) { TextShader = new Shader("Assets//shaders//text_v.glsl", "Assets//shaders//text_f.glsl"); }
	else { std::cout << "[ERROR] Could not find [Shaders]" << std::endl; }

	

	shader->bind();
	glEnableVertexAttribArray(0);	glBindAttribLocation(shader->getID(), 0, "vIn_vertex");
	glEnableVertexAttribArray(1);	glBindAttribLocation(shader->getID(), 1, "vIn_uv");
	glEnableVertexAttribArray(2);	glBindAttribLocation(shader->getID(), 2, "vIn_normal");
	glEnableVertexAttribArray(3);	glBindAttribLocation(shader->getID(), 3, "vIn_colour");

	//glEnableVertexAttribArray(4);	glBindAttribLocation(shader->getID(), 4, "vIn_vertex");
	//glEnableVertexAttribArray(5);	glBindAttribLocation(shader->getID(), 5, "vIn_uv");
	//glEnableVertexAttribArray(6);	glBindAttribLocation(shader->getID(), 6, "vIn_normal");
	//glEnableVertexAttribArray(7);	glBindAttribLocation(shader->getID(), 7, "vIn_colour");

	if (DoesFileExists("..//Assets//img//win.png")) { textureHandle = ilutGLLoadImage("..//Assets//img//win.png"); }
	else if (DoesFileExists("Assets//img//win.png")) { textureHandle = ilutGLLoadImage("Assets//img//win.png"); }
	glBindTexture(GL_TEXTURE_2D, 0);

	if (DoesFileExists("..//Assets//Fonts//")) { SystemText.LoadTextFont("..//Assets//Fonts//FreeSans.ttf", SystemText); }
	else if (DoesFileExists("Assets//Fonts//")) { SystemText.LoadTextFont("Assets//Fonts//FreeSans.ttf", SystemText); }
	else { std::cout << "[ERROR] Could not find [Font]" << std::endl; }

	LoadAllSounds();
	LoadAllObjects();
	LoadAllTextPlane();

	for (int i = 0; i < 4; i++) {
		cameralook = i;
		WhatCameraIsLookingAt();
	}

	srand(static_cast <unsigned> (time(NULL)));
	std::cout << std::endl << "Press [Esc] to exit." << std::endl;
	std::cout << "_____________________________________" << std::endl;
	planeForText[0].Viewable = true;

	// start the event handler
	glutMainLoop();

	shader->unbind();
	delete shader; shader = NULL;

	return 0;
}



