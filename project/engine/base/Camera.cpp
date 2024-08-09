#include "Camera.h"
#include "WinApp.h"
#include "Input.h"
#include "Object3D.h"

#include "Pipeline.h"
#include "Vector3Math.h"

#include "externals/imgui/imgui.h"

#include "numbers"

Camera::Camera()
	:transform_({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
	, matRot_(Pipeline::MakeIdentity4x4())
	, pivotPos_({ 0.0f,0.0f,0.0f })
	, offsetZ_(-15.0f)
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

void Camera::DebugUpdate() {

	velocity_ = { 0.0f,0.0f,0.0f };

	if (Input::GetInstance()->isPushKey(DIK_LSHIFT)) {

		//カメラ座標の移動
		if (Input::GetInstance()->IsPushMouseButton(0)) {

			velocity_.x = -Input::GetInstance()->GetMouseVelocity().x * 0.01f;
			velocity_.y = Input::GetInstance()->GetMouseVelocity().y * 0.01f;
		}

		//カメラ座標Z軸の移動
		offsetZ_ += Input::GetInstance()->GetMouseVelocity().z * 0.01f;

		//カメラの回転
		if (Input::GetInstance()->IsPushMouseButton(1)) {

			//今フレームの回転角度
			Matrix4x4 matRotateDelta = Pipeline::MakeIdentity4x4();

			//X軸回転
			matRotateDelta =
				matRotateDelta *
				Pipeline::MakeRotateXMatrix(
					(static_cast<float>(std::numbers::pi) / 180.0f) *
					Input::GetInstance()->GetMouseVelocity().y
				);

			//Y軸回転
			matRotateDelta =
				matRotateDelta *
				Pipeline::MakeRotateYMatrix(
					(static_cast<float>(std::numbers::pi) / 180.0f) *
					Input::GetInstance()->GetMouseVelocity().x
				);

			//Z軸回転
			matRotateDelta =
				matRotateDelta *
				Pipeline::MakeRotateZMatrix(
					(static_cast<float>(std::numbers::pi) / 180.0f) *
					Input::GetInstance()->GetMouseVelocity().z
				);

			//今フレームの回転角度を合成する
			matRot_ = matRotateDelta * matRot_;
		}
	}

	//追従対象がいる場合
	if (object_) {

		//追従対象からカメラまでのオフセット
		Vector3 offset = { 0.0f,0.0f,offsetZ_ };

		//オフセットをカメラの回転に合わせる
		offset = Vector3Math::TransformNormal(offset, matRot_);

		//カメラ座標を追従対象を中心にオフセット分ずらす
		transform_.translate = object_->GetTranslate() + offset;
	} else {

		float speed = 0.3f;

		velocity_ = Vector3Math::TransformNormal(velocity_, matRot_);

		pivotPos_ = pivotPos_ + velocity_;

		//追従対象からカメラまでのオフセット
		Vector3 offset = { 0.0f,0.0f,offsetZ_ };

		//オフセットをカメラの回転に合わせる
		offset = Vector3Math::TransformNormal(offset, matRot_);

		transform_.translate = pivotPos_ + offset;

	}

	//拡縮行列
	Matrix4x4 scaleMatrix = Pipeline::MakeScaleMatrix(transform_.scale);

	//平行移動行列
	Matrix4x4 translateMatrix = Pipeline::MakeTranslateMatrix(transform_.translate);

	//ワールド行列の計算
	worldMatrix_ = scaleMatrix * matRot_ * translateMatrix;

	//ビュー行列の計算
	viewMatrix_ = Pipeline::Inverse(worldMatrix_);

	//プロジェクション行列の計算
	projectionMatrix_ = Pipeline::MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);

	//ビュープロジェクション行列の計算
	viewProjectionMatrix_ = Pipeline::Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::DisplayImGui() {

	ImGui::DragFloat3("Translate", &pivotPos_.x, 0.1f);
	ImGui::DragFloat("offsetZ", &offsetZ_, 0.1f);

}

void Camera::SetRotate(const Vector3& rotate) {

	Matrix4x4 rotateMatrix;

	rotateMatrix =
		Pipeline::MakeRotateXMatrix(rotate.x) *
		Pipeline::MakeRotateYMatrix(rotate.y) *
		Pipeline::MakeRotateZMatrix(rotate.z);

	matRot_ = rotateMatrix;
}