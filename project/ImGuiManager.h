#pragma once
#include "d3d12.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include "wrl.h"

class WinApp;

class DirectXCommon;

class ImGuiManager {

public:

	static ImGuiManager* GetInstance();

	void Initialize();

	void Finalize();

	void Begin();

	void End();

	void Draw();

private:

	WinApp* winApp_;
	DirectXCommon* directXCommon_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};