#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Input.h"
#include "Sprite.h"
#include "Object3DCommon.h"
#include "Object3D.h"
#include "ModelCommon.h"
#include "Model.h"
#include "ModelManager.h"

#include "math/Vector.h"
#include "others/Log.h"
#include "math/Pipeline.h"
#include "math/Vector3Math.h"

#include "memory"
#include "vector"
#include "numbers"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//WinAppの静的インスタンスを取得
	WinApp* winApp = WinApp::GetInstance();

	//WinAppの初期化
	winApp->Initialize();

	//DirectXCommonの静的インスタンスを取得
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();

	//DirectXCommonの初期化
	directXCommon->Initialize();

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

	//TextureManagerの初期化
	TextureManager::GetInstance()->Initialize();

	//ModelManagerの初期化
	ModelManager::GetInstance()->Initialize();

	//Inputの静的インスタンスを取得
	Input* input = Input::GetInstance();

	//Inputの初期化
	input->Initialize();

	//スプライト
	std::unique_ptr<Sprite> sprite;

	//3Dオブジェクト
	std::unique_ptr<Object3D> object1;
	std::unique_ptr<Object3D> object2;

	//モデル
	std::unique_ptr<Model> model;

	//スプライトのテクスチャハンドラー
	int spriteTextureHandler;

	//スプライトを生成
	sprite = std::make_unique<Sprite>();

	//スプライトの初期化
	sprite->Initialize("resources/uvChecker.png");

	//スプライトの初期設定
	sprite->SetPosition(Vector2(sprite->GetSize().x / 2.0f, sprite->GetSize().y / 2.0f));
	sprite->SetAnchorPoint(Vector2(0.5f, 0.5f));

	//3Dオブジェクトの生成
	object1 = std::make_unique<Object3D>();
	object2 = std::make_unique<Object3D>();

	//3Dオブジェクトの初期化
	object1->Initialize();
	object2->Initialize();

	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	//3Dオブジェクトにモデルを設定する
	object1->SetModel("plane.obj");
	object2->SetModel("axis.obj");

	//3Dオブジェクトの初期設定
	object1->SetTranslate(Vector3(-1.5f, 0.0f, 0.0f));
	object1->SetRotate(Vector3(0.0f, static_cast<float>(std::numbers::pi / 180.0f) * 180.0f, 0.0f));
	object2->SetTranslate(Vector3(1.5f, 0.0f, 0.0f));
	object2->SetRotate(Vector3(0.0f, static_cast<float>(std::numbers::pi / 180.0f) * 180.0f, 0.0f));

	///            ///
	/// ゲームループ ///
	///            ///


	while (true) {

		//ウィンドウの✕ボタンが押されるまでループ
		if (winApp->ProcessMessage()) {
			break;
		}

		//ここからImGuiのフレームが始まる
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//Inputクラスの更新
		input->Update();

		//スプライトの更新
		sprite->Update();

		//3Dオブジェクトの更新
		object1->Update();
		object2->Update();

		//ImGuiを起動
		ImGui::Begin("Debug");

		//スプライトのImGui
		if (ImGui::TreeNode("Sprite")) {

			//スプライトのデバッグ情報を表示
			sprite->DisplayImGui();

			//スプライトのテクスチャ変更
			if (ImGui::Combo("Texture",&spriteTextureHandler,"uvTexture.png\0monsterBall.png\0\0")) {
				
				switch (spriteTextureHandler) {
				case 0:

					sprite->ChangeTexture("resources/uvChecker.png");
					
					break;
				case 1:

					sprite->ChangeTexture("resources/monsterBall.png");
					
					break;
				default:
					break;
				}
			}

			ImGui::TreePop();
		}

		//モデルのImGui
		if (ImGui::TreeNode("Object1")) {

			object1->DisplayImGui();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Object2")) {

			object2->DisplayImGui();

			ImGui::TreePop();

		}

		//ImGuiの終了
		ImGui::End();

		//ImGuiの内部コマンドを生成する
		//ゲームの処理が終わった後でなければならない
		ImGui::Render();


		///                         ///
		/// 画面にオブジェクトを表示する ///
		///                        ///


		//描画前処理
		directXCommon->PreDraw();

		//Spriteの描画準備
		spriteCommon->CommonDrawSetting();

		//ImGui描画用のDescriptorHeapの設定
		ID3D12DescriptorHeap* descriptorHeap[] = { directXCommon->GetSRVDescriptorHeap() };

		//ImGuiの設定
		directXCommon->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);

		//Spriteの描画
		sprite->Draw();
		
		//3DObjectの描画準備
		object3DCommon->CommonDrawSetting();

		//Object3Dの描画
		object1->Draw();
		object2->Draw();

		//実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), directXCommon->GetCommandList());

		//描画後処理
		directXCommon->PostDraw();

		///                               ///
		/// 画面にオブジェクトを表示する (終了) ///
		///                              ///

	}


	///                  ///
	/// ゲームループ (終了) ///
	///                  ///


	///               ///
	/// オブジェクト解放 ///
	///               ///


	/*各オブジェクトの開放処理*/

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//WinAppの終了処理
	winApp->Finalize();


	///                     ///
	/// オブジェクト解放 (終了) ///
	///                     ///

	return 0;
}