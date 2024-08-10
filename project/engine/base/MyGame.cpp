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
	soundData_ = audio_->SoundLoadWave("resources/se.wav");
}

void MyGame::Finalize() {

	//基底クラスの終了処理
	FrameWork::Finalize();

	//音声データの解放
	audio_->SoundUnLoad(&soundData_);
}

void MyGame::Update() {

	//基底クラスの更新処理
	FrameWork::Update();

	//imguiの更新前処理
	imGuiManager_->Begin();

	//カメラの更新
	camera_->DebugUpdate();

	//3Dオブジェクトの更新
	ball_->Update();

	//ImGuiを起動
	ImGui::Begin("Debug");

	ImGui::Text("LShift + Left Click : Camera Translate");
	ImGui::Text("LShift + Right Click : Camera Rotate");
	ImGui::Text("LShift + MiddleButton Scroll : Camera Distance");

	//カメラのImGui
	if (ImGui::TreeNode("Camera")) {

		camera_->DisplayImGui();

		ImGui::TreePop();

	}

	//モデルのImGui
	if (ImGui::TreeNode("Ball")) {

		ball_->DisplayImGui();

		ImGui::TreePop();
	}

	if (ImGui::Button("Start Audio")) {

		//音声データの再生
		audio_->SoundPlayWave(soundData_);
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

	//Spriteの描画準備
	spriteCommon_->CommonDrawSetting();

	//3DObjectの描画準備
	object3DCommon_->CommonDrawSetting();

	//Object3Dの描画
	ball_->Draw();

	//imguiの描画処理
	imGuiManager_->Draw();

	//描画後処理
	directXCommon_->PostDraw();
}