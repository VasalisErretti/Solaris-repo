//Jacob Rosengren
//Jean Duchene ver2
#pragma once
#pragma comment(lib, "Include/FMOD/fmod_vc.lib")

#include "fmod.hpp"
#include "fmod_errors.h"
#include <Windows.h>
#include <iostream>
using namespace std;

void CheckResult(FMOD_RESULT result);

class SoundEngine
{
public:
	SoundEngine();
	~SoundEngine();
	void update();
	bool Soundengineinit(); //Initializing the sound engine

	//Sound Engine variables
	FMOD::System *system = NULL;
	void         *driverData = NULL;

	//Listener
	FMOD_VECTOR listenerForward;
	FMOD_VECTOR listenerUp;
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR listenerVel;
private:
	bool systeminit;
};

class Sound 
{
public:
	Sound();
	~Sound();

	//bool load(char* fileName, bool loop);
	bool Load(char* fileName, bool is3d, bool isloop);
	FMOD::Channel* Play();
	//void play();
	void playUpdate();
	static void systemUpdate();
	void setRollOffModelCurve(FMOD_VECTOR, FMOD_VECTOR, FMOD_VECTOR);
	void soundUpdate();

	bool is3d; // If a 3d sound, if you want to loop the sound, or if you want to stop 
	bool isloop; // playing the sound
	bool isPlaying;
	bool rolloff;
	static SoundEngine sys; //all veriables share the same sound engine
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

	
};