#pragma once
#include "wrl.h"
#include "externals/DirectXTex/d3dx12.h"

class DirectXCommon;

class SpriteCommon {

public:

	static SpriteCommon* GetInstance();

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