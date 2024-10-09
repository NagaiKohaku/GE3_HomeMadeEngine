#pragma once
#include "BaseScene.h"

#include "string"

class AbstractSceneFactory {

public:

	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>シーン</returns>
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;

};