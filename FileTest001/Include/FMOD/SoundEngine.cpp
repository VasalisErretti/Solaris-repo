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
	systemInit = false;
	//Listener
	listenerForward = { 0.0f, 0.0f,  0.0f };
	listenerUp = { 0.0f, 0.0f,  0.0f };
	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerVel = { 0.0f, 0.0f,  0.0f };
}

SoundEngine::~SoundEngine() {
	if (systemInit) {
		result = system->close(); CheckResult(result);
		result = system->release(); CheckResult(result);
	}
}

void SoundEngine::update(){
	result = system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
	CheckResult(result);

	result = system->update(); CheckResult(result);
}

bool SoundEngine::init() {
	if (systemInit) {
		return true;
	}
	//Create a System object and initialize.
	unsigned int version;
	result = FMOD::System_Create(&system); CheckResult(result);
	result = system->getVersion(&version); CheckResult(result);

	if (version < FMOD_VERSION)
	{
		std::cout << "FMOD lib version doesn't match header!" << std::endl;
		return false;
	}

	result = system->init(100, FMOD_INIT_NORMAL, driverData);
	CheckResult(result);

	if (result != FMOD_OK) {
		std::cout << "Initialization Failed!" << std::endl;
		return false;
	}

	result = system->set3DSettings(1.0, 1.0, 1.0f);
	CheckResult(result);

	systemInit = true;
	return true;
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

bool Sound::load(char* fileName, bool loop) {
	if (!sys.init()) { 
		return false;
	}
	result = sys.system->createSound(fileName, FMOD_3D, 0, &sound);
	CheckResult(result);

	if (result != FMOD_OK) {
		std::cout << "[FO.2] File not opened. [" << fileName << "]" << std::endl;
		return false;
	}

	result = sound->set3DMinMaxDistance(0.5f, 5000.0f); CheckResult(result);
	if (loop) { result = sound->setMode(FMOD_LOOP_NORMAL); CheckResult(result); }
	else { result = sound->setMode(FMOD_LOOP_OFF); CheckResult(result); }
	RollOff = false;

	return true;
}

void Sound::play() {
	if (!isPlaying) {
		result = sys.system->playSound(sound, 0, true, &channel); CheckResult(result);
		result = channel->set3DAttributes(&pos, &vel); CheckResult(result);
		result = channel->setPaused(false); CheckResult(result);
		isPlaying = true;

		result = channel->set3DMinMaxDistance(1.0f, 60.0f); CheckResult(result);
		if (RollOff == true) {
			result = channel->setMode(FMOD_3D_CUSTOMROLLOFF); CheckResult(result);
			result = channel->set3DCustomRolloff(RollOffModelCurve, 3); CheckResult(result);
		}
		else if (RollOff == false) {
			result = channel->setMode(FMOD_3D_LINEARROLLOFF); CheckResult(result);
		}
	}
}

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

void Sound::systemUpdate() {
	sys.update();
}
