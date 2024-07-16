#pragma once
#include "wrl.h"

#include "string"
#include "vector"

#include "math/Vector.h"
#include "math/Matrix.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "d3d12.h"

class Object3DCommon;

class Model;

class Object3D {

public:

	void Initialize();

	void Update();

	void Draw();

	void DisplayImGui();

	void SetModel(const std::string& filePath);

	void SetScale(const Vector3& scale) { transform_.scale = scale; }

	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	const Vector3& GetScale() const { return transform_.scale; }

	const Vector3& GetRotate() const { return transform_.rotate; }

	const Vector3& GetTranslate() const { return transform_.translate; }

private:

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

	Object3DCommon* object3DCommon_;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> WVPResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> DirectionalLightResource_ = nullptr;

	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* WVPData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;

	//座標データ
	Transform transform_;

	//カメラの座標データ
	Transform cameraTransform_;

	//モデル情報
	Model* model_;

};