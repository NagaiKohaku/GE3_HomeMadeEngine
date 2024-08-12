#include "TitleScene.h"

#include "SceneManager.h"

#include "numbers"

void TitleScene::Initialize() {
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {

	if (Input::GetInstance()->IsTriggerPushKey(DIK_RETURN)) {

		//ゲームシーンを生成
		BaseScene* scene = new GameScene();

		//シーン切り替え依頼
		sceneManager_->SetNextScene(scene);
	}
}

void TitleScene::Draw() {

	//Spriteの描画準備
	SpriteCommon::GetInstance()->CommonDrawSetting();

	//3DObjectの描画準備
	Object3DCommon::GetInstance()->CommonDrawSetting();

}
