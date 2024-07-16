#pragma once
#include "wrl.h"

#include "string"
#include "vector"

#include "math/Vector.h"
#include "math/Matrix.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "d3d12.h"

class Object3DCommon;

class Object3D {

public:

	static Object3D* GetInstance();

	void Initialize();

	void Update();

	void Draw();

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

	//座標変換行列データ
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	//座標変換データ
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	//平行光源
	struct DirectionalLight {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

	//モデルのマテリアルデータ
	struct MaterialData {

		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	//モデルデータ
	struct ModelData {

		std::vector<VertexData> vertices;
		MaterialData material;
	};

	Object3DCommon* object3DCommon_;

	//objファイルのデータ
	ModelData modelData_;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> WVPResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> DirectionalLightResource_ = nullptr;

	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* WVPData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	Transform transform_;
	Transform cameraTransform_;


	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

};