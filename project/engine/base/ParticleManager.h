#pragma once

#include "Vector.h"
#include "Matrix.h"

#include "d3d12.h"

#include "string"
#include "list"
#include "unordered_map"
#include "random"
#include "wrl.h"

class DirectXCommon;

class TextureManager;

class SrvManager;

class Camera;

class ParticleManager {

private:

	//頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	//マテリアル
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	//ローカル情報
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	struct Particle {
		Transform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};

	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	//モデルのマテリアルデータ
	struct MaterialData {
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	struct AABB {
		Vector3 min;
		Vector3 max;
	};

	struct AccelerationField {
		Vector3 acceleration;
		AABB area;
	};

public:

	struct ParticleGroup {
		MaterialData material;
		std::list<Particle> particles;
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
		uint32_t srvIndex;
		uint32_t numInstance;
		ParticleForGPU* instancingData;
		AccelerationField accelerationField;
		bool useAccelerationField;
	};

	static const uint32_t kNumMaxInstance;

	static const float kDeltaTime;

	static ParticleManager* GetInstance();

	void Initialize();

	void Update();

	void Draw();

	void CreateParticleGroup(const std::string name, const std::string textureFilePath);

	void Emit(const std::string name, const Vector3& position, uint32_t count);

	void SetDefaultCamera(Camera* camera) { defaultCamera_ = camera; }

	void SetAcceleration(const std::string name, const Vector3& acceleration);

	Camera* GetDefaultCamera() const { return defaultCamera_; }

	std::unordered_map<std::string, ParticleGroup> GetParticleGroup(){ return particleGroups; }

private:

	DirectXCommon* directXCommon_;

	TextureManager* textureManager_;

	SrvManager* srvManager_;

	Camera* defaultCamera_;

	std::mt19937 randomEngine_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	std::unordered_map<std::string, ParticleGroup> particleGroups;

	void CreatePipeLine();

	bool IsCollision(const AABB& aabb, const Vector3& point);

	Particle MakeNewParticle(const Vector3& translate);

};