#include "SceneManager.h"

SceneManager::~SceneManager() {

	//最後のシーンの終了と解放
	scene_->Finalize();
	delete scene_;
}

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Update() {

	//次のシーンの予約があるなら
	if (nextScene_) {

		//旧シーンの終了
		if (scene_) {

			scene_->Finalize();
			delete scene_;
		}

		//シーン切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;

		//次シーンを初期化する
		scene_->Initialize();
	}

	//シーンマネージャーをセット
	scene_->SetSceneManager(this);

	//実行中シーンを更新する
	scene_->Update();

}

void SceneManager::Draw() {

	//実行中シーンを描画する
	scene_->Draw();
}