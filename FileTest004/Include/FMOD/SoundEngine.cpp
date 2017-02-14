#include <iostream>
#include "SoundEngine.h"

FMOD_RESULT      result;

void CheckResult(FMOD_RESULT result){
	if (result != FMOD_OK)
	{
		std::string mesg = FMOD_ErrorString(result);
	}
}

SoundEngine::SoundEngine() {
	system = NULL;
	driverData = NULL;
	systeminit = FALSE;
	//Listener variables
	listenerForward = { 0.0f, 0.0f,  0.0f };
	listenerUp = { 0.0f, 0.0f,  0.0f };
	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerVel = { 0.0f, 0.0f,  0.0f };
}


SoundEngine::~SoundEngine() {
	//Clean up
	//result = sound->release(); CheckResult(result);
	result = system->close();
	CheckResult(result);
	result = system->release();
	CheckResult(result);
}

void SoundEngine::update(){
	result = system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
	CheckResult(result);

	result = system->update(); CheckResult(result);
}

bool SoundEngine::Soundengineinit() {
	//Create a System object and initialize.
	if (!systeminit) {
		unsigned int version;
		result = FMOD::System_Create(&system);
		CheckResult(result);
		result = system->getVersion(&version);
		(result);

		if (version < FMOD_VERSION)
		{
			cout << "FMOD lib version doesn't match header version" << endl;
			return false;
		}

		result = system->init(100, FMOD_INIT_NORMAL, driverData);
		CheckResult(result);
		if (result != FMOD_OK)
		{
			cout << "intinialization failed" << endl;
			return false;
		}
		result = system->set3DSettings(1.0, 1.0, 1.0f);
		CheckResult(result);

		systeminit = TRUE;
		return TRUE;
	}
	return false;
}

////////////////////////////////////////////////////////////

SoundEngine Sound::sys;

Sound::Sound() {
	sound = NULL;
	channel = NULL;
	pos = { -10.0f , 0.0f, 0.0f };
	vel = { 0.0f, 0.0f, 0.0f };
	isPlaying = false;
}

Sound::~Sound() {
	result = sound->release(); CheckResult(result);
}

bool Sound::Load(char* filename, bool is3d, bool isloop)
{
	if (!sys.Soundengineinit()) {
		return false;
	}
	result = sys.system->createSound(filename, FMOD_3D, 0, &sound);
	CheckResult(result);
	if (result != FMOD_OK) {
		cout << "Couldn't open file " << filename << endl;
		return FALSE;
	}
	result = sound->set3DMinMaxDistance(0.5f, 5.0f);
	CheckResult(result);
	if (isloop == true) {
		result = sound->setMode(FMOD_LOOP_NORMAL);
		CheckResult(result);
	}
	else if (isloop == false)
	{
		result = sound->setMode(FMOD_LOOP_OFF);
		CheckResult(result);
	}
	return TRUE;
}

FMOD::Channel* Sound::Play()
{
	if (!isPlaying) {
		//Play sound
		result = sys.system->playSound(sound, 0, true, &channel);
		CheckResult(result);
		isPlaying = TRUE;
		if (is3d)
		{
			result = channel->set3DAttributes(&pos, &vel);
			CheckResult(result);

		}
		result = channel->setPaused(false);
		CheckResult(result);

		return channel;
	}
}


void Sound::soundUpdate()
{
	result = channel->set3DAttributes(&pos, &vel);
	CheckResult(result);
	if (rolloff == TRUE)
	{
		result = channel->setMode(FMOD_3D_LINEARROLLOFF); //Linear roll off
		CheckResult(result);
	}
	else if (rolloff == FALSE)
	{
		result = channel->setMode(FMOD_3D_INVERSEROLLOFF); // Log roll off
		CheckResult(result);
	}
}
/*
void Sound::playUpdate() {
	if (result != FMOD_OK) {
		//cout << "Could not update sound position." << endl;
		return;
	}
	result = channel->set3DAttributes(&pos, &vel); CheckResult(result);


}

void Sound::setRollOffModelCurve(FMOD_VECTOR ROMC_0, FMOD_VECTOR ROMC_1, FMOD_VECTOR ROMC_2) {
	RollOffModelCurve[0] = ROMC_0; RollOffModelCurve[1] = ROMC_1; RollOffModelCurve[2] = ROMC_2;
}
*/
void Sound::systemUpdate() {
	sys.update();
}
