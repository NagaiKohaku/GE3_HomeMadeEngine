#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "Windows.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include "Vector.h"

#include "cstdint"

#include "wrl.h"

class WinApp;

class Input {

public:

	//template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static Input* GetInstance();

	void Initialize();

	void Update();

	bool isPushKey(uint8_t keyNum);

	bool isReleaseKey(uint8_t keyNum);

	bool IsTriggerPushKey(uint8_t keyNum);

	bool IsTriggerReleaseKey(uint8_t keyNum);

	bool IsPushMouseButton(int mouseButton);

	bool IsReleaseMouseButton(int mouseButton);

	bool IsTriggerPushMouseButton(int mouseButton);

	bool IsTriggerReleaseMouseButton(int mouseButton);

	Vector2 GetMousePos();

	Vector3 GetMouseVelocity();

private:

	WinApp* winApp_ = nullptr;

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> gamePad_;

	BYTE key_[256] = {};

	BYTE preKey_[256] = {};

	DIMOUSESTATE mouseState_;

	DIMOUSESTATE preMouseState_;
};