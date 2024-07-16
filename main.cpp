#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Input.h"
#include "Sprite.h"
#include "Object3DCommon.h"
#include "Object3D.h"

#include "math/Vector.h"
#include "others/Log.h"
#include "math/Pipeline.h"
#include "math/Vector3Math.h"

#include "memory"
#include "vector"

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

	//TextureManagerの初期化
	TextureManager::GetInstance()->Initialize();

	//Inputの静的インスタンスを取得
	Input* input = Input::GetInstance();

	//Inputの初期化
	input->Initialize();

	std::vector<std::unique_ptr<Sprite>> sprites;

	int texture[2];

	for (uint32_t i = 0; i < 2; i++) {
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprites.push_back(std::move(sprite));
	}

	sprites[0]->Initialize("resources/monsterBall.png");
	sprites[1]->Initialize("resources/uvChecker.png");

	sprites[0]->SetPosition(Vector2(sprites[0]->GetSize().x / 2.0f, sprites[0]->GetSize().y / 2.0f));
	sprites[0]->SetAnchorPoint(Vector2(0.5f, 0.5f));

	sprites[1]->SetPosition(Vector2(sprites[1]->GetSize().x / 2.0f, sprites[1]->GetSize().y / 2.0f));
	sprites[1]->SetAnchorPoint(Vector2(0.5f, 0.5f));

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

		for (std::vector<std::unique_ptr<Sprite>>::iterator sprite = sprites.begin(); sprite != sprites.end(); ++sprite) {
			(*sprite)->Update();
		}

		/*ImGuiの設定*/

		//ImGuiを起動
		ImGui::Begin("Debug");

		if (ImGui::TreeNode("Sprite1")) {

			sprites[0]->DisplayImGui();

			if (ImGui::Combo("Texture",&texture[0],"uvTexture.png\0monsterBall.png\0\0")) {
				
				switch (texture[0]) {
				case 0:

					sprites[0]->ChangeTexture("resources/uvChecker.png");
					
					break;
				case 1:

					sprites[0]->ChangeTexture("resources/monsterBall.png");
					
					break;
				default:
					break;
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sprite2")) {

			sprites[1]->DisplayImGui();

			if (ImGui::Combo("Texture", &texture[1], "uvTexture.png\0monsterBall.png\0\0")) {

				switch (texture[1]) {
				case 0:

					sprites[1]->ChangeTexture("resources/uvChecker.png");

					break;
				case 1:

					sprites[1]->ChangeTexture("resources/monsterBall.png");

					break;
				default:
					break;
				}
			}

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

		sprites[0]->Draw();
		sprites[1]->Draw();
		
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