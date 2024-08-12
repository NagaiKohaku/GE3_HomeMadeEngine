#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Object3DCommon.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "ParticleManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"

class FrameWork {

public:

	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~FrameWork() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 実行
	/// </summary>
	void Run();

	/// <summary>
	/// 終了フラグのゲッター
	/// </summary>
	/// <returns></returns>
	virtual bool IsEndRequest() { return endRequest_; }

public:

	//ウィンドウ
	WinApp* winApp_ = nullptr;

	//DirectX
	DirectXCommon* directXCommon_ = nullptr;

	//SRV管理
	SrvManager* srvManager_ = nullptr;

	//スプライト汎用
	SpriteCommon* spriteCommon_ = nullptr;

	//3Dオブジェクト汎用
	Object3DCommon* object3DCommon_ = nullptr;

	//モデル汎用
	ModelCommon* modelCommon_ = nullptr;

	//パーティクル管理
	ParticleManager* particleManager_ = nullptr;

	//入力
	Input* input_ = nullptr;

	//音声
	Audio* audio_ = nullptr;

	//ImGui管理
	ImGuiManager* imGuiManager_ = nullptr;

	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

private:

	//終了フラグ
	bool endRequest_ = false;

};