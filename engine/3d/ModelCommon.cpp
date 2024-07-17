#include "ModelCommon.h"
#include "DirectXCommon.h"

ModelCommon* ModelCommon::GetInstance() {
	static ModelCommon instance;
	return &instance;
}

void ModelCommon::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();

}