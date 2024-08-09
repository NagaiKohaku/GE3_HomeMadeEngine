#include "WinApp.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "Input.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3DCommon.h"
#include "Object3D.h"
#include "ModelCommon.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "ImGuiManager.h"
#include "Audio.h"

#include "Vector.h"
#include "Log.h"
#include "Pipeline.h"
#include "Vector3Math.h"

#include "memory"
#include "vector"
#include "numbers"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


	///               ///
	/// エンジンの初期化 ///
	///              ///


	//WinAppの静的インスタンスを取得
	WinApp* winApp = WinApp::GetInstance();
	//WinAppの初期化
	winApp->Initialize();

	//DirectXCommonの静的インスタンスを取得
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	//DirectXCommonの初期化
	directXCommon->Initialize();

	//SrvManagerの静的インスタンスを取得
	SrvManager* srvManager = SrvManager::GetInstance();
	//SrvManagerの初期化
	srvManager->Initialize();

	//SpriteCommonの静的インスタンスを取得
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();
	//SpriteCommonの初期化
	spriteCommon->Initialize();

	//Object3DCommonの静的インスタンスを取得
	Object3DCommon* object3DCommon = Object3DCommon::GetInstance();
	//Object3DCommonの初期化
	object3DCommon->Initialize();

	//ModelCommonの静的インスタンスを取得
	ModelCommon* modelCommon = ModelCommon::GetInstance();
	//ModelCommonの初期化
	modelCommon->Initialize();

	//ParticleManagetの静的インスタンスを取得
	ParticleManager* particleManager = ParticleManager::GetInstance();
	//ParticleManagerの初期化
	particleManager->Initialize();

	//Inputの静的インスタンスを取得
	Input* input = Input::GetInstance();
	//Inputの初期化
	input->Initialize();

	//Audioの静的インスタンスを取得
	Audio* audio = Audio::GetInstance();
	//audioの初期化
	audio->Initialize();

	//ImGuiManagerの静的インスタンスを取得
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();
	//ImGuiManagerの初期化
	imGuiManager->Initialize();

	//TextureManagerの初期化
	TextureManager::GetInstance()->Initialize();

	//ModelManagerの初期化
	ModelManager::GetInstance()->Initialize();


	///          ///
	/// 初期化処理 ///
	///          ///


	//カメラ
	std::unique_ptr<Camera> camera;

	//3Dオブジェクト
	std::unique_ptr<Object3D> ball;

	//カメラを生成
	camera = std::make_unique<Camera>();

	//カメラの設定
	camera->SetTranslate({ 0.0f,0.0f,0.0f });

	camera->SetRotate({ 0.0f,static_cast<float>(std::numbers::pi) / 180.0f * 180.0f,0.0f });

	//3Dオブジェクトのカメラ情報を設定
	object3DCommon->SetDefaultCamera(camera.get());

	//モデルのロード
	ModelManager::GetInstance()->CreateSphere("monsterBall.png");

	//3Dオブジェクトの生成
	ball = std::make_unique<Object3D>();

	//3Dオブジェクトの初期化
	ball->Initialize("monsterBall.png");

	//3Dオブジェクトの設定
	ball->SetTranslate({ 0.0f,0.0f,0.0f });

	ball->SetRotate({ 0.0f,static_cast<float>(std::numbers::pi) / 180.0f * 90.0f,0.0f });

	//音声データの読み込み
	SoundData soundData = audio->SoundLoadWave("resources/se.wav");

	///            ///
	/// ゲームループ ///
	///            ///


	while (true) {

		//ウィンドウの✕ボタンが押されるまでループ
		if (winApp->ProcessMessage()) {
			break;
		}


		///        ///
		/// 更新処理 ///
		///        ///


		//imguiの更新前処理
		imGuiManager->Begin();

		//Inputクラスの更新
		input->Update();

		//カメラの更新
		camera->DebugUpdate();

		//3Dオブジェクトの更新
		ball->Update();

		//ImGuiを起動
		ImGui::Begin("Debug");

		ImGui::Text("LShift + Left Click : Camera Translate");
		ImGui::Text("LShift + Right Click : Camera Rotate");
		ImGui::Text("LShift + MiddleButton Scroll : Camera Distance");

		//カメラのImGui
		if (ImGui::TreeNode("Camera")) {

			camera->DisplayImGui();

			ImGui::TreePop();

		}

		//モデルのImGui
		if (ImGui::TreeNode("Ball")) {

			ball->DisplayImGui();

			ImGui::TreePop();
		}

		if (ImGui::Button("Start Audio")) {

			//音声データの再生
			audio->SoundPlayWave(soundData);
		}

		//ImGuiの終了
		ImGui::End();

		//imguiの描画前処理
		imGuiManager->End();


		///        ///
		/// 描画処理 ///
		///        ///


		//描画前処理
		directXCommon->PreDraw();

		//SRVの設定
		srvManager->PreDraw();

		//Spriteの描画準備
		spriteCommon->CommonDrawSetting();

		//3DObjectの描画準備
		object3DCommon->CommonDrawSetting();

		//Object3Dの描画
		ball->Draw();

		//imguiの描画処理
		imGuiManager->Draw();

		//描画後処理
		directXCommon->PostDraw();

	}


	///        ///
	/// 終了処理 ///
	///        ///

	//xAudioの解放
	audio->Finalize();

	//音声データの解放
	audio->SoundUnLoad(&soundData);

	//ImGuiManagerの終了処理
	imGuiManager->Finalize();

	//WinAppの終了処理
	winApp->Finalize();

	return 0;
}