#pragma once
#include "stdint.h"
#include "string"

#include "wrl.h"

#include "math/Vector.h"
#include "math//Matrix.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "d3d12.h"

class SpriteCommon;

class Sprite {

public:

	void Initialize(const std::string& filePath);

	void Update();

	void Draw();

	const Vector2& GetPosition() const { return position_; }

	float GetRotation() const { return rotation_; }

	const Vector2& GetSize() const { return size_; }

	const Vector4& GetColor() const { return materialData_->color; }

	const Vector2& GetAnchorPoint() const { return anchorPoint_; }

	bool GetIsFlipX() const { return isFlipX_; }

	bool GetIsFlipY() const { return isFlipY_; }

	const Vector2& GetTextureLeftTop() const { return textureLeftTop_; }

	const Vector2& GetTextureSize() const { return textureSize_; }

	void SetPosition(const Vector2& position) { position_ = position; }

	void SetRotation(float rotation) { rotation_ = rotation; }

	void SetSize(const Vector2& size) { size_ = size; }

	void SetColor(const Vector4& color) { materialData_->color = color; }

	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }

	void SetIsFlipX(const bool isFlipX) { isFlipX_ = isFlipX; }

	void SetIsFlipY(const bool isFlipY) { isFlipY_ = isFlipY; }

	void SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; }

	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; }

	void ChangeTexture(const std::string& filePath);

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

	SpriteCommon* spriteCommon_ = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> WVPResource_ = nullptr;

	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* WVPData_ = nullptr;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	uint32_t textureIndex_;

	Vector2 position_{ 0.0f,0.0f };
	float rotation_ = 0.0f;
	Vector2 size_ = { 640.0f,360.0f };
	Vector2 anchorPoint_ = { 0.0f,0.0f };

	bool isFlipX_ = false;
	bool isFlipY_ = false;

	Vector2 textureLeftTop_ = { 0.0f,0.0f };

	Vector2 textureSize_ = { 100.0f,100.0f };

	void AdjustTextureSize();
};