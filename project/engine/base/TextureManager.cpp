#include "TextureManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

#include "Log.h"

#include "string"
#include "cassert"

uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize() {

	directXCommon_ = DirectXCommon::GetInstance();

	srvManager_ = SrvManager::GetInstance();

	textureDatas.reserve(srvManager_->kMaxSRVCount_);

}

void TextureManager::LoadTexture(const std::string& filePath) {

	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}

	//テクスチャ枚数上限チェック
	assert(srvManager_->AllocateCheck());

	//テクスチャファイルを読んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの生成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas[filePath];

	textureData.filePath = filePath;
	textureData.metaData = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(textureData.metaData);
	textureData.intermediateResource = UploadTextureData(textureData.resource, mipImages);
	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(textureData.metaData.mipLevels);

	directXCommon_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath) {

	TextureData& textureData = textureDatas[filePath];

	return textureData.metaData;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath) {

	TextureData& textureData = textureDatas[filePath];

	return textureData.srvIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath) {

	TextureData& textureData = textureDatas[filePath];

	return textureData.srvHandleGPU;
}

//DirectX12のTextureResourceを作る
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata) {

	//metadataをもとにResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); //Textureの幅
	resourceDesc.Height = UINT(metadata.height); //Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); //mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); //奥引き or 配列Textureの配列数
	resourceDesc.Format = metadata.format; //TextureのFormat
	resourceDesc.SampleDesc.Count = 1; //サンプリングカウント 1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); //Textureの次元数。普段使っているのは2次元

	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース番がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //細かい設定を行う
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //WriteBAckポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //プロセッサの近くに配置

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = directXCommon_->GetDevice()->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, //初回のResourceState。Textureは基本読むだけ
		nullptr, //Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource) //作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(
	Microsoft::WRL::ComPtr<ID3D12Resource> texture,
	const DirectX::ScratchImage& mipImages
) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;

	DirectX::PrepareUpload(directXCommon_->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));

	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = directXCommon_->CreateBufferResource(intermediateSize);

	UpdateSubresources(directXCommon_->GetCommandList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	D3D12_RESOURCE_BARRIER barrier{};

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	directXCommon_->GetCommandList()->ResourceBarrier(1, &barrier);

	return intermediateResource;
}