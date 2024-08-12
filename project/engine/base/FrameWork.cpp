#include "FrameWork.h"

void FrameWork::Initialize() {

	//WinAppの静的インスタンスを取得
	winApp_ = WinApp::GetInstance();
	//WinAppの初期化
	winApp_->Initialize();

	//DirectXCommonの静的インスタンスを取得
	directXCommon_ = DirectXCommon::GetInstance();
	//DirectXCommonの初期化
	directXCommon_->Initialize();

	//SrvManagerの静的インスタンスを取得
	srvManager_ = SrvManager::GetInstance();
	//SrvManagerの初期化
	srvManager_->Initialize();

	//SpriteCommonの静的インスタンスを取得
	spriteCommon_ = SpriteCommon::GetInstance();
	//SpriteCommonの初期化
	spriteCommon_->Initialize();

	//Object3DCommonの静的インスタンスを取得
	object3DCommon_ = Object3DCommon::GetInstance();
	//Object3DCommonの初期化
	object3DCommon_->Initialize();

	//ModelCommonの静的インスタンスを取得
	modelCommon_ = ModelCommon::GetInstance();
	//ModelCommonの初期化
	modelCommon_->Initialize();

	//ParticleManagetの静的インスタンスを取得
	particleManager_ = ParticleManager::GetInstance();
	//ParticleManagerの初期化
	particleManager_->Initialize();

	//Inputの静的インスタンスを取得
	input_ = Input::GetInstance();
	//Inputの初期化
	input_->Initialize();

	//Audioの静的インスタンスを取得
	audio_ = Audio::GetInstance();
	//audioの初期化
	audio_->Initialize();

	//ImGuiManagerの静的インスタンスを取得
	imGuiManager_ = ImGuiManager::GetInstance();
	//ImGuiManagerの初期化
	imGuiManager_->Initialize();

	//TextureManagerの初期化
	TextureManager::GetInstance()->Initialize();

	//ModelManagerの初期化
	ModelManager::GetInstance()->Initialize();

}

void FrameWork::Finalize() {

	//シーンファクトリーの解放
	delete sceneFactory_;

	//xAudioの解放
	audio_->Finalize();

	//ImGuiManagerの終了処理
	imGuiManager_->Finalize();

	//WinAppの終了処理
	winApp_->Finalize();
}

void FrameWork::Update() {

	//ウィンドウの✕ボタンが押されたら
	if (winApp_->ProcessMessage()) {

		//終了フラグをtrueにする
		endRequest_ = true;
	}

	//Inputクラスの更新
	input_->Update();

	//シーンマネージャーの更新
	SceneManager::GetInstance()->Update();
}

void FrameWork::Run() {

	//ゲームの初期化
	Initialize();

	//メインループ
	while (true) {

		//毎フレーム更新
		Update();

		//終了リクエストが来たら抜ける
		if (IsEndRequest()) {
			break;
		}

		//描画
		Draw();
	}

	//ゲーム終了
	Finalize();
}