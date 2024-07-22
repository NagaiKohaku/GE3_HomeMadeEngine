#pragma once
#include "d3d12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

#include "unordered_map"
#include "string"
#include "vector"
#include "wrl.h"

class DirectXCommon;

class SrvManager;

class TextureManager {

public:
	static TextureManager* GetInstance();

	void Initialize();

	void LoadTexture(const std::string& filePath);

	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

	uint32_t GetSrvIndex(const std::string& filePath);

	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

	static uint32_t kSRVIndexTop;

private:

	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metaData;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	DirectXCommon* directXCommon_ = nullptr;

	SrvManager* srvManager_ = nullptr;

	std::unordered_map<std::string, TextureData> textureDatas;

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(
		Microsoft::WRL::ComPtr<ID3D12Resource> texture,
		const DirectX::ScratchImage& mipImages
	);
};