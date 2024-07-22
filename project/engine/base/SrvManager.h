#pragma once

#include "d3d12.h"

#include "cstdint"
#include "wrl.h"

class DirectXCommon;

class SrvManager {

public:

	//最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount_;

	//静的インスタンス
	static SrvManager* GetInstance();

	//初期化
	void Initialize();

	//メモリ確保
	uint32_t Allocate();

	//描画前処理
	void PreDraw();

	//確保可能チェック
	bool AllocateCheck();

	//SRV生成(テクスチャ用)
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

	//SRV生成(Structured Buffer用)
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

private:

	DirectXCommon* directXCommon = nullptr;

	uint32_t useIndex_ = 0;

	//SRV用のデスクリプタサイズ
	uint32_t descriptorSize_;

	//SRVデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

};