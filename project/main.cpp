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

	std::unique_ptr<ParticleEmitter> emitter;

	////パーティクルグループ
	//particleManager->CreateParticleGroup("particle", "resources/circle.png");

	//particleManager->CreateParticleGroup("uvParticle", "resources/uvChecker.png");

	//particleManager->SetAcceleration("particle", { 15.0f,0.0f,0.0f });

	//particleManager->SetAcceleration("uvParticle", { -15.0f,0.0f,0.0f });

	////スプライト
	//std::unique_ptr<Sprite> sprite;

	//3Dオブジェクト
	std::unique_ptr<Object3D> object1;
	//std::unique_ptr<Object3D> object2;

	//モデル
	std::unique_ptr<Model> model;

	//カメラを生成
	camera = std::make_unique<Camera>();

	camera->SetRotate({ 0.3f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,4.0f,-10.0f });

	object3DCommon->SetDefaultCamera(camera.get());

	particleManager->SetDefaultCamera(camera.get());

	emitter = std::make_unique<ParticleEmitter>();

	////スプライトを生成
	//sprite = std::make_unique<Sprite>();

	////スプライトの初期化
	//sprite->Initialize("resources/uvChecker.png");

	////スプライトの初期設定
	//sprite->SetPosition(Vector2(sprite->GetSize().x / 2.0f, sprite->GetSize().y / 2.0f));
	//sprite->SetAnchorPoint(Vector2(0.5f, 0.5f));

	//3Dオブジェクトの生成
	object1 = std::make_unique<Object3D>();
	//object2 = std::make_unique<Object3D>();

	//3Dオブジェクトの初期化
	object1->Initialize();
	//object2->Initialize();

	//モデルのロード
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	ModelManager::GetInstance()->CreateSphere("monsterBall.png");

	//3Dオブジェクトにモデルを設定する
	object1->SetModel("monsterBall.png");
	//object2->SetModel("axis.obj");

	////3Dオブジェクトの初期設定
	//object1->SetTranslate(Vector3(-1.5f, 0.0f, 0.0f));
	//object1->SetRotate(Vector3(0.0f, static_cast<float>(std::numbers::pi / 180.0f) * 180.0f, 0.0f));
	//object2->SetTranslate(Vector3(1.5f, 0.0f, 0.0f));
	//object2->SetRotate(Vector3(0.0f, static_cast<float>(std::numbers::pi / 180.0f) * 180.0f, 0.0f));

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

		particleManager->Update();

		//Inputクラスの更新
		input->Update();

		//カメラの更新
		camera->Update();

		emitter->Update();

		////スプライトの更新
		//sprite->Update();

		//3Dオブジェクトの更新
		object1->Update();
		//object2->Update();

		//ImGuiを起動
		ImGui::Begin("Debug");

		if (ImGui::TreeNode("Camera")) {

			camera->DisplayImGui();

			ImGui::TreePop();
		}

		////スプライトのImGui
		//if (ImGui::TreeNode("Sprite")) {

		//	//スプライトのデバッグ情報を表示
		//	sprite->DisplayImGui();

		//	//スプライトのテクスチャ変更
		//	if (ImGui::Combo("Texture",&spriteTextureHandler,"uvTexture.png\0monsterBall.png\0\0")) {
		//		
		//		switch (spriteTextureHandler) {
		//		case 0:

		//			sprite->ChangeTexture("resources/uvChecker.png");
		//			
		//			break;
		//		case 1:

		//			sprite->ChangeTexture("resources/monsterBall.png");
		//			
		//			break;
		//		default:
		//			break;
		//		}
		//	}

		//	ImGui::TreePop();
		//}

		//モデルのImGui
		if (ImGui::TreeNode("Object1")) {

			object1->DisplayImGui();

			ImGui::TreePop();
		}

		//if (ImGui::TreeNode("Object2")) {

		//	object2->DisplayImGui();

		//	ImGui::TreePop();

		//}

		//ImGuiの終了
		ImGui::End();

		//imguiの描画前処理
		imGuiManager->End();

		///        ///
		/// 描画処理 ///
		///        ///


		//描画前処理
		directXCommon->PreDraw();

		//Spriteの描画準備
		spriteCommon->CommonDrawSetting();

		//SRVの設定
		srvManager->PreDraw();

		////Spriteの描画
		//sprite->Draw();

		//3DObjectの描画準備
		object3DCommon->CommonDrawSetting();

		//Object3Dの描画
		object1->Draw();
		//object2->Draw();

		particleManager->Draw();

		//imguiの描画処理
		imGuiManager->Draw();

		//描画後処理
		directXCommon->PostDraw();

	}


	///        ///
	/// 終了処理 ///
	///        ///

	//ImGuiManagerの終了処理
	imGuiManager->Finalize();

	//WinAppの終了処理
	winApp->Finalize();

	return 0;
}