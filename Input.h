#pragma once
#include "wrl.h"
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "Windows.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;

class Input {

public:

	static Input* GetInstance();

	void Initialize(HINSTANCE hInstance,HWND hwnd);

	void Update();

private:

};