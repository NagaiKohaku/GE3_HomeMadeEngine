#include "Camera.h"
#include "WinApp.h"
#include "Pipeline.h"

#include "externals/imgui/imgui.h"

Camera::Camera()
	:transform_({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
	, fovY_(0.45f)
	, aspectRatio_(static_cast<float>(WinApp::kClientWidth) / static_cast<float>(WinApp::kClientHeight))
	, nearClip_(0.1f)
	, farClip_(100.0f)
	, worldMatrix_(Pipeline::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Pipeline::Inverse(worldMatrix_))
	, projectionMatrix_(Pipeline::MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_))
	, viewProjectionMatrix_(Pipeline::Multiply(viewMatrix_, projectionMatrix_)) {
}

void Camera::Update() {

	worldMatrix_ = Pipeline::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Pipeline::Inverse(worldMatrix_);
	projectionMatrix_ = Pipeline::MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);
	viewProjectionMatrix_ = Pipeline::Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::DisplayImGui() {

	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);

}