#pragma once

#include "Sprite.h"
#include "Object3D.h"
#include "Model.h"
#include "Camera.h"
#include "ParticleEmitter.h"
#include "Audio.h"

#include "memory"

class GameScene {

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:

	//音声データ
	SoundData soundData_;

	//3Dオブジェクト
	std::unique_ptr<Object3D> ball_;
};