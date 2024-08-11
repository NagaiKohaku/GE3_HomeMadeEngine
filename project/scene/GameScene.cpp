#include "GameScene.h"

#include "SpriteCommon.h"
#include "Object3DCommon.h"
#include "ModelManager.h"
#include "ParticleManager.h"

#include "numbers"
#include "externals/imgui/imgui.h"

void GameScene::Initialize() {

	//モデルのロード
	ModelManager::GetInstance()->CreateSphere("monsterBall.png");

	//3Dオブジェクトの生成
	ball_ = std::make_unique<Object3D>();

	//3Dオブジェクトの初期化
	ball_->Initialize("monsterBall.png");

	//3Dオブジェクトの設定
	ball_->SetTranslate({ 0.0f,0.0f,0.0f });

	ball_->SetRotate({ 0.0f,static_cast<float>(std::numbers::pi) / 180.0f * 90.0f,0.0f });

	//音声データの読み込み
	soundData_ = Audio::GetInstance()->SoundLoadWave("resources/se.wav");
}

void GameScene::Finalize() {

	//音声データの解放
	Audio::GetInstance()->SoundUnLoad(&soundData_);
}

void GameScene::Update() {

	//3Dオブジェクトの更新
	ball_->Update();

	//ImGuiを起動
	ImGui::Begin("Scene");

	//モデルのImGui
	if (ImGui::TreeNode("Ball")) {

		ball_->DisplayImGui();

		ImGui::TreePop();
	}

	if (ImGui::Button("Start Audio")) {

		//音声データの再生
		Audio::GetInstance()->SoundPlayWave(soundData_);
	}

	//ImGuiの終了
	ImGui::End();

}

void GameScene::Draw() {

	//Spriteの描画準備
	SpriteCommon::GetInstance()->CommonDrawSetting();

	//3DObjectの描画準備
	Object3DCommon::GetInstance()->CommonDrawSetting();

	//Object3Dの描画
	ball_->Draw();

}