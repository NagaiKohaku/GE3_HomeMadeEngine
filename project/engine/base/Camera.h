#pragma once
#include "Vector.h"
#include "Matrix.h"

class Object3D;

class Camera {

public:

	Camera();

	void Update();

	void DisplayImGui();

	void DebugUpdate();

	void SetTrackingObject(Object3D* object) { object_ = object; }

	void ResetTrackingObject() { object_ = nullptr; }

	void SetRotate(const Vector3& rotate);
	void SetTranslate(const Vector3& translate) { pivotPos_ = translate; }
	void SetFovY(const float fovY) { fovY_ = fovY; }
	void SetAspectRatio(const float aspectRatio) { aspectRatio_ = aspectRatio; }
	void SetNearClip(const float nearClip) { nearClip_ = nearClip; }
	void SetFarClip(const float farClip) { farClip_ = farClip; }

	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return pivotPos_; }

private:

	//座標変換データ
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	Object3D* object_;

	Transform transform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	float fovY_;
	float aspectRatio_;
	float nearClip_;
	float farClip_;

	Matrix4x4 matRot_;

	float offsetZ_ = -15.0f;

	Vector3 pivotPos_ = { 0.0f,0.0f,0.0f };

	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
};