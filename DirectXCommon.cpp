#include "DirectXCommon.h"
#include "WinApp.h"
#include "cassert"
#include "others/Log.h"

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize() {

	winApp = WinApp::GetInstance();

	InitializeDevice();
	InitializeCommand();
	InitializeSwapChain();
	InitializeDepthBuffer();
	InitializeDescriptorHeap();
	InitializeRenderTargetView();
	InitializeDepthStencilView();
	InitializeFence();
	InitializeViewportRect();
	InitializeScissorRect();
	InitializeDXCCompile();
	InitializeImGui();
}

void DirectXCommon::InitializeDevice() {

	HRESULT hr;

#ifdef _DEBUG

	/*デバッグレイヤーを有効化する*/

	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();

		//更にGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG

	/*DXGIFactoryの生成*/

	//DXGIファクトリーの生成
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	//正常に生成できているか確認
	assert(SUCCEEDED(hr));

	/*使用するアダプタ(GPU)を決定する*/

	//使用するアダプト用の変数。最初にnullptrを入れておく
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;

	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {

		DXGI_ADAPTER_DESC3 adapterDesc{};

		//アダプターの情報を取得する
		hr = useAdapter->GetDesc3(&adapterDesc);

		//アダプタのの情報が取得できたかの確認
		assert(SUCCEEDED(hr));

		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {

			//採用されたアダプタ名をログに出力
			OutPutLog(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));

			break;
		}

		//ソフトウェアアダプタの場合は見なかったことにする
		useAdapter = nullptr;
	}

	//適切なアダプタが見つからなかった場合はプログラムを止める
	assert(useAdapter != nullptr);

	/*D3D12Deviceの生成*/

	//機能レベル
	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};

	//ログ出力用の文字列
	const char* featureLevelString[] = { "12.2","12.1","12.0" };

	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {

		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));

		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {

			//生成できた機能レベルをログに出力
			OutPutLog(std::format("FeatureLevel : {}\n", featureLevelString[i]));

			break;
		}
	}

	//デバイスの生成がうまくいかなかった場合はプログラムを止める
	assert(device_ != nullptr);

	//初期化完了のログを出す
	OutPutLog("Complete create D3D12Device\n");

#ifdef _DEBUG

	/*エラー・警告の確認*/

	ID3D12InfoQueue* infoQueue = nullptr;

	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		//警告時に止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//解放
		infoQueue->Release();

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11までのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		/*エラーと警告の抑制*/

		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

		D3D12_INFO_QUEUE_FILTER filter{};

		filter.DenyList.NumIDs = _countof(denyIds);

		filter.DenyList.pIDList = denyIds;

		filter.DenyList.NumSeverities = _countof(severities);

		filter.DenyList.pSeverityList = severities;

		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif // _DEBUG

}

void DirectXCommon::InitializeCommand() {

	HRESULT hr;

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	//コマンドキューを生成する
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));

	//コマンドキューの生成がうまくいったかの確認
	assert(SUCCEEDED(hr));

	//コマンドアロケータを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));

	//コマンドアロケータの生成がうまくいったかの確認
	assert(SUCCEEDED(hr));

	//コマンドリストを生成する
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));

	//コマンドリストの生成がうまくいったかの確認
	assert(SUCCEEDED(hr));

}

void DirectXCommon::InitializeSwapChain() {

	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//スワップチェーンの設定
	swapChainDesc.Width = WinApp::kClientWidth;                  //画面の幅。ウィンドウのクライアント領域と同じものにしておく
	swapChainDesc.Height = WinApp::kClientHeight;                //画面の高さ。ウィンドウのクライアント領域と同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           //色の形式
	swapChainDesc.SampleDesc.Count = 1;                          //マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;                               //ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    //モニタにうつしたら、中身を破棄

	//スワップチェーンを生成する
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));

	//スワップチェーンの生成がうまくいったかの確認
	assert(SUCCEEDED(hr));

}

void DirectXCommon::InitializeDepthBuffer() {

	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = WinApp::kClientWidth;                    //Textureの幅
	resourceDesc.Height = WinApp::kClientHeight;                  //Textureの高さ
	resourceDesc.MipLevels = 1;                                   //mipmapの数
	resourceDesc.DepthOrArraySize = 1;                            //奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;          //DepthSthncilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;                            //サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;  //2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;              //1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //フォーマット。Resourceと合わせる

	//Resourceの生成
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,                  //Heapの設定
		D3D12_HEAP_FLAG_NONE,             //Heapの特殊な設定。特になし
		&resourceDesc,                    //Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値を書き込む状態にしておく
		&depthClearValue,                 //Clear最適値
		IID_PPV_ARGS(&depthStancilResource_)
	);

	assert(SUCCEEDED(hr));

}

void DirectXCommon::InitializeDescriptorHeap() {

	//DescriptorSizeを取得しておく
	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

}

void DirectXCommon::InitializeRenderTargetView() {

	HRESULT hr;

	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));

	//Resourceが取得できたかの確認
	assert(SUCCEEDED(hr));

	//2つ目のResourseを引っ張ってくる
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));

	//Resourceが取得できたかの確認
	assert(SUCCEEDED(hr));

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; //2dテクスチャとして書き込む

	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, 0);

	//まず1つ目を作る・1つ目は最初のところに作る。
	rtvHandles_[0] = rtvStartHandle;

	device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc, rtvHandles_[0]);

	//2つ目のディスクリプタハンドルを得る
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//2つ目を作る
	device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc, rtvHandles_[1]);

}

void DirectXCommon::InitializeDepthStencilView() {

	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; //2DTexture
	//DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(depthStancilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

}

void DirectXCommon::InitializeFence() {

	HRESULT hr;

	//Fenceの初期値
	uint64_t fenceValue = 0;

	hr = device_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));

	//Fenceが作成できたかの確認
	assert(SUCCEEDED(hr));

}

void DirectXCommon::InitializeViewportRect() {

	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<FLOAT>(WinApp::kClientWidth);
	viewport_.Height = static_cast<FLOAT>(WinApp::kClientHeight);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void DirectXCommon::InitializeScissorRect() {

	//基本的にビューポートと同じく矩形が編成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight;
}

void DirectXCommon::InitializeDXCCompile() {

	HRESULT hr;

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));

	assert(SUCCEEDED(hr));

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));

	assert(SUCCEEDED(hr));

	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);

	assert(SUCCEEDED(hr));

}

void DirectXCommon::InitializeImGui() {

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
		device_.Get(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvDescriptorHeap_.Get(),
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
	);

}

void DirectXCommon::PreDraw() {

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	/*ResourceをRTVとして使えるようにする*/

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();

	//遷移前(現在)のResourceState 
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	/*画面をクリアする*/

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCPUDescriptorHandle(dsvDescriptorHeap_, descriptorSizeDSV_, 0);

	//描画先のRTVとDSVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);

	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f }; //青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//コマンドを積んでいく

	//Viewportを設定
	commandList_->RSSetViewports(1, &viewport_);

	//Scissor設定
	commandList_->RSSetScissorRects(1, &scissorRect_);

}

void DirectXCommon::PostDraw() {

	HRESULT hr;

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;

	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	/*コマンドリストの受付を終了する*/

	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList_->Close();

	//コマンドリストをCloseできたかの確認
	assert(SUCCEEDED(hr));

	/*コマンドリストの実行*/

	//今まで積まれてきたコマンドリストをコピー
	ID3D12CommandList* commandLists[] = { commandList_.Get() };

	//GPUにコマンドリストの実行を行わせる
	commandQueue_->ExecuteCommandLists(1, commandLists);

	//GPUとOSに画面の交換を行いように通知する
	swapChain_->Present(1, 0);

	/*GPUの実行完了を待つ*/

	//Fenceの値を更新
	fenceValue_++;

	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompleatedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {

		//FenceのSignalを待つためのイベントを作成する
		/*Fenceのイベント*/
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		//イベントを作成できたかの確認
		assert(fenceEvent != nullptr);

		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent);

		//イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);

		//FenceEventの解放
		CloseHandle(fenceEvent);
	}

	/*次フレームの準備*/

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();

	//コマンドアロケータをリセットできたかの確認
	assert(SUCCEEDED(hr));

	//コマンドリストをコマンドアロケータでリセット
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);

	//コマンドリストをリセットできたかの確認
	assert(SUCCEEDED(hr));

}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType,
	UINT numDescriptors,
	bool shaderVisible
) {

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> result;

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};

	/*ディスクリプタの設定*/

	//種類
	descriptorHeapDesc.Type = heapType;

	//数
	descriptorHeapDesc.NumDescriptors = numDescriptors;

	//ShederVisibleの有無
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	//ディスクリプタヒープの生成
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&result));

	//ディスクリプタヒープの生成がうまくいったかの確認
	assert(SUCCEEDED(hr));

	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

Microsoft::WRL::ComPtr<IDxcBlob> DirectXCommon::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile
) {

	/*hlslファイルを読む*/

	//これからシェーダーをコンパイルする旨をログに出す
	OutPutLog(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));

	//hlslのソースファイル
	IDxcBlobEncoding* shaderSource = nullptr;

	//hlslファイルを読む
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	//読めなかったら止める
	assert(SUCCEEDED(hr));

	//読み込んだファイルの内容を設定する
	//hlslファイルの内容
	DxcBuffer shaderSourceBuffer;

	//ポインター
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();

	//サイズ
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();

	//エンコード形式
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //UTF8の文字コードであることを通知

	/*コンパイルする*/

	//コンパイルの設定
	LPCWSTR arguments[] = {
		filePath.c_str(),        //コンパイル対象のhlslファイル名
		L"-E",L"main",           //エントリーポイントの指定。基本的にmain以外はしない
		L"-T",profile,           //ShaderProfileの指定
		L"-Zi",L"-Qembed_debug", //デバッグ用の情報を埋め込む
		L"-Od",                  //最適化しておく
		L"-Zpr",                 //メモリレイアウトは行優先
	};

	//コンパイル結果
	IDxcResult* shaderResult = nullptr;

	//実際にShaderをコンパイルする
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,        //読み込んだファイル
		arguments,                  //コンパイルオプション
		_countof(arguments),        //コンパイルオプションの数
		includeHandler_,             //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) //コンパイル結果
	);

	//コンパイルエラーではなくdxcが起動できないなど致命的な状態
	assert(SUCCEEDED(hr));

	/*蛍光エラーが出ていないか確認する*/

	//エラー確認用
	IDxcBlobUtf8* shaderError = nullptr;

	//警告・エラーがあるか確認する
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	//警告・エラーが出たらログを出して止める
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {

		//エラーがあったことをログに出力する
		OutPutLog(shaderError->GetStringPointer());

		//プログラムを止める
		assert(false);
	}

	/*コンパイル結果を受け取って返す*/

	//コンパイル結果をBlob(BinaryLargeOBject)として受け取る
	//Blobは大きなバイナリデータのこと

	//コンパイル結果取得用
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;

	//コンパイル結果から実行用のバイナリ部分を取得
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	//成功したログを出す
	OutPutLog(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));

	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();

	//実行用のバイナリを返却
	return shaderBlob;
}

ResourceObject DirectXCommon::CreateBufferResource(size_t sizeInBytes) {

	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //UploadHeapを使う

	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};

	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; //リソースのサイズ
	//バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	//バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//実際に頂点リソースを作る
	ID3D12Resource* vertexResource = nullptr;

	HRESULT hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));

	assert(SUCCEEDED(hr));

	//作った頂点リソースを返す
	return vertexResource;
}