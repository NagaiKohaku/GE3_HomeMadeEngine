#pragma once
#include "externals/DirectXTex/d3dx12.h"

#include "wrl.h"

class DirectXCommon;

class Camera;

class DebugCamera;

class Object3DCommon {

public:
	static Object3DCommon* GetInstance();

	void Initialize();

	void CommonDrawSetting();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void SetDefaultCamera(Camera* camera) { defaultCamera_ = camera; }

	void SetDefaultCamera(DebugCamera* debugCamera) { debugCamera_ = debugCamera; }

	Camera* GetDefaultCamera() const { return defaultCamera_; }

private:

	void CreateRootSignature();

	void CreateGraphicsPipeline();

	DirectXCommon* dxCommon_ = nullptr;

	Camera* defaultCamera_ = nullptr;

	DebugCamera* debugCamera_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
};