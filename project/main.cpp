#include "FrameWork.h"
#include "GameScene.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//シーンを生成
	FrameWork* gameScene = new GameScene();

	//シーンを実行
	gameScene->Run();

	//シーンの解放
	delete gameScene;

	return 0;
}