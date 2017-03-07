#pragma once
//Defining Xinput Libraries
#include <windows.h>
#include <xinput.h>
//Defining Architecture for Xinput
#define _AMD64_

//Core Libraries
#include <math.h>
#include <fstream>

class Gamepad
{
private:
	int cId;
	XINPUT_STATE state;

	float deadzoneX;
	float deadzoneY;
	WORD lastpressed;
	XINPUT_VIBRATION g;


public:


	Gamepad() : deadzoneX(0.05f), deadzoneY(0.02f) {}
	Gamepad(float dzX, float dzY) : deadzoneX(dzX), deadzoneY(dzY) {}

	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;

	int GetActivePort();
	void SetActivePort(int);
	XINPUT_GAMEPAD *GetState();
	bool CheckConnection();
	bool Refresh();
	bool IsPressed(WORD);
	bool WasPressed(WORD);
};

int Gamepad::GetActivePort()
{
	return cId;
}
void Gamepad::SetActivePort(int newcId)
{
	cId = newcId;
	XInputSetState(cId, &g);
}

XINPUT_GAMEPAD *Gamepad::GetState()
{
	return &state.Gamepad;
}

bool Gamepad::CheckConnection()
{
	int controllerId = -1;

	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE)); 
		if (XInputGetState(i, &state) == ERROR_SUCCESS) { controllerId = i; }
	} 
	cId = controllerId;

	return controllerId != -1;
}

// Returns false if the controller has been disconnected
bool Gamepad::Refresh()
{
	if (cId == -1)
	{
		CheckConnection();
	}

	if (cId != -1)
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState(cId, &state) != ERROR_SUCCESS)
		{
			cId = -1;
			return false;
		}

		float normLX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
		float normLY = fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767);

		leftStickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
		leftStickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));

		if (deadzoneX > 0) leftStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) leftStickY *= 1 / (1 - deadzoneY);

		float normRX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
		float normRY = fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767);

		rightStickX = (abs(normRX) < deadzoneX ? 0 : (abs(normRX) - deadzoneX) * (normRX / abs(normRX)));
		rightStickY = (abs(normRY) < deadzoneY ? 0 : (abs(normRY) - deadzoneY) * (normRY / abs(normRY)));

		if (deadzoneX > 0) rightStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) rightStickY *= 1 / (1 - deadzoneY);

		leftTrigger = (float)state.Gamepad.bLeftTrigger / 255;
		rightTrigger = (float)state.Gamepad.bRightTrigger / 255;

		return true;
	}
	return false;
}

bool Gamepad::IsPressed(WORD button)
{
	lastpressed = button;
	return (state.Gamepad.wButtons & button) != 0;
}

bool Gamepad::WasPressed(WORD button)
{ 
	if (lastpressed == button) {
		return (state.Gamepad.wButtons & button) != 0;
	}
	else { return false; }
}
