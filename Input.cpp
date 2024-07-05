#include "Input.h"
#include "cassert"

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	HRESULT result;

	//DirectInputのインスタンス生成
	result = DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(directInput_.GetAddressOf()),
		nullptr
	);

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

	//キーボード情報の取得開始
	keyboard_->Acquire();
	keyboard_->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNumber) {

	//指定キーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}

	//そうでなければfalseを返す
	return false;
}