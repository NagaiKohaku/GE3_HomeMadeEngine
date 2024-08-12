#pragma once
#include "BaseScene.h"
#include "GameScene.h"
#include "TitleScene.h"

class SceneManager {

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager();

	/// <summary>
	/// シングルトンの取得
	/// </summary>
	/// <returns>シングルトン</returns>
	static SceneManager* GetInstance();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 次シーン予約
	/// </summary>
	/// <param name="nextScene">次シーン</param>
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }

private:

	//今のシーン
	BaseScene* scene_ = nullptr;

	//次のシーン
	BaseScene* nextScene_ = nullptr;

};