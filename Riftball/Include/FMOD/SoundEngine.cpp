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
	for (int i = 0; i < numberOfListeners; i++) {
		//Listener
		listenerForward[i]	 = { 0.0f, 0.0f, 0.0f };
		listenerUp[i]		 = { 0.0f, 0.0f, 0.0f };
		listenerPos[i]		 = { 0.0f, 0.0f, 0.0f };
		listenerVel[i]		 = { 0.0f, 0.0f, 0.0f };
	}
}

SoundEngine::~SoundEngine() {
	if (init) {
		result = system->close(); CheckResult(result);
		result = system->release(); CheckResult(result);
	}
}

void SoundEngine::Update(){
	for (int i = 0; i < numberOfListeners; i++) {
		result = system->set3DListenerAttributes(0, &listenerPos[i], &listenerVel[i], &listenerForward[i], &listenerUp[i]); CheckResult(result);
	}
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

	result = system->set3DSettings(1.0, 1.0, 0.5f);
	CheckResult(result);

	init = true;

	return true;
}

////////////////////////////////////////////////////////////

SoundEngine Sound::Sys;

Sound::Sound() {
	sound = NULL;
	channel = NULL;
	pos = { 0.0f, 0.0f, 0.0f };
	vel = { 0.0f, 0.0f, 0.0f };
	isPlaying = false;
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
	result = sound->set3DMinMaxDistance(0.5f, 10000.0f); CheckResult(result);

	//Set Loop
	if (isLoop) { sound->setMode(FMOD_LOOP_NORMAL); }
	else { sound->setMode(FMOD_LOOP_OFF); }
	CheckResult(result);

	return true;
}
FMOD::Channel* Sound::Play()
{
	if (!isPlayingF()) { isPlaying = false; }
	//Play sound
	if (!isPlaying) { isPlaying = true;
		result = Sys.system->playSound(sound, 0, true, &channel); CheckResult(result);
		if (is3d) {
			result = channel->set3DAttributes(&pos, &vel); CheckResult(result);
		}
		result = channel->setPaused(false); CheckResult(result);
	}

	return channel;
}

bool Sound::isPlayingF() {
	bool IsPlay = false;
	result = channel->isPlaying(&IsPlay);
	if (result == FMOD_ERR_INVALID_HANDLE || result == FMOD_ERR_CHANNEL_STOLEN) { return false; }
	else { return IsPlay; }
}


void Sound::SystemUpdate() {
	Sys.Update();
}


void Sound::SetRollOffModelCurve(FMOD_VECTOR ROMC_0, FMOD_VECTOR ROMC_1, FMOD_VECTOR ROMC_2) {
	RollOffModelCurve[0] = ROMC_0; RollOffModelCurve[1] = ROMC_1; RollOffModelCurve[2] = ROMC_2;
}

void Sound::SetPosition(glm::vec3 m_position) {
	FMOD_VECTOR position; position.x = m_position.x; position.y = m_position.y; position.z = m_position.z;
	FMOD_VECTOR velocity; velocity.x = 0.0f; velocity.y = 0.0f; velocity.z = 0.0f;
	pos = position;
	vel = velocity;

	if (is3d) { result = this->channel->set3DAttributes(&position, &velocity); CheckResult(result); }
}

void Sound::SetPosition(FMOD::Channel *channel, FMOD_VECTOR position) {
	FMOD_VECTOR velocity; velocity.x = 0.0f; velocity.y = 0.0f; velocity.z = 0.0f;
	pos = position;
	vel = velocity;

	if (is3d) { result = channel->set3DAttributes(&position, &velocity); CheckResult(result); }
	else {}
}

void Sound::SetPosition(FMOD::Channel *channel, FMOD_VECTOR position, FMOD_VECTOR velocity){
	pos = position;
	vel = velocity;

	if (is3d) { result = channel->set3DAttributes(&position, &velocity); CheckResult(result); }
	else { }
}

void Sound::SetRolloff(FMOD::Channel *channel, bool linear, float min, float max){
	channel->set3DMinMaxDistance(min, max);
	if (linear) { channel->setMode(FMOD_3D_LINEARROLLOFF); }
	else { channel->setMode(FMOD_3D_INVERSEROLLOFF); }
}