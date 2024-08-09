#include "Object3D.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Object3DCommon.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "externals/imgui/imgui.h"

#include "Pipeline.h"
#include "Vector3Math.h"

#include "cassert"

void Object3D::Initialize(const std::string& filePath) {

	object3DCommon_ = Object3DCommon::GetInstance();

	model_ = ModelManager::GetInstance()->FindModel(filePath);

	//座標変換行列リソースを作成
	WVPResource_ = object3DCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	//平行光源リソースを作成
	DirectionalLightResource_ = object3DCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));

	//書き込むためのアドレスを取得する
	WVPResource_->Map(0, nullptr, reinterpret_cast<void**>(&WVPData_));
	DirectionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

	//座標変換行列データの設定
	WVPData_->WVP = Pipeline::MakeIdentity4x4();
	WVPData_->World = Pipeline::MakeIdentity4x4();

	//平行光源データの設定
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f }; //色を設定
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f }; //向きを設定
	directionalLightData_->intensity = 1.0f;                //輝度を設定

	//Transformの設定
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	camera_ = object3DCommon_->GetDefaultCamera();

}

void Object3D::Update() {

	Matrix4x4 worldMatrix = Pipeline::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	Matrix4x4 worldViewProjectionMatrix;

	if (camera_) {
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Pipeline::Multiply(worldMatrix, viewProjectionMatrix);
	} else {
		worldViewProjectionMatrix = worldMatrix;
	}

	WVPData_->WVP = worldViewProjectionMatrix;
	WVPData_->World = worldMatrix;

	directionalLightData_->direction = Vector3Math::Normalize(directionalLightData_->direction);
}

void Object3D::Draw() {

	object3DCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, WVPResource_.Get()->GetGPUVirtualAddress());

	object3DCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLightResource_.Get()->GetGPUVirtualAddress());

	//3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw();
	}
}

void Object3D::DisplayImGui() {

	Vector4 color = model_->GetColor();

	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::ColorEdit3("Color", &color.x);

	if (ImGui::TreeNode("Light")) {

		ImGui::DragFloat3("Direction", &directionalLightData_->direction.x, 0.01f);
		ImGui::DragFloat("Intensity", &directionalLightData_->intensity, 0.01f,0.0f,1.0f);
		ImGui::ColorEdit3("Color", &directionalLightData_->color.x);

		ImGui::TreePop();
	}

	model_->SetColor(color);
}

void Object3D::SetModel(const std::string& filePath) {

	model_ = ModelManager::GetInstance()->FindModel(filePath);

}