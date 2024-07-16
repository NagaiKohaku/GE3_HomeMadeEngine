#pragma once
#include "wrl.h"
#include "externals/DirectXTex/d3dx12.h"

class DirectXCommon;

class Object3DCommon {

public:
	static Object3DCommon* GetInstance();

	void Initialize();

	void CommonDrawSetting();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:

	void CreateRootSignature();

	void CreateGraphicsPipeline();

	DirectXCommon* dxCommon_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
};