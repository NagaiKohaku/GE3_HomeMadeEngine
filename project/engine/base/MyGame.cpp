#include "MyGame.h"

#include "Vector.h"
#include "Log.h"
#include "Pipeline.h"
#include "Vector3Math.h"

#include "numbers"

void MyGame::Initialize() {

	//基底クラスの初期化
	FrameWork::Initialize();

	//カメラを生成
	camera_ = std::make_unique<Camera>();

	//カメラの設定
	camera_->SetTranslate({ 0.0f,0.0f,0.0f });

	camera_->SetRotate({ 0.0f,static_cast<float>(std::numbers::pi) / 180.0f * 180.0f,0.0f });

	//3Dオブジェクトのカメラ情報を設定
	object3DCommon_->SetDefaultCamera(camera_.get());

	//ゲームシーンの生成
	scene_ = new GameScene();

	//ゲームシーンの初期化
	scene_->Initialize();
}

void MyGame::Finalize() {

	//基底クラスの終了処理
	FrameWork::Finalize();

	//ゲームシーンの終了処理
	scene_->Finalize();

	//ゲームシーンの解放
	delete scene_;
}

void MyGame::Update() {

	//基底クラスの更新処理
	FrameWork::Update();

	//imguiの更新前処理
	imGuiManager_->Begin();

	//カメラの更新
	camera_->DebugUpdate();

	//ImGuiを起動
	ImGui::Begin("Camera");

	ImGui::Text("LShift + Left Click : Camera Translate");
	ImGui::Text("LShift + Right Click : Camera Rotate");
	ImGui::Text("LShift + MiddleButton Scroll : Camera Distance");

	//カメラのImGui
	if (ImGui::TreeNode("Camera")) {

		camera_->DisplayImGui();

		ImGui::TreePop();

	}

	//ゲームシーンのの更新
	scene_->Update();

	//ImGuiの終了
	ImGui::End();

	//imguiの描画前処理
	imGuiManager_->End();
}

void MyGame::Draw() {

	//描画前処理
	directXCommon_->PreDraw();

	//SRVの設定
	srvManager_->PreDraw();

	//ゲームシーンの描画
	scene_->Draw();

	//imguiの描画処理
	imGuiManager_->Draw();

	//描画後処理
	directXCommon_->PostDraw();
}