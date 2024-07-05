#pragma once
#include "Windows.h"
#include "cstdint"

class WinApp {

public:

	//ウィンドウの横幅
	static const int32_t kClientWidth = 1280;

	//ウィンドウの縦幅
	static const int32_t kClientHeight = 720;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static WinApp* GetInstance();

	void Initialize();

	void Finalize();

	HINSTANCE GetHInstance() const { return wc_.hInstance; }

	HWND GetHwnd() const { return hwnd_; }

private:

	//ウィンドウクラス
	WNDCLASS wc_{};

	//ウィンドウハンドル
	HWND hwnd_ = nullptr;

};