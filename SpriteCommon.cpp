#include "SpriteCommon.h"

SpriteCommon* SpriteCommon::GetInstance() {
	static SpriteCommon instance;
	return &instance;
}

void SpriteCommon::Initialize() {

}