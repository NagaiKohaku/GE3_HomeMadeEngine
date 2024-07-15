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

	Vector2 position[2];
	float rotation[2];
	Vector2 size[2];
	Vector4 color[2];
	Vector2 anchorPoint[2];
	bool isFlipX[2];
	bool isFlipY[2];
	Vector2 textureLeftTop[2];
	Vector2 textureSize[2];
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

	for (uint32_t i = 0; i < 2; i++) {
		position[i] = sprites[i]->GetPosition();
		rotation[i] = sprites[i]->GetRotation();
		size[i] = sprites[i]->GetSize();
		color[i] = sprites[i]->GetColor();
		anchorPoint[i] = sprites[i]->GetAnchorPoint();
		isFlipX[i] = sprites[i]->GetIsFlipX();
		isFlipY[i] = sprites[i]->GetIsFlipY();
		textureLeftTop[i] = sprites[i]->GetTextureLeftTop();
		textureSize[i] = sprites[i]->GetTextureSize();
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
			ImGui::DragFloat2("Position", &position[0].x, 0.1f);
			ImGui::SliderAngle("Rotation", &rotation[0]);
			ImGui::DragFloat2("Size", &size[0].x, 0.1f);
			ImGui::ColorEdit4("Color", &color[0].x);
			ImGui::DragFloat2("AnchorPoint", &anchorPoint[0].x, 0.1f);
			ImGui::Checkbox("IsFlipX", &isFlipX[0]);
			ImGui::Checkbox("IsFlipY", &isFlipY[0]);
			ImGui::DragFloat2("TexLeftTop", &textureLeftTop[0].x, 0.1f);
			ImGui::DragFloat2("TexSize", &textureSize[0].x, 0.1f);

			if (ImGui::Combo("Texture",&texture[0],"uvTexture.png\0monsterBall.png\0\0")) {
				switch (texture[0]) {
				case 0:
					sprites[0]->ChangeTexture("resources/uvChecker.png");
					size[0] = sprites[0]->GetSize();
					textureSize[0] = sprites[0]->GetTextureSize();
					break;
				case 1:
					sprites[0]->ChangeTexture("resources/monsterBall.png");
					size[0] = sprites[0]->GetSize();
					textureSize[0] = sprites[0]->GetTextureSize();
					break;
				default:
					break;
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sprite2")) {
			ImGui::DragFloat2("Position", &position[1].x, 0.1f);
			ImGui::SliderAngle("Rotation", &rotation[1]);
			ImGui::DragFloat2("Size", &size[1].x, 0.1f);
			ImGui::ColorEdit4("Color", &color[1].x);
			ImGui::DragFloat2("AnchorPoint", &anchorPoint[1].x, 0.1f);
			ImGui::Checkbox("IsFlipX", &isFlipX[1]);
			ImGui::Checkbox("IsFlipY", &isFlipY[1]);
			ImGui::DragFloat2("TexLeftTop", &textureLeftTop[1].x, 0.1f);
			ImGui::DragFloat2("TexSize", &textureSize[1].x, 0.1f);

			if (ImGui::Combo("Texture", &texture[0], "uvTexture.png\0monsterBall.png\0\0")) {
				switch (texture[0]) {
				case 0:
					sprites[0]->ChangeTexture("resources/uvChecker.png");
					size[0] = sprites[0]->GetSize();
					textureSize[0] = sprites[0]->GetTextureSize();
					break;
				case 1:
					sprites[0]->ChangeTexture("resources/monsterBall.png");
					size[0] = sprites[0]->GetSize();
					textureSize[0] = sprites[0]->GetTextureSize();
					break;
				default:
					break;
				}
			}

			ImGui::TreePop();
		}

		for (uint32_t i = 0; i < 2; i++) {
			sprites[i]->SetPosition(position[i]);
			sprites[i]->SetRotation(rotation[i]);
			sprites[i]->SetSize(size[i]);
			sprites[i]->SetColor(color[i]);
			sprites[i]->SetAnchorPoint(anchorPoint[i]);
			sprites[i]->SetIsFlipX(isFlipX[i]);
			sprites[i]->SetIsFlipY(isFlipY[i]);
			sprites[i]->SetTextureLeftTop(textureLeftTop[i]);
			sprites[i]->SetTextureSize(textureSize[i]);
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