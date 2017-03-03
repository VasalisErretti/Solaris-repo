#include <iostream>
#include "SoundEngine.h"

FMOD_RESULT      result;

void CheckResult(FMOD_RESULT result){
	if (result != FMOD_OK) { string mesg = FMOD_ErrorString(result); }
}

SoundEngine::SoundEngine() {
	system = NULL;
	driverData = NULL;

	init = false;
	//Listener
	listenerForward	 = { 0.0f, 0.0f, 0.0f };
	listenerUp		 = { 0.0f, 0.0f, 0.0f };
	listenerPos		 = { 0.0f, 0.0f, 0.0f };
	listenerVel		 = { 0.0f, 0.0f, 0.0f };
}

SoundEngine::~SoundEngine() {
	if (init) {
		result = system->close(); CheckResult(result);
		result = system->release(); CheckResult(result);
	}
}

void SoundEngine::Update(){
	result = system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp); CheckResult(result);

	result = system->update(); CheckResult(result);
}

bool SoundEngine::Init() {
	if (init) {
		return true;
	}
	//Create a System object and initialize.
	unsigned int version;
	result = FMOD::System_Create(&system); CheckResult(result);
	result = system->getVersion(&version); CheckResult(result);

	if (version < FMOD_VERSION)
	{
		cout << "FMOD lib version doesn't match header!" << endl;
		return false;
	}

	result = system->init(100, FMOD_INIT_NORMAL, driverData);
	CheckResult(result);

	if (result != FMOD_OK) {
		cout << "Initialization Failed!" << endl;
		return false;
	}

	result = system->set3DSettings(1.0, 1.0, 1.0f);
	CheckResult(result);

	init = true;

	return true;
}

////////////////////////////////////////////////////////////

SoundEngine Sound::Sys;

Sound::Sound() {
	sound = NULL;
	channel = NULL;
}

Sound::~Sound() {
	result = sound->release(); CheckResult(result);
}

bool Sound::Load(char* fileName, bool is3d, bool isLoop) {
	//Init sound
	if (!Sys.init)
	{
		if (!Sys.Init()) { return false; }
	}

	//Load sound
	this->is3d = is3d;
	if (is3d) { result = Sys.system->createSound(fileName, FMOD_3D, 0, &sound); }
	else { result = Sys.system->createSound(fileName, FMOD_2D, 0, &sound); }
	CheckResult(result);


	//Load File
	if (result != FMOD_OK) {
		cout << "Could not open file [" << fileName << "]" << endl;
		return false;
	}
	
	//Set distance
	result = sound->set3DMinMaxDistance(0.5f, 5000.0f); CheckResult(result);

	//Set Loop
	if (isLoop) { sound->setMode(FMOD_LOOP_NORMAL); }
	else { sound->setMode(FMOD_LOOP_OFF); }
	CheckResult(result);

	return true;
}

FMOD::Channel* Sound::Play()
{
	FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

	//Play sound
	result = Sys.system->playSound(sound, 0, true, &channel); CheckResult(result);
	if (is3d)
	{
		result = channel->set3DAttributes(&pos, &vel); CheckResult(result);
	}
	result = channel->setPaused(false); CheckResult(result);

	return channel;
}


void Sound::SystemUpdate() {
	Sys.Update();
}


void Sound::SetRollOffModelCurve(FMOD_VECTOR ROMC_0, FMOD_VECTOR ROMC_1, FMOD_VECTOR ROMC_2) {
	RollOffModelCurve[0] = ROMC_0; RollOffModelCurve[1] = ROMC_1; RollOffModelCurve[2] = ROMC_2;
}

void Sound::SetPosition(FMOD::Channel *channel, FMOD_VECTOR position, FMOD_VECTOR velocity){
	if (is3d) { result = channel->set3DAttributes(&position, &velocity); CheckResult(result); }
	else { }
}

void Sound::SetRolloff(FMOD::Channel *channel, bool linear, float min, float max){
	channel->set3DMinMaxDistance(min, max);
	if (linear) { channel->setMode(FMOD_3D_LINEARROLLOFF); }
	else { channel->setMode(FMOD_3D_INVERSEROLLOFF); }
}