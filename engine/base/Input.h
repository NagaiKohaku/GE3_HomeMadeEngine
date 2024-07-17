#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "Windows.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include "wrl.h"

class WinApp;

class Input {

public:

	//template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static Input* GetInstance();

	void Initialize();

	void Update();

	bool PushKey(BYTE keyNumber);

private:

	WinApp* winApp_ = nullptr;

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;

	BYTE key[256] = {};

};