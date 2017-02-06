//Jacob Rosengren
#pragma once
#pragma comment(lib, "Include/FMOD/fmod_vc.lib")

#include "fmod.hpp"
#include "fmod_errors.h"
#include <Windows.h>
#include <iostream>
//using namespace std;

void CheckResult(FMOD_RESULT result);

class SoundEngine
{
private:
	bool systemInit;
public:
	SoundEngine();
	~SoundEngine();
	void update();
	bool init(); //

	//Sound Engine
	FMOD::System *system = NULL;
	void         *driverData = NULL;

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

	bool load(char* fileName, bool loop);
	void play();
	void playUpdate();
	static void systemUpdate();
	void setRollOffModelCurve(FMOD_VECTOR, FMOD_VECTOR, FMOD_VECTOR);

	bool isPlaying;
	bool RollOff;

	//Sound
	FMOD::Sound   *sound;
	FMOD::Channel *channel;
	FMOD_VECTOR pos;
	FMOD_VECTOR vel;

	//
	FMOD_VECTOR RollOffModelCurve[3] =
	{
		{ 0.0f, 1.0f, 0.0f },
		{ 10.0f, 0.2f, 0.0f },
		{ 60.0f, 0.0f, 0.0f }
	};

	static SoundEngine sys; //all veriables share the same sound engine
};