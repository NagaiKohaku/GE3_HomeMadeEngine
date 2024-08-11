#pragma once
#include "FrameWork.h"
#include "BaseScene.h"
#include "GameScene.h"
#include "TitleScene.h"

#include "ModelManager.h"
#include "ParticleManager.h"

#include "Sprite.h"
#include "Object3D.h"
#include "Model.h"
#include "Camera.h"
#include "ParticleEmitter.h"

#include "memory"

class MyGame : public FrameWork {

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:

	//ゲームシーン
	BaseScene* scene_ = nullptr;

	//カメラ
	std::unique_ptr<Camera> camera_;

};