#pragma once
#include "Windows.h"
#include "wrl.h"
#include "d3d12.h"
#include "dxgi1_6.h"
#include "dxgidebug.h"
#include "dxcapi.h"

#include "cstdint"
#include "string"
#include "chrono"
#include "thread"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "others/ResourceObject.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

class WinApp;

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {

#ifdef _DEBUG

		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;

		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {

			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);

			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);

			debug->Release();
		}

#endif
	}
};

class DirectXCommon {

public:

	//DirectXCommonの静的インスタンス
	static DirectXCommon* GetInstance();

	//初期化
	void Initialize();

	//描画前処理
	void PreDraw();

	//描画後処理
	void PostDraw();

	//デバイスのゲッター
	ID3D12Device* GetDevice() { return device_.Get(); }

	//コマンドリストのゲッター
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

	//SRVディスクリプターヒープのゲッター
	ID3D12DescriptorHeap* GetSRVDescriptorHeap() { return srvDescriptorHeap_.Get(); }

	//SRVディスクリプターヒープのサイズのゲッター
	uint32_t GetSRVDescriptorSize() { return descriptorSizeSRV_; }

	//シェーダーファイルのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile
	);

	//ResourceObjectの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	//最大SRV数
	static const uint32_t kMaxSRVCount;

private:

	//デバイスの生成
	void InitializeDevice();

	//コマンド関連の生成
	void InitializeCommand();

	//スワップチェーンの生成
	void InitializeSwapChain();

	//深度バッファの生成
	void InitializeDepthBuffer();

	//各種デスクリプタヒープの生成
	void InitializeDescriptorHeap();

	//レンダーターゲットビューの初期化
	void InitializeRenderTargetView();

	//深度ステンシルビューの初期化
	void InitializeDepthStencilView();

	//フェンスの初期化
	void InitializeFence();

	//ビューポート矩形の初期化
	void InitializeViewportRect();

	//シザリング矩形の生成
	void InitializeScissorRect();

	//DXCコンパイラの生成
	void InitializeDXCCompile();

	//ImGuiの初期化
	void InitializeImGui();

	//FPS固定初期化
	void initializeFixFPS();

	//FPS固定更新
	void UpdateFixFPS();

	//デスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
		bool shaderVisible
	);

	//CPUデスクリプタヒープのゲッター
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	//GPUデスクリプタヒープのゲッター
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

private:

	//WinAppクラス(借り物)
	WinApp* winApp = nullptr;

	//メモリリークチェック
	D3DResourceLeakChecker leakCheck;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	//コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	//コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	//深度ステンシル
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStancilResource_ = nullptr;

	//各種でスクリプタのサイズ
	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	//RTVデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;

	//SRVデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

	//DSVデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	//スワップチェーンリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };

	//RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

	//フェンスの値
	uint64_t fenceValue_ = 0;

	//ビューポート
	D3D12_VIEWPORT viewport_{};

	//シザー矩形
	D3D12_RECT scissorRect_{};

	//dxcUtils
	IDxcUtils* dxcUtils_ = nullptr;

	//dxcCompiler
	IDxcCompiler3* dxcCompiler_ = nullptr;

	//includeHander
	IDxcIncludeHandler* includeHandler_ = nullptr;

	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

};