#include "SceneFactory.h"

#include "TitleScene.h"
#include "GameScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {

	//返り値のシーン
    BaseScene* newScene = nullptr;

	if (sceneName == "TITLE") {

		//タイトルシーンを生成
		newScene = new TitleScene();
	} else if (sceneName == "GAMESCENE") {

		//ゲームシーンを生成
		newScene = new GameScene();
	}

	//生成したシーンを返す
    return newScene;
}