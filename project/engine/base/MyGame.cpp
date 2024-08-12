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

	//最初のシーンの生成
	BaseScene* scene = new TitleScene();

	//シーンマネージャーに最初のシーンをセット
	sceneManager_->SetNextScene(scene);

}

void MyGame::Finalize() {

	//基底クラスの終了処理
	FrameWork::Finalize();

}

void MyGame::Update() {

	//imguiの更新前処理
	imGuiManager_->Begin();

	//基底クラスの更新処理
	FrameWork::Update();

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
	sceneManager_->Draw();

	//imguiの描画処理
	imGuiManager_->Draw();

	//描画後処理
	directXCommon_->PostDraw();
}