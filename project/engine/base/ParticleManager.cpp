#include "ParticleManager.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "Camera.h"

#include "Log.h"
#include "Pipeline.h"
#include "Vector3Math.h"

#include "random"
#include "numbers"

const uint32_t ParticleManager::kNumMaxInstance = 100;

const float ParticleManager::kDeltaTime = 1.0f / 60.0f;

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Initialize() {

	directXCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	srvManager_ = SrvManager::GetInstance();

	//ランダムエンジンの初期化
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	randomEngine_ = randomEngine;

	//パイプラインの生成
	CreatePipeLine();

	//頂点リソースの生成
	vertexResource_ = directXCommon_->CreateBufferResource(sizeof(VertexData) * 4);

	//頂点インデックスリソースの生成
	IndexResource_ = directXCommon_->CreateBufferResource(sizeof(uint32_t) * 6);

	//マテリアルリソースの生成
	materialResource_ = directXCommon_->CreateBufferResource(sizeof(Material));

	//頂点バッファビューの作成
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	indexBufferView_.BufferLocation = IndexResource_->GetGPUVirtualAddress();

	//使用するリソースのサイズを設定
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;

	//1頂点当たりのサイズを設定
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//フォーマットを設定
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//頂点リソースに頂点データを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	IndexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	//頂点データの初期化
	//左下
	vertexData_[0].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//左上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[2].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[3].position = { 1.0f,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	//頂点インデックスデータの初期化
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

	//マテリアルデータの設定
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = Pipeline::MakeIdentity4x4();

	particleGroups.reserve(srvManager_->kMaxSRVCount_);

}

void ParticleManager::Update() {

	//ビルボード行列の計算
	Matrix4x4 backToFrontMatrix = Pipeline::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = Pipeline::Multiply(backToFrontMatrix, defaultCamera_->GetWorldMatrix());

	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	//カメラからビュープロジェクション行列を取得
	Matrix4x4 viewProjectionMatrix = defaultCamera_->GetViewProjectionMatrix();

	//すべてのパーティクルグループの処理をする
	for (std::unordered_map<std::string, ParticleGroup>::iterator particleGroupIterator = particleGroups.begin();
		particleGroupIterator != particleGroups.end();) {

		//描画すべきインスタンス数
		particleGroupIterator->second.numInstance = 0;

		//すべてのパーティクルの処理をする
		for (std::list<Particle>::iterator particleIterator = particleGroupIterator->second.particles.begin();
			particleIterator != particleGroupIterator->second.particles.end();) {

			//寿命に達していたら削除する
			if (particleIterator->lifeTime <= particleIterator->currentTime) {
				particleIterator = particleGroupIterator->second.particles.erase(particleIterator);
				continue;
			}

			if (particleGroupIterator->second.useAccelerationField) {

				//場の影響を計算
				if (IsCollision(particleGroupIterator->second.accelerationField.area, particleIterator->transform.translate)) {
					particleIterator->velocity = Vector3Math::Add(particleIterator->velocity, Vector3Math::Multiply(kDeltaTime, particleGroupIterator->second.accelerationField.acceleration));
				}

			}
			//移動処理
			particleIterator->transform.translate = Vector3Math::Add(particleIterator->transform.translate, Vector3Math::Multiply(kDeltaTime, particleIterator->velocity));

			//経過時間を加算
			particleIterator->currentTime += kDeltaTime;

			//経過時間からアルファ値を計算
			float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);

			//インスタンス数を最大数を超えていなかったら処理をする
			if (particleGroupIterator->second.numInstance < kNumMaxInstance) {

				//拡縮行列の計算
				Matrix4x4 scaleMatrix = Pipeline::MakeScaleMatrix(particleIterator->transform.scale);

				//座標行列の計算
				Matrix4x4 translateMatrix = Pipeline::MakeTranslateMatrix(particleIterator->transform.translate);

				//ワールド行列の計算
				Matrix4x4 worldMatrix = Pipeline::Multiply(scaleMatrix, Pipeline::Multiply(billboardMatrix, translateMatrix));

				//ワールドビュープロジェクション行列の合成
				Matrix4x4 worldViewProjectionMatrix = Pipeline::Multiply(worldMatrix, viewProjectionMatrix);

				//インスタンシングデータに書き込む
				particleGroupIterator->second.instancingData[particleGroupIterator->second.numInstance].WVP = worldViewProjectionMatrix;
				particleGroupIterator->second.instancingData[particleGroupIterator->second.numInstance].World = worldMatrix;
				particleGroupIterator->second.instancingData[particleGroupIterator->second.numInstance].color = particleIterator->color;
				particleGroupIterator->second.instancingData[particleGroupIterator->second.numInstance].color.w = alpha;

				++particleGroupIterator->second.numInstance;
			}

			++particleIterator;
		}

		++particleGroupIterator;
	}

}

void ParticleManager::Draw() {

	//ルートシグネチャの設定
	directXCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//PSOの設定
	directXCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());

	//プリミティブトポロジーの設定
	directXCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//VBVを設定
	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	directXCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	//すべてのパーティクルグループの処理をする
	for (std::unordered_map<std::string, ParticleGroup>::iterator particleGroupIterator = particleGroups.begin();
		particleGroupIterator != particleGroups.end();) {

		directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

		directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(particleGroupIterator->second.srvIndex));

		directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(particleGroupIterator->second.material.textureIndex));

		directXCommon_->GetCommandList()->DrawIndexedInstanced(6, particleGroupIterator->second.numInstance, 0, 0, 0);

		++particleGroupIterator;
	}

}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath) {

	//登録済みの名前かチェック
	if (particleGroups.contains(name)) {
		//登録済みなら早期return
		return;
	}

	//あたらしいパーティクルグループを生成
	ParticleGroup& particleGroup = particleGroups[name];

	//マテリアルのテクスチャファイルパスを設定
	particleGroup.material.textureFilePath = textureFilePath;

	//テクスチャを読み込む
	textureManager_->LoadTexture(particleGroup.material.textureFilePath);

	//マテリアルデータにテクスチャのSRVインデックスを記録
	particleGroup.material.textureIndex = textureManager_->GetSrvIndex(particleGroup.material.textureFilePath);

	//インスタンシング用のリソースの生成
	particleGroup.instancingResource = directXCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

	//書き込むためのアドレスを取得
	particleGroup.instancingResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));

	//単位行列を書き込む
	for (uint32_t index = 0; index < kNumMaxInstance; index++) {
		particleGroup.instancingData[index].WVP = Pipeline::MakeIdentity4x4();
		particleGroup.instancingData[index].World = Pipeline::MakeIdentity4x4();
		particleGroup.instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	//インスタンシング用にSRVを確保してSRVインデックスを記録する
	particleGroup.srvIndex = srvManager_->Allocate();

	//インスタンシングのSRVを生成(StructuredBuffer)
	srvManager_->CreateSRVforStructuredBuffer(
		particleGroup.srvIndex,
		particleGroup.instancingResource.Get(),
		kNumMaxInstance,
		sizeof(ParticleForGPU)
	);

	particleGroup.accelerationField.acceleration = { -15.0f,0.0f,0.0f };
	particleGroup.accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	particleGroup.accelerationField.area.max = { 1.0f,1.0f,1.0f };

	particleGroup.useAccelerationField = true;
}

void ParticleManager::CreatePipeLine() {

	HRESULT hr;

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
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	//マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;                   //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                //PixelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                                   //レジスタ番号0とバインド

	//WVP
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      //DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;               //VertexShaderを使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange;             //Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

	//テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                //PixelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;             //Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

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

	hr = directXCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));

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
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	/*RasterizerStateを設定する*/

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	/*Shaderのコンパイル*/

	//VertexShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob =
		directXCommon_->CompileShader(
			L"resources/shaders/Particle.VS.hlsl",
			L"vs_6_0"
		);

	assert(vertexShaderBlob != nullptr);

	//PixelShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob =
		directXCommon_->CompileShader(
			L"resources/shaders/Particle.PS.hlsl",
			L"ps_6_0"
		);

	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	/*PSOを生成する*/

	//PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	//RootSignature
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();

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

	hr = directXCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));

}

bool ParticleManager::IsCollision(const AABB& aabb, const Vector3& point) {

	Vector3 closestPoint{
		std::clamp(point.x,aabb.min.x,aabb.max.x),
		std::clamp(point.y,aabb.min.y,aabb.max.y),
		std::clamp(point.z,aabb.min.z,aabb.max.z)
	};

	float distance = Vector3Math::Length(Vector3Math::Subtract(closestPoint, point));

	if (distance <= 0.0f) {
		return true;
	}

	return false;

}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count) {

	//登録済みの名前かチェック
	if (particleGroups.contains(name)) {

		std::list<Particle> particles;

		for (uint32_t i = 0; i < count; i++) {

			particles.push_back(MakeNewParticle(position));
		}

		particleGroups[name].particles.splice(particleGroups[name].particles.end(), particles);
	}

}

ParticleManager::Particle ParticleManager::MakeNewParticle(const Vector3& translate) {

	Particle particle;

	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	particle.transform.scale = { 1.0f,1.0f,1.0f };
	particle.transform.rotate = { 0.0f,3.14f,0.0f };
	particle.transform.translate = { distribution(randomEngine_),distribution(randomEngine_) ,distribution(randomEngine_) };
	particle.velocity = { distribution(randomEngine_),distribution(randomEngine_),distribution(randomEngine_) };
	particle.color = { distribution(randomEngine_) ,distribution(randomEngine_) ,distribution(randomEngine_) ,1.0f };
	particle.lifeTime = distTime(randomEngine_);
	particle.currentTime = 0.0f;

	particle.transform.translate = Vector3Math::Add(particle.transform.translate, translate);

	return particle;
}

void ParticleManager::SetAcceleration(const std::string name, const Vector3& acceleration) {

	//登録済みの名前かチェック
	if (particleGroups.contains(name)) {

		particleGroups[name].accelerationField.acceleration = acceleration;
	}
}