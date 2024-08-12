#pragma once
#include "AbstractSceneFactory.h"
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
	/// <param name="sceneName">シーン名</param>
	void ChangeScene(const std::string& sceneName);

	/// <summary>
	/// シーンファクトリーのセッター
	/// </summary>
	/// <param name="sceneFactory">シーンファクトリー</param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:

	//シーンファクトリー(借り物)
	AbstractSceneFactory* sceneFactory_ = nullptr;

	//今のシーン
	BaseScene* scene_ = nullptr;

	//次のシーン
	BaseScene* nextScene_ = nullptr;

};