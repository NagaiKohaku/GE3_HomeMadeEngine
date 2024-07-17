#pragma once

class DirectXCommon;

class ModelCommon {

public:

	static ModelCommon* GetInstance();

	void Initialize();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:

	DirectXCommon* dxCommon_;

};