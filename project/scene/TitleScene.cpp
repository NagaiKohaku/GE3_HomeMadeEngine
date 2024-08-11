#include "TitleScene.h"

#include "SpriteCommon.h"
#include "Object3DCommon.h"
#include "ModelManager.h"
#include "ParticleManager.h"

#include "numbers"
#include "externals/imgui/imgui.h"

void TitleScene::Initialize() {
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {
}

void TitleScene::Draw() {

	//Spriteの描画準備
	SpriteCommon::GetInstance()->CommonDrawSetting();

	//3DObjectの描画準備
	Object3DCommon::GetInstance()->CommonDrawSetting();

}
