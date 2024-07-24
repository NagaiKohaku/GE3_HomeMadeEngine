#include "ParticleEmitter.h"
#include "ParticleManager.h"

const float ParticleEmitter::kDeltaTime = 1.0f / 60.0f;

ParticleEmitter::ParticleEmitter()
	:transform_({
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f }
		}
	),
	count_(3),
	frequency_(0.3f),
	frequencyTime_(0.0f) {
}

void ParticleEmitter::Update() {

	frequencyTime_ += kDeltaTime;

	std::unordered_map<std::string, ParticleManager::ParticleGroup> particleGroups;

	particleGroups = ParticleManager::GetInstance()->GetParticleGroup();

	if (frequency_ <= frequencyTime_) {

		for (std::unordered_map<std::string, ParticleManager::ParticleGroup>::iterator it = particleGroups.begin();
			it != particleGroups.end();it++) {

			ParticleManager::GetInstance()->Emit(it->first, transform_.translate, count_);
		}

		frequencyTime_ -= frequency_;
	}

}

void ParticleEmitter::Emit() {

	for (std::unordered_map<std::string, ParticleManager::ParticleGroup>::iterator it = ParticleManager::GetInstance()->GetParticleGroup().begin();
		it != ParticleManager::GetInstance()->GetParticleGroup().end();) {

		ParticleManager::GetInstance()->Emit(it->first, transform_.translate, count_);
	}
}