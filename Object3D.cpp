#include "Object3D.h"
#include "Object3DCommon.h"
#include "DirectXCommon.h"
#include "WinApp.h"
#include "Model.h"
#include "math/Pipeline.h"

#include "cassert"

void Object3D::Initialize() {

	object3DCommon_ = Object3DCommon::GetInstance();

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
	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };

}

void Object3D::Update() {

	Matrix4x4 worldMatrix = Pipeline::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	Matrix4x4 viewMatrix = Pipeline::Inverse(Pipeline::MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate));

	Matrix4x4 projectionMatrix = Pipeline::MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);

	WVPData_->WVP = Pipeline::Multiply(worldMatrix, Pipeline::Multiply(viewMatrix, projectionMatrix));
	WVPData_->World = worldMatrix;
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
	ImGui::ColorEdit4("Color", &color.x);

	model_->SetColor(color);
}