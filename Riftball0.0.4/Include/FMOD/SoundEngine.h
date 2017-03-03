//Jacob Rosengren
#pragma once
#pragma comment(lib, "../Include/FMOD/fmod_vc.lib")

#include "fmod.hpp"
#include "fmod_errors.h"
#include <Windows.h>
#include <iostream>
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
	FMOD_VECTOR listenerForward;
	FMOD_VECTOR listenerUp;
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR listenerVel;
};

class Sound 
{
public:
	Sound();
	~Sound();

	bool Load(char* fileName, bool is3d, bool isLoop);
	static void SystemUpdate();

	FMOD::Channel* Play();
	void SetPosition(FMOD::Channel *channel, FMOD_VECTOR position, FMOD_VECTOR velocity);
	void SetRolloff(FMOD::Channel *channel, bool linear, float min, float max);
	bool is3d;

	//Sound
	FMOD::Sound   *sound;
	FMOD::Channel *channel;
	//FMOD_VECTOR pos;
	//FMOD_VECTOR vel;

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