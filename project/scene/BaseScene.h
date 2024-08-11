#pragma once

#include "Sprite.h"
#include "Object3D.h"
#include "Model.h"
#include "Camera.h"
#include "ParticleEmitter.h"
#include "Audio.h"
#include "SpriteCommon.h"
#include "Object3DCommon.h"
#include "ModelManager.h"
#include "ParticleManager.h"

#include "externals/imgui/imgui.h"

class BaseScene {

public:

	virtual ~BaseScene() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw();

};