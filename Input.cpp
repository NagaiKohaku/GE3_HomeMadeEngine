#include "Input.h"
#include "cassert"

Input* Input::GetInstance() {
	static Input* instance;
	return instance;
}

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	HRESULT result;

	ComPtr<IDirectInput8> directInput_ = nullptr;

	//DirectInputのインスタンス生成
	result = DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput_,
		nullptr
	);

	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;

	//キーボードデバイス生成
	result = directInput_->CreateDevice(
		GUID_SysKeyboard,
		&keyboard_,
		NULL
	);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(
		hwnd,
		DISCL_FOREGROUND |
		DISCL_NONEXCLUSIVE |
		DISCL_NOWINKEY
	);

}

void Input::Update() {

}