#pragma once
#include <Windows.h>
#include "wrl.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "WinApp.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

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

	static DirectXCommon* GetInstance();

	void Initialize();

private:

	void InitializeDevice();

	void InitializeCommand();

	void InitializeSwapChain();

	void InitializeDepthBuffer();

	void InitializeDescriptorHeap();

	void InitializeRenderTargetView();

	void InitializeDepthStencilView();

	void InitializeFence();

	void InitializeViewportRect();

	void InitializeScissorRect();

	void InitializeDXCCompile();

	void InitializeImGui();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
		bool shaderVisible
	);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

private:

	WinApp* winApp = nullptr;

	D3DResourceLeakChecker leakCheck;

	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	//コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	//コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStancilResource_ = nullptr;

	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	Microsoft::WRL::ComPtr<ID3D12Resource> backBufferResource_[2] = { nullptr };

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

	//ビューポート
	D3D12_VIEWPORT viewport_{};

	//シザー矩形
	D3D12_RECT scissorRect_{};

	//dxcUtilsの初期化
	IDxcUtils* dxcUtils_ = nullptr;

	//dxcCompilerを初期化
	IDxcCompiler3* dxcCompiler_ = nullptr;

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler_ = nullptr;

};