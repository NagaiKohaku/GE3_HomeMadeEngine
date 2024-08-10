#include "FrameWork.h"
#include "MyGame.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//シーンを生成
	FrameWork* myGame = new MyGame();

	//シーンを実行
	myGame->Run();

	//シーンの解放
	delete myGame;

	return 0;
}