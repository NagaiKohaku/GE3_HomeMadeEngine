#pragma once

#include "Vector.h"

#include "cstdint"

class ParticleEmitter {

public:

	static const float kDeltaTime;

	ParticleEmitter();

	void Initialize();

	void Update();

	void Emit();

private:

	//ローカル情報
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	Transform transform_;
	uint32_t count_;
	float frequency_;
	float frequencyTime_;

};