#pragma warning(push)
//C4023の警告を見なかったことにする
#pragma warning(disable:4023)
#include <fstream>
#include <sstream>
#include "math/Vector.h"
#include "others/Log.h"
#include "math/Pipeline.h"
#include "math/Vector3Math.h"
#include "others/ResourceObject.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#pragma warning(pop)

#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Input.h"

//頂点データ
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

//ローカル情報
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

//マテリアル
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

DirectX::ScratchImage LoadTexture(const std::string& filePath);

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

ResourceObject UploadTextureData(
	ID3D12Resource* texture,
	const DirectX::ScratchImage& mipImages,
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList
);

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	HRESULT hr;

	//WinAppの静的インスタンスを取得
	WinApp* winApp = WinApp::GetInstance();

	//WinAppの初期化
	winApp->Initialize();

	//DirectXCommonの静的インスタンスを取得
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();

	//DirectXCommonの初期化
	directXCommon->Initialize();

	//SpriteCommonの静的インスタンスを取得
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	//SpriteCommonの初期化
	spriteCommon->Initialize();

	//Inputの静的インスタンスを取得
	Input* input = Input::GetInstance();

	//Inputの初期化
	input->Initialize();

	/*RootSignatureを作成*/

	//RootSignatureを作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; //0から始まる
	descriptorRange[0].NumDescriptors = 1; //数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算

	//RootParameterを作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

	//マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;                   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                //PixelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                                   //レジスタ番号0とバインド

	//WVP
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;                   //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;               //VertexShaderを使う
	rootParameters[1].Descriptor.ShaderRegister = 0;                                   //レジスタ番号0とバインド

	//テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      //DesctiptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                //PixelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;             //Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	//ライト
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;                   //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                //PixelShaderを使う
	rootParameters[3].Descriptor.ShaderRegister = 1;                                   //レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;               //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);   //配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; //ありったけのMinMapを使う
	staticSamplers[0].ShaderRegister = 0; //レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

	//エラーが出たら止める
	if (FAILED(hr)) {

		OutPutLog(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));

		assert(false);
	}

	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	hr = directXCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	assert(SUCCEEDED(hr));

	/*InputLayoutを設定する*/

	//InputLayOutを設定する
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//InputLayoutを確定する
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	/*BlendStateを設定する*/

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};

	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	/*RasterizerStateを設定する*/

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	/*Shaderのコンパイル*/

	//VertexShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob =
		directXCommon->CompileShader(
			L"resources/shaders/Object3D.VS.hlsl",
			L"vs_6_0"
		);

	assert(vertexShaderBlob != nullptr);

	//PixelShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob =
		directXCommon->CompileShader(
			L"resources/shaders/Object3D.PS.hlsl",
			L"ps_6_0"
		);

	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	/*PSOを生成する*/

	//PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	//RootSignature
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();

	//InputLayout
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;

	//VertexShader
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };

	//PixelShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//BlendState
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//RasterizerState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;

	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;

	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//実際に生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	hr = directXCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));

	assert(SUCCEEDED(hr));

	/*Model用のリソースの作成*/

	//WVP用のリソースを作る。TransformationMatrix 1つ分のサイズを用意する
	ResourceObject wvpResourceModel = directXCommon->CreateBufferResource(sizeof(TransformationMatrix));

	//データを書き込む
	TransformationMatrix* wvpDataModel = nullptr;

	//書き込むためのアドレスを取得
	wvpResourceModel.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataModel));

	//単位行列で初期化
	wvpDataModel->WVP = Pipeline::MakeIdentity4x4();
	wvpDataModel->World = Pipeline::MakeIdentity4x4();

	//モデルの読み込み
	ModelData modelData = LoadObjFile("resources", "axis.obj");

	//頂点リソースの作成
	ResourceObject vertexResourceModel = directXCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewModel{};
	vertexBufferViewModel.BufferLocation = vertexResourceModel.Get()->GetGPUVirtualAddress(); //リソースの先頭アドレスから使う
	vertexBufferViewModel.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size()); //使用するリソースのサイズは頂点のサイズ
	vertexBufferViewModel.StrideInBytes = sizeof(VertexData); //1頂点当たりのサイズ

	//頂点リソースにデータを書き込む
	VertexData* vertexDataModel = nullptr;
	vertexResourceModel.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataModel)); //書き込むためのアドレスを取得
	std::memcpy(vertexDataModel, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size()); //頂点データをリソースにコピー

	/*Modelのマテリアルの作成*/

	//マテリアル用のリソースを作る
	ResourceObject materialResourceModel = directXCommon->CreateBufferResource(sizeof(Material));

	//マテリアルにデータを書き込む
	Material* materialDateModel = nullptr;

	//書き込むためのアドレスを取得
	materialResourceModel.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDateModel));

	//色を設定
	materialDateModel->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	//ライトをオン
	materialDateModel->enableLighting = true;

	materialDateModel->uvTransform = Pipeline::MakeIdentity4x4();

	//ライト用のリソースを作る
	ResourceObject directionalLightResourceModel = directXCommon->CreateBufferResource(sizeof(DirectionalLight));

	//リソースに書き込むためのデータ
	DirectionalLight* directionalLightDataModel = nullptr;

	//書き込むためのアドレスを取得
	directionalLightResourceModel.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDataModel));

	directionalLightDataModel->color = { 1.0f,1.0f,1.0f,1.0f }; //色を設定
	directionalLightDataModel->direction = { 0.0f,-1.0f,0.0f }; //向きを設定
	directionalLightDataModel->intensity = 1.0f;                //輝度を設定

	//カメラのローカルTransform
	Transform transformCamera{ {1.0f,1.0f,1.0f},{0.5f,0.0f,0.0f},{0.0f,6.0f,-10.0f} };

	Transform transformModel{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//ライトの向き
	Vector3 lightDirection{ 0.0f,-1.0f,0.0f };

	Transform uvTransformModel{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	float speed = 0.1f;

	//ライトの輝度
	float lightIntensity = 1.0f;

	//テクスチャの切り替え用フラグ
	bool useMonsterBall = true;

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ResourceObject textureResource = CreateTextureResource(directXCommon->GetDevice(), metadata);

	ResourceObject intermediateResource = UploadTextureData(textureResource.Get(), mipImages, directXCommon->GetDevice(), directXCommon->GetCommandList());

	//2枚目のTextureを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ResourceObject textureResource2 = CreateTextureResource(directXCommon->GetDevice(), metadata2);

	ResourceObject intermediateResource2 = UploadTextureData(textureResource2.Get(), mipImages2, directXCommon->GetDevice(), directXCommon->GetCommandList());

	//SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//SRVを作成するDescriptorの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(directXCommon->GetSRVDescriptorHeap(), directXCommon->GetSRVDescriptorSize(), 1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(directXCommon->GetSRVDescriptorHeap(), directXCommon->GetSRVDescriptorSize(), 1);
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(directXCommon->GetSRVDescriptorHeap(), directXCommon->GetSRVDescriptorSize(), 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(directXCommon->GetSRVDescriptorHeap(), directXCommon->GetSRVDescriptorSize(), 2);

	//SRVの生成
	directXCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	directXCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);


	///            ///
	/// ゲームループ ///
	///            ///


	while (true) {

		//ウィンドウの✕ボタンが押されるまでループ
		if (winApp->ProcessMessage()) {
			break;
		}

		//ここからImGuiのフレームが始まる
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//Inputクラスの更新
		input->Update();

		//WASDを押したらモデルを動かす
		if (input->PushKey(DIK_W)) {
			transformModel.translate.y += speed;
		}
		if (input->PushKey(DIK_S)) {
			transformModel.translate.y -= speed;
		}
		if (input->PushKey(DIK_D)) {
			transformModel.translate.x += speed;
		}
		if (input->PushKey(DIK_A)) {
			transformModel.translate.x -= speed;
		}

		/*Modelの作成*/

		Matrix4x4 worldMatrixModel = Pipeline::MakeAffineMatrix(transformModel.scale, transformModel.rotate, transformModel.translate);
		Matrix4x4 viewMatrixModel = Pipeline::Inverse(Pipeline::MakeAffineMatrix(transformCamera.scale, transformCamera.rotate, transformCamera.translate));
		Matrix4x4 projectionMatrixModel = Pipeline::MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
		Matrix4x4 worldProjectionMatrixModel = Pipeline::Multiply(worldMatrixModel, Pipeline::Multiply(viewMatrixModel, projectionMatrixModel));

		wvpDataModel->WVP = worldProjectionMatrixModel;
		wvpDataModel->World = worldMatrixModel;

		Matrix4x4 uvTransformMatrixModel = Pipeline::MakeScaleMatrix(uvTransformModel.scale);
		uvTransformMatrixModel = Pipeline::Multiply(uvTransformMatrixModel, Pipeline::MakeRotateZMatrix(uvTransformModel.rotate.z));
		uvTransformMatrixModel = Pipeline::Multiply(uvTransformMatrixModel, Pipeline::MakeTranslateMatrix(uvTransformModel.translate));

		materialDateModel->uvTransform = uvTransformMatrixModel;

		lightDirection = Vector3Math::Normalize(lightDirection);

		directionalLightDataModel->direction = lightDirection;

		directionalLightDataModel->intensity = lightIntensity;


		/*ImGuiの設定*/

		//ImGuiを起動
		ImGui::Begin("window");

		if (ImGui::TreeNode("Camera")) {
			//カメラを動かせるようにする
			ImGui::DragFloat3("translate", &transformCamera.translate.x, 0.01f);

			ImGui::SliderAngle("rotateX", &transformCamera.rotate.x);
			ImGui::SliderAngle("rotateY", &transformCamera.rotate.y);
			ImGui::SliderAngle("rotateZ", &transformCamera.rotate.z);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Light")) {
			//ライトの向き
			ImGui::DragFloat3("lightDirection", &lightDirection.x, 0.01f, -1.0f, 1.0f);
			//ライトの輝度
			ImGui::DragFloat("lightIntensity", &lightIntensity, 0.01f, 0.0f, 1.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Model")) {
			ImGui::DragFloat3("ModelTransform", &transformModel.translate.x, 0.01f);

			ImGui::SliderAngle("ModelRotateX", &transformModel.rotate.x);
			ImGui::SliderAngle("ModelRotateY", &transformModel.rotate.y);
			ImGui::SliderAngle("ModelRotateZ", &transformModel.rotate.z);

			ImGui::DragFloat2("UVTranslate", &uvTransformModel.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformModel.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformModel.rotate.z);

			ImGui::TreePop();
		}

		ImGui::Text("\n WASD : Move Model");

		//ImGuiの終了
		ImGui::End();

		//ImGuiの内部コマンドを生成する
		//ゲームの処理が終わった後でなければならない
		ImGui::Render();


		///                         ///
		/// 画面にオブジェクトを表示する ///
		///                        ///


		//描画前処理
		directXCommon->PreDraw();

		//ImGui描画用のDescriptorHeapの設定
		ID3D12DescriptorHeap* descriptorHeap[] = { directXCommon->GetSRVDescriptorHeap()};

		//RootSignatureを設定。PSOに設定しているけど別途設定が必要
		directXCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		//PSOを設定
		directXCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());

		//ImGuiの設定
		directXCommon->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

		/*モデルの描画*/

		directXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewModel);

		directXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceModel.Get()->GetGPUVirtualAddress());

		directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResourceModel.Get()->GetGPUVirtualAddress());

		directXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

		directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResourceModel.Get()->GetGPUVirtualAddress());

		directXCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		//実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), directXCommon->GetCommandList());

		//描画後処理
		directXCommon->PostDraw();

		///                               ///
		/// 画面にオブジェクトを表示する (終了) ///
		///                              ///

	}


	///                  ///
	/// ゲームループ (終了) ///
	///                  ///


	///               ///
	/// オブジェクト解放 ///
	///               ///


	/*各オブジェクトの開放処理*/

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//WinAppの終了処理
	winApp->Finalize();


	///                     ///
	/// オブジェクト解放 (終了) ///
	///                     ///

	return 0;
}

//Textureを読み込む関数
DirectX::ScratchImage LoadTexture(const std::string& filePath) {

	//テクスチャファイルを読んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの生成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	return mipImages;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData; //構築するModelData
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals; //法線
	std::vector<Vector2> texcoords; //テクスチャ座標
	std::string line; //ファイルから読んだ1行を格納するもの

	VertexData triangle[3];

	//ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	//ファイルが開けたかの確認
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; //先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				//頂点の要素へのIndexは[位置/UV/法線]で格納されているので、分類してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); //区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	return modelData;
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData; //構築するMaterialData
	std::string line; //ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
	assert(file.is_open()); //開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

//DirectX12のTextureResourceを作る
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {

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
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
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
ResourceObject UploadTextureData(
	ID3D12Resource* texture,
	const DirectX::ScratchImage& mipImages,
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList
) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ResourceObject intermediateResource = DirectXCommon::GetInstance()->CreateBufferResource(intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}