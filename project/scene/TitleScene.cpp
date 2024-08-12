#include "TitleScene.h"

#include "SceneManager.h"

#include "numbers"

void TitleScene::Initialize() {
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {

	//Enterキーが押されたら
	if (Input::GetInstance()->IsTriggerPushKey(DIK_RETURN)) {

		//ゲームシーンに切り替え
		SceneManager::GetInstance()->ChangeScene("GAMESCENE");
	}
}

void TitleScene::Draw() {

	//Spriteの描画準備
	SpriteCommon::GetInstance()->CommonDrawSetting();

	//3DObjectの描画準備
	Object3DCommon::GetInstance()->CommonDrawSetting();

}
