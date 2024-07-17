#pragma once
#include "Model.h"

#include "memory"
#include "map"

class ModelCommon;

class ModelManager {

public:
	static ModelManager* GetInstance();

	void Initialize();

	void LoadModel(const std::string& filePath);

	Model* FindModel(const std::string& filePath);

private:

	ModelCommon* modelCommon_;

	std::map<std::string, std::unique_ptr<Model>> models_;
};