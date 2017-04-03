//Jacob Rosengren
#pragma once
#pragma comment(lib, "../Include/FMOD/fmod_vc.lib")

#include "fmod.hpp"
#include "fmod_errors.h"
#include <Windows.h>
#include <iostream>
#include <glm\glm\vec3.hpp>
using namespace std;

void CheckResult(FMOD_RESULT result);



class SoundEngine
{
private:
	//bool systemInit;
public:
	SoundEngine();
	~SoundEngine();
	void Update();
	bool Init(); //

	//Sound Engine
	FMOD::System *system = NULL;
	void *driverData = NULL;
	bool init;

	//Listener
	static const int numberOfListeners = 2;
	FMOD_VECTOR listenerForward[numberOfListeners];
	FMOD_VECTOR listenerUp[numberOfListeners];
	FMOD_VECTOR listenerPos[numberOfListeners];
	FMOD_VECTOR listenerVel[numberOfListeners];
};

class Sound
{
public:
	Sound();
	~Sound();

	bool Load(char* fileName, bool is3d, bool isLoop);
	static void SystemUpdate();

	bool isPlaying;
	bool isPlayingF();
	FMOD::Channel* PlayNow();
	FMOD::Channel* Play();
	void SetPosition(glm::vec3 position);
	void SetPosition(FMOD::Channel *channel, FMOD_VECTOR position);
	void SetPosition(FMOD::Channel *channel, FMOD_VECTOR position, FMOD_VECTOR velocity);
	void SetRolloff(FMOD::Channel *channel, bool linear, float min, float max);
	bool is3d;

	//Sound
	FMOD::Sound   *sound;
	FMOD::Channel *channel;
	FMOD_VECTOR pos;
	FMOD_VECTOR vel;

	//Set Roll Off Model Curve
	void SetRollOffModelCurve(FMOD_VECTOR, FMOD_VECTOR, FMOD_VECTOR);
	bool RollOff;
	FMOD_VECTOR RollOffModelCurve[3] =
	{
		{ 0.0f, 1.0f, 0.0f },
		{ 10.0f, 0.2f, 0.0f },
		{ 60.0f, 0.0f, 0.0f }
	};

	static SoundEngine Sys; //all veriables share the same sound engine
};