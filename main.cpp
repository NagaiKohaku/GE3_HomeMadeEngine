#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Input.h"
#include "Sprite.h"

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

	//TextureManagerの初期化
	TextureManager::GetInstance()->Initialize();

	//Inputの静的インスタンスを取得
	Input* input = Input::GetInstance();

	//Inputの初期化
	input->Initialize();

	std::vector<std::unique_ptr<Sprite>> sprites;

	for (uint32_t i = 0; i < 5; i++) {
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize();
		sprites.push_back(std::move(sprite));
	}

	uint32_t uvCheckerTexture = TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	uint32_t monsterBallTexture = TextureManager::GetInstance()->LoadTexture("resources/monsterBall.png");

	Vector2 spritePos[5];
	float spriteRotation[5];
	Vector2 spriteSize[5];
	Vector4 spriteColor[5];

	int num = 0;

	for (std::vector<std::unique_ptr<Sprite>>::iterator sprite = sprites.begin(); sprite != sprites.end(); ++sprite) {

		spritePos[num] = (*sprite)->GetPosition();
		spriteRotation[num] = (*sprite)->GetRotation();
		spriteSize[num] = (*sprite)->GetSize();
		spriteColor[num] = (*sprite)->GetColor();

		spritePos[num] = Vector2(num * 200.0f, 0.0f);
		spriteSize[num] = Vector2(150.0f, 150.0f);

		num++;
	}


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
		ImGui::Begin("window");

		if (ImGui::TreeNode("Sprite1")) {
			ImGui::DragFloat2("Transform", &spritePos[0].x, 0.1f);
			ImGui::DragFloat("Rotation", &spriteRotation[0], 0.1f);
			ImGui::DragFloat2("Size", &spriteSize[0].x, 0.1f);
			ImGui::ColorEdit4("Color", &spriteColor[0].x);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sprite2")) {
			ImGui::DragFloat2("Transform", &spritePos[1].x, 0.1f);
			ImGui::DragFloat("Rotation", &spriteRotation[1], 0.1f);
			ImGui::DragFloat2("Size", &spriteSize[1].x, 0.1f);
			ImGui::ColorEdit4("Color", &spriteColor[1].x);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sprite3")) {
			ImGui::DragFloat2("Transform", &spritePos[2].x, 0.1f);
			ImGui::DragFloat("Rotation", &spriteRotation[2], 0.1f);
			ImGui::DragFloat2("Size", &spriteSize[2].x, 0.1f);
			ImGui::ColorEdit4("Color", &spriteColor[2].x);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sprite4")) {
			ImGui::DragFloat2("Transform", &spritePos[3].x, 0.1f);
			ImGui::DragFloat("Rotation", &spriteRotation[3], 0.1f);
			ImGui::DragFloat2("Size", &spriteSize[3].x, 0.1f);
			ImGui::ColorEdit4("Color", &spriteColor[3].x);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sprite5")) {
			ImGui::DragFloat2("Transform", &spritePos[4].x, 0.1f);
			ImGui::DragFloat("Rotation", &spriteRotation[4], 0.1f);
			ImGui::DragFloat2("Size", &spriteSize[4].x, 0.1f);
			ImGui::ColorEdit4("Color", &spriteColor[4].x);

			ImGui::TreePop();
		}
		num = 0;
		for (std::vector<std::unique_ptr<Sprite>>::iterator sprite = sprites.begin(); sprite != sprites.end(); ++sprite) {
			(*sprite)->SetPosition(spritePos[num]);
			(*sprite)->SetRotation(spriteRotation[num]);
			(*sprite)->SetSize(spriteSize[num]);
			(*sprite)->SetColor(spriteColor[num]);
			num++;
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

		sprites[0]->Draw(uvCheckerTexture);
		sprites[1]->Draw(monsterBallTexture);
		sprites[2]->Draw(uvCheckerTexture);
		sprites[3]->Draw(monsterBallTexture);
		sprites[4]->Draw(uvCheckerTexture);

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