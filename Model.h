#pragma once
#include "wrl.h"

#include "string"
#include "vector"

#include "math/Vector.h"
#include "math/Matrix.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "d3d12.h"

class ModelCommon;

class Model {

public:

	void Initialize();

	void Update();

	void Draw();

	void SetColor(const Vector4& color) { materialData_->color = color; }

	const Vector4& GetColor() { return materialData_->color; }

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

	ModelCommon* modelCommon_;

	//objファイルのデータ
	ModelData modelData_;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;


	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

};